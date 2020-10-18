/*
 * L I S T O B V R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiListObserver

	The base class for such things as ScrollBars, ScrollButtons etc. They will automatically
	get informed of changes to the list box ( e.g. children added, scroll events etc. ).
*/

#ifndef _GUI_LISTOBVR_HPP
#define _GUI_LISTOBVR_HPP

#include "base/base.hpp"

class GuiSimpleScrollableList;

class GuiListObserver
// Canonical form
{
public:
    GuiListObserver( GuiSimpleScrollableList* pList );
	// PRE( pList );

    virtual ~GuiListObserver( void );

	virtual void listUpdated() = 0;
	virtual void listDeleted();

protected:
	bool hasList() const;

	GuiSimpleScrollableList& list();
	// PRE( hasList() );

private:
    friend ostream& operator <<( ostream& o, const GuiListObserver& t );

    void CLASS_INVARIANT;

    GuiListObserver( const GuiListObserver& );
    GuiListObserver& operator =( const GuiListObserver& );

	GuiSimpleScrollableList* pList_;
};							   

#endif

/* End LISTOBVR.HPP *************************************************/
