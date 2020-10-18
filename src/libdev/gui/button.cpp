
#include "gui/button.hpp"
#include "gui/event.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"

/* //////////////////////////////////////////////////////////////// */

GuiButton::GuiButton( GuiDisplayable *pParent, const Gui::Box& rel )
: GuiDisplayable( pParent, rel ),
  isDepressed_( false ),
  isDisabled_( false ),
  isPopupButton_( true )
{
	// Intentionally Empty
}

GuiButton::~GuiButton()
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

void GuiButton::popupButton( bool isPopupButton )
{
	isPopupButton_ = isPopupButton;
}

void GuiButton::setDepressed( bool depressed )
{
	if ( isDepressed_ != depressed )
	{
		isDepressed_ = depressed;
		changed();

		if ( isDepressed_ )
		{
			doBeDepressed( GuiMouseEvent( Gui::Coord( 0, 0 ), Gui::NO_CHANGE, Gui::NO_CHANGE, 0 ) );
		}
		else
		{
			doBeReleased( GuiMouseEvent( Gui::Coord( 0, 0 ), Gui::NO_CHANGE, Gui::NO_CHANGE, 0 ) );
		}
	}
}

bool GuiButton::isDisabled() const { return isDisabled_; }
bool GuiButton::isDepressed() const { return isDepressed_; }

//////////////////////////////////////////////////////////////////////

void GuiButton::doHandleMouseClickEvent( const GuiMouseEvent& e )
{
	if( isVisible() )
	{
		if( e.leftButton() == Gui::RELEASED )
		{
			// Only interested in release event if we are a popupButton,
			// stay-pressed buttons are sent release event when they are pressed
			// a second time.
			if ( isPopupButton_ and isDepressed_ )
			{
				isDepressed_ = false;

				if( not isDisabled() )
				{
					changed();
					doBeReleased( e );
				}
			}
		}
		else if( e.leftButton() == Gui::PRESSED )
		{
			if ( isPopupButton_ )
				isDepressed_ = true;
			else
				isDepressed_ = not isDepressed_;

			if( not isDisabled() )
			{
				changed();

				isDepressed_ ? doBeDepressed( e ) : doBeReleased( e );
			}
		}
	}
}

void GuiButton::doHandleMouseExitEvent( const GuiMouseEvent& )
{
	if( isVisible() )
	{
		if( isDepressed_ and isPopupButton_ )
		{
			isDepressed_ = false;
			changed();
		}
	}
}

void GuiButton::doDisplay()
{
	if( isDepressed() )
		isDisabled() ? doDisplayDepressedEnabled() : doDisplayDepressedDisabled();
	else
		isDisabled() ? doDisplayReleasedEnabled() : doDisplayReleasedDisabled();
}

//////////////////////////////////////////////////////////////////////

void GuiButton::doDisplayDepressedDisabled()
{
	doDisplayDepressedEnabled();
}

void GuiButton::doDisplayReleasedDisabled()
{
	doDisplayReleasedEnabled();
}

//////////////////////////////////////////////////////////////////////

void GuiButton::enable()
{
	PRE( isDisabled() );

	isDisabled_ = false;
	changed();

	PRE( not isDisabled() );
}

void GuiButton::disable()
{
	PRE( not isDisabled() );

	isDisabled_ = true;
	changed();

	POST( isDisabled() );
}

//////////////////////////////////////////////////////////////////////

GuiButtonWithBorder::GuiButtonWithBorder( GuiDisplayable *pParent,
											const Gui::Box& rel,
											const GuiBorderMetrics& m,
											const Gui::Coord& depressedOffset )
: GuiButton( pParent, rel ),
  borderMetrics_( m ),
  borderDimensions_( GuiButtonWithBorder::interiorDimensions( rel, m ) ),
  depressedInteriorOffset_( depressedOffset )
{
	POST( borderDimensions().exteriorWidth() == Gui::width( rel ) );
	POST( borderDimensions().exteriorHeight() == Gui::height( rel ) );
}

GuiButtonWithBorder::~GuiButtonWithBorder()
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////

const GuiBorderMetrics&
GuiButtonWithBorder::borderMetrics() const
{
	return borderMetrics_;
}

const GuiBorderDimensions&
GuiButtonWithBorder::borderDimensions() const
{
	return borderDimensions_;
}

const Gui::Coord&
GuiButtonWithBorder::depressedInteriorOffset() const
{
	return depressedInteriorOffset_;
}

//////////////////////////////////////////////////////////////////////

void GuiButtonWithBorder::doDisplayBorderDepressedDisabled()
{
	doDisplayBorderDepressedEnabled();
}

