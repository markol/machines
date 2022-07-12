/*
 * A U T O D E L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiAutoDeleteDisplayable

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_AUTODEL_HPP
#define _MACHGUI_AUTODEL_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachGuiStartupScreens;

class MachGuiAutoDeleteDisplayable
// Canonical form revoked
{
public:
    MachGuiAutoDeleteDisplayable( MachGuiStartupScreens* );
	// PRE( pStartupScreens );

    virtual ~MachGuiAutoDeleteDisplayable();

    void CLASS_INVARIANT;

	virtual bool containsMousePointer() = 0;

	virtual void update();

protected:
	MachGuiStartupScreens& startupScreens();
	// PRE( pStartupScreens_ );

private:
    friend ostream& operator <<( ostream& o, const MachGuiAutoDeleteDisplayable& t );

    MachGuiAutoDeleteDisplayable( const MachGuiAutoDeleteDisplayable& );
    MachGuiAutoDeleteDisplayable& operator =( const MachGuiAutoDeleteDisplayable& );

	// Data members...
	MachGuiStartupScreens* pStartupScreens_;
};


#endif

/* End AUTODEL.HPP **************************************************/
