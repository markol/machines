/*
 * S T A R T C T X . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/startctx.hpp"

MachGuiStartupScreenContext::MachGuiStartupScreenContext( MachGuiStartupScreens* pStartupScreens ) 
:	pStartupScreens_( pStartupScreens )
{

    TEST_INVARIANT;
}

MachGuiStartupScreenContext::~MachGuiStartupScreenContext()
{
    TEST_INVARIANT;

}

//virtual
void MachGuiStartupScreenContext::update()
{
	// Intentionally empty
}

// virtual
bool MachGuiStartupScreenContext::okayToSwitchContext()
{
	return true;
}

//virtual 
void MachGuiStartupScreenContext::buttonEvent( MachGuiStartupScreens::ButtonEvent )
{
	// Intentionally empty
}

//virtual 
bool MachGuiStartupScreenContext::doHandleKeyEvent( const GuiKeyEvent& )
{
	// Intentionally empty
	return false;
}
	
void MachGuiStartupScreenContext::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiStartupScreenContext& t )
{

    o << "MachGuiStartupScreenContext " << (void*)&t << " start" << std::endl;
    o << "MachGuiStartupScreenContext " << (void*)&t << " end" << std::endl;

    return o;
}

/* End STARTCTX.CPP *************************************************/
