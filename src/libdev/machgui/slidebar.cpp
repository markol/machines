/*
 * S L I D E B A R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/slidebar.hpp"
#include "machgui/gui.hpp"
#include "gui/painter.hpp"
#include "gui/event.hpp"

MachGuiSlideBar::MachGuiSlideBar( 	MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width, 
   				 					float minVal, float maxVal )
:	GuiDisplayable( pParent, Gui::Box( topLeft, width, reqHeight() ) ),
	MachGuiFocusCapableControl( pStartupScreens ),
	minValue_(minVal),
	maxValue_(maxVal),
	highlighted_( false )
{
	barPos_ = 1.0;
	
	POST_INFO( value() );
	POST( value() >= minValue_ );
	POST( value() <= maxValue_ );

    TEST_INVARIANT;
}


MachGuiSlideBar::MachGuiSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width )
:	GuiDisplayable( pParent, Gui::Box( topLeft, width, reqHeight() ) ),
	MachGuiFocusCapableControl( pStartupScreens ),
	highlighted_( false )
{
	minValue_ = 0.0;
	maxValue_ = 1.0;
	barPos_ = 1.0;
	
	POST_INFO( value() );
	POST( minValue_ == 0.0 );
	POST( maxValue_ == 1.0 );
	POST( value() == minValue_ );

    TEST_INVARIANT;
}

MachGuiSlideBar::~MachGuiSlideBar()
{
    TEST_INVARIANT;

}

void MachGuiSlideBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSlideBar& t )
{

    o << "MachGuiSlideBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiSlideBar " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiSlideBar::minMax( float minV, float maxV )
{
	PRE( minV < maxV );

	minValue_ = minV;
	maxValue_ = maxV;

	RICHARD_STREAM("Minmax value " << value() << std::endl);

	valueChanged( value() );
}

float MachGuiSlideBar::value() const
{
	float usableWidth = width();
	usableWidth -= 2; // sandy coloured border
	usableWidth -= MachGui::slideBtnBmp().width();

	float barPos = barPos_ - 1.0;
	RICHARD_STREAM("barPos_" << barPos_ << std::endl);
	RICHARD_STREAM("usableWidth" << usableWidth << std::endl);

	float minMaxDiff = maxValue_ - minValue_;
	 
	float barPosPercent = barPos / usableWidth;

	float value = minValue_ + ( barPosPercent * minMaxDiff );

	RICHARD_STREAM("maxValue_" << maxValue_ << std::endl);
	RICHARD_STREAM("minValue_ " << minValue_ << std::endl);
	RICHARD_STREAM("Value set to " << value << std::endl);

	return value;
}

void MachGuiSlideBar::value( float newVal )
{
	PRE( newVal >= minValue_ and newVal <= maxValue_ );

	RICHARD_STREAM("Setting value to " << newVal << std::endl);

	float usableWidth = width();
	usableWidth -= 2; // sandy coloured border
	usableWidth -= MachGui::slideBtnBmp().width();
	RICHARD_STREAM("usableWidth(b) " << usableWidth  << std::endl);

	float minMaxDiff = maxValue_ - minValue_;
	RICHARD_STREAM("value maxValue_" << maxValue_ << std::endl);
	RICHARD_STREAM("value minValue_ " << minValue_ << std::endl);

	float barPosPercent = ( newVal - minValue_ ) / minMaxDiff;

	float barPos = barPosPercent * usableWidth;

	barPos_ = barPos + 1.0;
	RICHARD_STREAM("barPos_(b) " << barPos_ << std::endl);

	changed();

	RICHARD_STREAM("New val " << newVal << std::endl);

	valueChanged( newVal );
}

//virtual 
void MachGuiSlideBar::valueChanged( float /*value*/ )
{
	// Intentionally empty
}

//static 
size_t MachGuiSlideBar::reqHeight()
{
	return MachGui::slideBtnBmp().height() + 4;
}

//virtual 
void MachGuiSlideBar::doDisplay()
{
	if ( isFocusControl() )
	{
		GuiPainter::instance().blit( MachGui::longYellowGlowBmp(), Gui::Box(0,0,width(),height()), absoluteBoundary().minCorner() );
	}
	else if ( highlighted_ )
	{
		GuiPainter::instance().blit( MachGui::longGlowBmp(), Gui::Box(0,0,width(),height()), absoluteBoundary().minCorner() );
	}
	else
	{
		GuiPainter::instance().filledRectangle( absoluteBoundary(), MachGui::MENUDARKGREEN() );
	}

	GuiPainter::instance().hollowRectangle( absoluteBoundary(), MachGui::DARKSANDY(), 1 );
	GuiPainter::instance().blit( MachGui::slideBtnBmp(), 
								 Gui::Coord( absoluteBoundary().minCorner().x() + (int)barPos_, absoluteBoundary().minCorner().y() + 2 ) );
}

//virtual 
void MachGuiSlideBar::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	if ( rel.leftButton() == Gui::PRESSED )
	{
		barMoved( rel.coord().x() );
	}
}

//virtual 
void MachGuiSlideBar::doHandleMouseExitEvent( const GuiMouseEvent& )
{
	highlighted_ = false;

	changed();
}

//virtual 
void MachGuiSlideBar::doHandleMouseEnterEvent( const GuiMouseEvent& )
{
	highlighted_ = true;

	changed();
}

//virtual 
void MachGuiSlideBar::doHandleContainsMouseEvent( const GuiMouseEvent& rel )
{
	if ( rel.leftButton() == Gui::PRESSED )
	{		
		barMoved( rel.coord().x() );
	}
}

void MachGuiSlideBar::barMoved( Gui::XCoord newBarPos )
{
	float wastedWidth = 2 /*sandy coloured border*/ + MachGui::slideBtnBmp().width();
	float usableWidth = width() - wastedWidth;

	float validStartPos = wastedWidth / 2.0;
	float validEndPos = width() - ( wastedWidth / 2.0 );

	if ( newBarPos < validStartPos )
		newBarPos = validStartPos;
	else if ( newBarPos > validEndPos )
		newBarPos = validEndPos;

	barPos_ = newBarPos - ( wastedWidth / 2.0 ) + 1.0;

	changed();
	valueChanged( value() );
}

//virtual
bool MachGuiSlideBar::doHandleNavigationKey( NavKey navKey, MachGuiFocusCapableControl** )
{
	bool retValue = false;

	if ( navKey == MachGuiFocusCapableControl::RIGHT_ARROW )
	{
		double incAmount = ( maxValue_ - minValue_ ) / 100.0;
		double newValue = value() + incAmount;
		
		if ( newValue > maxValue_ )
		{
			newValue = maxValue_;
		}

		value( newValue );

		retValue = true;
	}
	else if ( navKey == MachGuiFocusCapableControl::LEFT_ARROW )
	{
		double decAmount = ( maxValue_ - minValue_ ) / 100.0;
		double newValue = value() - decAmount;

		if ( newValue < minValue_ )
		{
			newValue = minValue_;
		}

		value( newValue );

		retValue = true;
	}

	return retValue;
}

//virtual 
void MachGuiSlideBar::hasFocus( bool newValue )
{
	MachGuiFocusCapableControl::hasFocus( newValue );

	changed();
}

/* End SLIDEBAR.CPP *************************************************/
