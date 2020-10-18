/*
 * B U I L D B A R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/buildbar.hpp"
#include "machgui/gui.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"

MachGuiBuildProgressBar::MachGuiBuildProgressBar( 	GuiDisplayable * pParent,
				    								const Gui::Coord& rel,
    												size_t width )
: GuiDisplayable( pParent, Gui::Box( rel, width, SHADOW_THICKNESS + BAR_THICKNESS + ( 2 * BORDER_THICKNESS ) ) ),
  percentageComplete_(0.0)
{

    TEST_INVARIANT;
}

MachGuiBuildProgressBar::~MachGuiBuildProgressBar()
{
    TEST_INVARIANT;

}

void MachGuiBuildProgressBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachGuiBuildProgressBar::depress( bool doDepress )
{
	if ( doDepress )
	{
		relativeCoord( Gui::Coord( 3, 3 ) );
	}
	else
	{
		relativeCoord( Gui::Coord( 2, 2 ) );
	}
}

Gui::Colour	MachGuiBuildProgressBar::barColour() const
// width in pixels for a given ratio
{
	unsigned currentValue = 1;
	unsigned maxValue = 1;

	PRE( currentValue <= maxValue );
	const float LOW_THRESHOLD = 1.0 / 3.0;
	const float MID_THRESHOLD = 1.7 / 3.0;

	Gui::Colour result = MachGui::PROGRESSGREEN();
	float ratio = currentValue  / (float)maxValue;
	if( ratio <= LOW_THRESHOLD )
		result = Gui::RED();
	else if( ratio <= MID_THRESHOLD )
		result = Gui::YELLOW();

	return result;
}

void MachGuiBuildProgressBar::doDisplay()
{
	GuiPainter::instance().hollowRectangle( absoluteBoundary(), Gui::WHITE(), BORDER_THICKNESS );

	size_t interiorWidth = width() - ( 2 * BORDER_THICKNESS ) - SHADOW_THICKNESS;
	size_t barWidth = ( percentageComplete_ / 100.0 ) * ((double)interiorWidth - 1.0);

 	Gui::Coord topLeft( absoluteCoord().x() + BORDER_THICKNESS, absoluteCoord().y() + BORDER_THICKNESS );
	Gui::Coord sc( absoluteCoord().x() + BORDER_THICKNESS + SHADOW_THICKNESS, absoluteCoord().y() + BORDER_THICKNESS  );
	Gui::Coord bc( absoluteCoord().x() + BORDER_THICKNESS + SHADOW_THICKNESS, absoluteCoord().y() + 1 + BORDER_THICKNESS + SHADOW_THICKNESS);

 	GuiPainter::instance().horizontalLine( sc, interiorWidth, Gui::BLACK(), SHADOW_THICKNESS );
	GuiPainter::instance().horizontalLine( bc, interiorWidth - 1, MachGui::PURPLE(), BAR_THICKNESS );
	if ( barWidth ) // Only draw if the barWidth is not zero
		GuiPainter::instance().horizontalLine( bc, barWidth, barColour(), BAR_THICKNESS );
	GuiPainter::instance().verticalLine( topLeft, SHADOW_THICKNESS + BAR_THICKNESS, Gui::BLACK(), SHADOW_THICKNESS );
}

//static
size_t MachGuiBuildProgressBar::height()
{
	return ( BORDER_THICKNESS + SHADOW_THICKNESS + BAR_THICKNESS + BORDER_THICKNESS );
}

void MachGuiBuildProgressBar::percentageComplete( float complete )
{
	percentageComplete_ = complete;
	changed();
}


ostream& operator <<( ostream& o, const MachGuiBuildProgressBar& t )
{

    o << "MachGuiBuildProgressBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiBuildProgressBar " << (void*)&t << " end" << std::endl;

    return o;
}

float MachGuiBuildProgressBar::percentageComplete() const
{
	return percentageComplete_;
}

/* End BUILDBAR.CPP *************************************************/