void GuiButtonWithBorder::doDisplayBorderReleasedDisabled()
{
	doDisplayBorderReleasedEnabled();
}

void GuiButtonWithBorder::doDisplayDepressedEnabled()
{
	doDisplayBorderDepressedEnabled();

	const GuiBorderMetrics& m = borderMetrics();
	unsigned offsetX = m.frameThickness() + m.highlightThickness()
							+ depressedInteriorOffset_.x();
	unsigned offsetY = m.frameThickness() + m.highlightThickness()
							+ depressedInteriorOffset_.y();

	doDisplayInteriorEnabled( Gui::Coord( absoluteCoord().x() + offsetX,
											absoluteCoord().y() + offsetY ) );
}

void GuiButtonWithBorder::doDisplayReleasedEnabled()
{
	doDisplayBorderReleasedEnabled();

	const GuiBorderMetrics& m = borderMetrics();
	unsigned offset = m.frameThickness() + m.highlightThickness();
	doDisplayInteriorEnabled( Gui::Coord( absoluteCoord().x() + offset,
									absoluteCoord().y() + offset ) );
}

///////////////////////////////////

//static
GuiBorderDimensions
GuiButtonWithBorder::interiorDimensions( const Gui::Box& rel, const GuiBorderMetrics& m )
{
	return GuiBorderDimensions::interiorBorder( Gui::WidthAndHeight( Gui::width( rel ), Gui::height( rel ) ), m );
}

//////////////////////////////////////////////////////////////////////

GuiButtonWithFilledBorder::GuiButtonWithFilledBorder( GuiDisplayable *pParent,
														const Gui::Box& rel,
														const GuiBorderMetrics& m,
														const GuiFilledBorderColours& colours,
														const Gui::Coord& depressedOffset )
: GuiButtonWithBorder( pParent, rel, m, depressedOffset ),
  border_( colours )
{
	// Intentionally Empty
}

GuiButtonWithFilledBorder::~GuiButtonWithFilledBorder()
{
	// Intentionally Empty
}

void GuiButtonWithFilledBorder::doDisplayBorderDepressedEnabled()
{
	border_.isDepressed( true );
	const Gui::Coord& deprCoord = depressedInteriorOffset();
	const GuiBorderMetrics& m = borderMetrics();
	GuiBorderMetrics depressedMetrics( m.frameThickness(),
										m.highlightThickness() + std::max( deprCoord.x(), deprCoord.y() ),
										m.shadowThickness() );

	border_.display( absoluteCoord(), borderDimensions(), depressedMetrics );
}

void GuiButtonWithFilledBorder::doDisplayBorderReleasedEnabled()
{
	border_.isDepressed( false );
	border_.display( absoluteCoord(), borderDimensions(), borderMetrics() );
}

GuiFilledBorder& GuiButtonWithFilledBorder::border() { return border_; }
const GuiFilledBorder& GuiButtonWithFilledBorder::border() const { return border_; }

//////////////////////////////////////////////////////////////////////

GuiBitmapButtonWithFilledBorder::GuiBitmapButtonWithFilledBorder(
	GuiDisplayable *pParent,
	const Gui::Coord& rel,
	const GuiBorderMetrics& m,
	const GuiFilledBorderColours& colours,
	const GuiBitmap& bitmap,
	const Gui::Coord& depressedOffset )
: GuiButtonWithFilledBorder(
	pParent,
	GuiBitmapButtonWithFilledBorder::exteriorRelativeBoundary( bitmap, m, rel ),
	m, colours, depressedOffset ),
  bitmap_( bitmap )
{
	// Intentionally Empty
}

GuiBitmapButtonWithFilledBorder::~GuiBitmapButtonWithFilledBorder()
{
	// Intentionally Empty
}

void GuiBitmapButtonWithFilledBorder::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	GuiPainter::instance().blit( getBitmap(), abs );
}

//////////////////////////////////////////////////////////////////////

// static
Gui::Box
GuiBitmapButtonWithFilledBorder::exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel )
{
	return Gui::Box( rel, m.totalHorizontalThickness() + bitmap.width(),
						m.totalVerticalThickness() + bitmap.height() );
}

//virtual
const GuiBitmap& GuiBitmapButtonWithFilledBorder::getBitmap() const
{
	return bitmap_;
}

void GuiBitmapButtonWithFilledBorder::bitmap( const GuiBitmap& newBitmap )
{
	bitmap_ = newBitmap;
}

//////////////////////////////////////////////////////////////////////

/* End BUTTON.CPP ***************************************************/
