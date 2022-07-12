/*
 * S C R O L I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiSimpleScrollableList

    Controls the display of a list of GuiDisplayables, i.e. displays
	a subset of the list based on an offset into the list and the
	max number of items that can be displayed at anyone time ( constrained
	by the size of this GuiDisplayable ).

	Various widgets can be attached to this control to enable interactive
	scrolling of the list. See GuiVerticalScrollBar and GuiScrollButton.
*/

#ifndef _GUI_SCROLIST_HPP
#define _GUI_SCROLIST_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

template<class T> class ctl_pvector;

class GuiListObserver;
class GuiSimpleScrollableListImpl;

class GuiSimpleScrollableList : public GuiDisplayable
// Canonical form revoked
{
public:
    GuiSimpleScrollableList(GuiDisplayable* pParent, const Gui::Box&, 
    						size_t horizontalSpacing, size_t verticalSpacing,
    						size_t scrollInc );
    ~GuiSimpleScrollableList();

    void CLASS_INVARIANT;

	// Scroll towards the end of the list ( list = child GuiDisplayables ) 
	bool canScrollFoward() const;

	// Scroll towards the beginning of the list ( list = child GuiDisplayables )
	bool canScrollBackward() const;

	// True if canScrollFoward or canScrollBackward 
	bool canScroll() const;

	void scrollFoward();
	// PRE( canScrollFoward() );

	void scrollBackward();
	// PRE( canScrollBackward() );

	void scrollTo( size_t scrollPos );
	// PRE( canScroll() );
	// PRE( scrollPos < numListItems() );

	// Scroll to the end of the list ( list = child GuiDisplayables )
	void scrollToEnd();

	// Scroll to the beginning of the list ( list = child GuiDisplayables )
	void scrollToBegin();

	// Call whenever child GuiDisplayables are added or removed ( except for when a
	// single child is added, in this case use 'childAdded' ).
	void childrenUpdated();	

	// Call this instead of 'childrenUpdated' if you have added
	// a single child to the scrollable list. This is more efficient
	void childAdded();

	void doDisplay();

	// Returns the number of position that will be scrolled with one call to either
	// scrollRight or scrollLeft
	size_t scrollIncrement() const;

	// Returns the current start position for visible list box items.
	size_t scrollOffset() const;

	// Returns the maximum number of list box items that can be visible at anyone time.
	size_t visiblePositions() const; 

	// Returns the number of items in the list.
	size_t numListItems() const; 

	// Add/Remove a GuiListObserver. GuiListObservers get notified of changes to the list
	// box, for example when a scroll event occurs, a list item is added etc. This is useful
	// for keeping scroll bars, scroll buttons up-to-date.
	void notifyMe( GuiListObserver* );
	void dontNotifyMe( GuiListObserver* );

protected:
	void notifyObservers();

private:
    GuiSimpleScrollableList( const GuiSimpleScrollableList& );
    GuiSimpleScrollableList& operator =( const GuiSimpleScrollableList& );
    bool operator ==( const GuiSimpleScrollableList& );

    friend ostream& operator <<( ostream& o, const GuiSimpleScrollableList& t );

	void updateInfo();

	typedef ctl_pvector< GuiListObserver > Observers;

    GuiSimpleScrollableListImpl * pImpl_;

	friend class GuiSimpleScrollableListImpl;
};


#endif

/* End SCROLIST.HPP *************************************************/
