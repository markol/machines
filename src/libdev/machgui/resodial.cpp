/*
 * R E S O D I A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/resodial.hpp"
#include "stdlib/string.hpp"
#include "gui/manager.hpp"

MachGuiResourceDial::MachGuiResourceDial
(
    const SysPathName& bitmapName, GuiDisplayable* pParent,
    const Gui::Coord& relativePosition, const Gui::Coord& textOffset
)
:   GuiImage( pParent, relativePosition, Gui::bitmap( bitmapName ) ),
    lastDisplayedValue_( 0 ),
    textOffset_( textOffset )
{

    TEST_INVARIANT;
}

MachGuiResourceDial::~MachGuiResourceDial()
{
    TEST_INVARIANT;

}

void MachGuiResourceDial::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiResourceDial& t )
{

    o << "MachGuiResourceDial " << (void*)&t << " start" << std::endl;
    o << "MachGuiResourceDial " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiResourceDial::updateValue( int newValue )
{
    //Check for change of value
    if( newValue != lastDisplayedValue_ )
    {
        lastDisplayedValue_ = newValue;
        changed();
    }
}

//virtual
void MachGuiResourceDial::doDisplay()
{
    //Do the background
    GuiImage::doDisplay();

    //Convert value to text string
    char buffer[64];
//    itoa( lastDisplayedValue_, buffer, 10 );
    sprintf(buffer, "%d", lastDisplayedValue_);

    //Draw the text
	text( textOffset_, string( buffer ), Gui::BLACK() );

}

/* End RESODIAL.CPP *************************************************/
