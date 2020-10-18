/*
 * A U T O D E L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/autodel.hpp"
#include "machgui/startup.hpp"

MachGuiAutoDeleteDisplayable::MachGuiAutoDeleteDisplayable( MachGuiStartupScreens* pStartupScreens )
:	pStartupScreens_( pStartupScreens )
{
	PRE( pStartupScreens );

	pStartupScreens_->registerAutoDeleteGuiElement( this );

    TEST_INVARIANT;
}

// virtual
MachGuiAutoDeleteDisplayable::~MachGuiAutoDeleteDisplayable()
{
    TEST_INVARIANT;

	pStartupScreens_->unregisterAutoDeleteGuiElement();
	// Make sure the screen redraws it's self. We don't know what this control may
	// have wiped over so redraw whole screen just to be sure. This isn't ideal
	// by "gui" lib isn't that sophisticated.
	pStartupScreens_->changed();
}

MachGuiStartupScreens& MachGuiAutoDeleteDisplayable::startupScreens()
{
	PRE( pStartupScreens_ );

	return *pStartupScreens_;
}

//virtual
void MachGuiAutoDeleteDisplayable::update()
{}

void MachGuiAutoDeleteDisplayable::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiAutoDeleteDisplayable& t )
{

    o << "MachGuiAutoDeleteDisplayable " << (void*)&t << " start" << std::endl;
    o << "MachGuiAutoDeleteDisplayable " << (void*)&t << " end" << std::endl;

    return o;
}

/* End AUTODEL.CPP **************************************************/
