/*
 * S C R O L B A R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiVerticalScrollBar

    +--------+
	|        |<- scroll bar
	|        |
	|        |
    +--------+
	|////////|<- scroll box
	|////////|
    +--------+
	|        |
	|        |
    +--------+

*/

#ifndef _GUI_SCROLBAR_HPP
#define _GUI_SCROLBAR_HPP

#include "base/base.hpp"
#include "gui/listobvr.hpp"
#include "gui/displaya.hpp"

class GuiVerticalScrollBar : public GuiDisplayable, public GuiListObserver
// Canonical form revoked
{
public:
    GuiVerticalScrollBar( GuiDisplayable* pParent, const Gui::Box& area, GuiSimpleScrollableList* pList );
	// PRE( pList )

    ~GuiVerticalScrollBar();

    void CLASS_INVARIANT;

protected:
	virtual void listUpdated();

	virtual void doDisplay();
	virtual void doDisplayBar();
	virtual void doDisplayBox( const Gui::Box& absoluteBox );

	// Enables derived scroll bars to specify a minimum size ( in pixels ) for the scroll 
	// box. Default, i.e. this implementation, is set to 7.
	virtual size_t minBoxHeight() const; 

	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );

	Gui::Box boxAbsPosition() const;
	Gui::Box boxRelPosition() const;

	void scrollToMousePos( const GuiMouseEvent& rel );

private:
    friend ostream& operator <<( ostream& o, const GuiVerticalScrollBar& t );

    GuiVerticalScrollBar( const GuiVerticalScrollBar& );
    GuiVerticalScrollBar& operator =( const GuiVerticalScrollBar& );

	Gui::Box boxAbsPosition_;
	Gui::Box boxRelPosition_;
	bool dragBox_;
};


#endif

/* End SCROLBAR.HPP *************************************************/
