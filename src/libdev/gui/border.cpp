/*
 * B O R D E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>

#include "gui/border.hpp"
#include "gui/painter.hpp"

//////////////////////////////////////////////////////////////////////

GuiBorderMetrics::GuiBorderMetrics(
	unsigned frameThickness,
	unsigned highlightThickness,
	unsigned shadowThickness
)
: frameThickness_( frameThickness ),
  highlightThickness_( highlightThickness ),
  shadowThickness_( shadowThickness )
{
	// Intentionally Empty
}

///////////////////////////////////

unsigned
GuiBorderMetrics::frameThickness() const { return frameThickness_; }

unsigned
GuiBorderMetrics::highlightThickness() const { return highlightThickness_; }

unsigned
GuiBorderMetrics::shadowThickness() const { return shadowThickness_;  }

//////////////////////////////////////////////////////////////////////

unsigned GuiBorderMetrics::totalVerticalThickness() const
{
	return highlightThickness_ + shadowThickness_ + ( 2 * frameThickness_ );
}

unsigned GuiBorderMetrics::totalHorizontalThickness() const
{
	return highlightThickness_ + shadowThickness_ + ( 2 * frameThickness_ );
}

//////////////////////////////////////////////////////////////////////

GuiBorderDimensions::GuiBorderDimensions( const Gui::WidthAndHeight& interior,
											const Gui::WidthAndHeight& exterior,
											const GuiBorderMetrics& m )
: interiorDimensions_( interior ),
  exteriorDimensions_( exterior )
{
	PRE( exterior.width() == interior.width() + m.totalHorizontalThickness() );
	PRE( exterior.height() == interior.height() + m.totalVerticalThickness() );
}

///////////////////////////////////

unsigned
GuiBorderDimensions::exteriorWidth() const
{
	return exteriorDimensions().width();
}

unsigned
GuiBorderDimensions::exteriorHeight() const
{
	return exteriorDimensions().height();
}

unsigned
GuiBorderDimensions::interiorWidth() const
{
	return interiorDimensions().width();
}

unsigned
GuiBorderDimensions::interiorHeight() const
{
	return interiorDimensions().height();
}

//////////////////////////////////////////////////////////////////////

Gui::WidthAndHeight
GuiBorderDimensions::exteriorDimensions() const { return exteriorDimensions_; }

Gui::WidthAndHeight
GuiBorderDimensions::interiorDimensions() const { return interiorDimensions_; }

//////////////////////////////////////////////////////////////////////

// static
GuiBorderDimensions
GuiBorderDimensions::exteriorBorder( const Gui::WidthAndHeight& interior, const GuiBorderMetrics& m )
{
	unsigned exteriorWidth = interior.width() + m.totalHorizontalThickness();
	unsigned exteriorHeight = interior.height() + m.totalVerticalThickness();
	return GuiBorderDimensions( interior, Gui::WidthAndHeight( exteriorWidth, exteriorHeight ), m );
}

// static
GuiBorderDimensions
GuiBorderDimensions::interiorBorder( const Gui::WidthAndHeight& exterior, const GuiBorderMetrics& m )
{
	PRE( exterior.width() >= m.totalHorizontalThickness() );
	PRE( exterior.height() >= m.totalVerticalThickness() );
	unsigned interiorWidth = exterior.width() - m.totalHorizontalThickness();
	unsigned interiorHeight = exterior.height() - m.totalVerticalThickness();
	return GuiBorderDimensions( Gui::WidthAndHeight( interiorWidth, interiorHeight ), exterior, m );
}

//////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const GuiBorderDimensions& d )
{
	o << "GuiBorderDimensions start" << std::endl;
	o << "\texteriorWidth " << d.exteriorDimensions().width() << std::endl;
	o << "\texteriorHeight " << d.exteriorDimensions().height() << std::endl;
	o << "\tinteriorHeight " << d.interiorDimensions().width() << std::endl;
	o << "\tinteriorHeight " << d.interiorDimensions().height() << std::endl;
	o << "GuiBorderDimensions end" << std::endl;
	return o;
}

//////////////////////////////////////////////////////////////////////

GuiFilledBorderColours::GuiFilledBorderColours( const Gui::Colour& frame,
												const Gui::Colour& highlight,
												const Gui::Colour& shadow,
												const Gui::Colour& selected )
: frameColour_( frame ),
  highlightColour_( highlight ),
  shadowColour_( shadow ),
  selectedColour_( selected ),
  depressedShadowColour_( shadow )
{
	// Intentionally Empty
}

GuiFilledBorderColours::GuiFilledBorderColours( const Gui::Colour& frame,
												const Gui::Colour& highlight,
												const Gui::Colour& shadow,
												const Gui::Colour& selected,
												const Gui::Colour& depressedShadow )
: frameColour_( frame ),
  highlightColour_( highlight ),
  shadowColour_( shadow ),
  selectedColour_( selected ),
  depressedShadowColour_( depressedShadow )
{
	// Intentionally Empty
}

///////////////////////////////////

void GuiFilledBorderColours::frameColour( const Gui::Colour& c )
{
	frameColour_ = c;
}

void GuiFilledBorderColours::highlightColour( const Gui::Colour& c )
{
	highlightColour_ = c;
}

void GuiFilledBorderColours::shadowColour( const Gui::Colour& c )
{
	shadowColour_ = c;
}

void GuiFilledBorderColours::selectedColour( const Gui::Colour& c )
{
	selectedColour_ = c;
}

void GuiFilledBorderColours::depressedShadowColour( const Gui::Colour& c )
{
	depressedShadowColour_ = c;
}

///////////////////////////////////

const Gui::Colour&
GuiFilledBorderColours::frameColour() const { return frameColour_; }

const Gui::Colour&
GuiFilledBorderColours::highlightColour() const { return highlightColour_; }

const Gui::Colour&
GuiFilledBorderColours::shadowColour() const { return shadowColour_; }

const Gui::Colour&
GuiFilledBorderColours::selectedColour() const { return selectedColour_; }

const Gui::Colour&
GuiFilledBorderColours::depressedShadowColour() const { return depressedShadowColour_; }

//////////////////////////////////////////////////////////////////////

void GuiFilledBorderColours::invert()
{
	std::swap( depressedShadowColour_, highlightColour_ );
}

void GuiFilledBorderColours::select()
{
	std::swap( frameColour_, selectedColour_ );
}

// static
GuiFilledBorderColours
GuiFilledBorderColours::invert( const GuiFilledBorderColours& c )
{
	return GuiFilledBorderColours( c.frameColour(),
									c.depressedShadowColour(),
									c.highlightColour(),
									c.selectedColour() );
}

// static
GuiFilledBorderColours
GuiFilledBorderColours::select( const GuiFilledBorderColours& c )
{
	return GuiFilledBorderColours( c.selectedColour(),
									c.highlightColour(),
									c.shadowColour(),
									c.frameColour() );
}

//////////////////////////////////////////////////////////////////////

GuiFilledBorder::GuiFilledBorder( const GuiFilledBorderColours& c )
: baseColours_( c ),
  isDepressed_( false ),
  isSelected_( false )
{
	POST( not isDepressed() );
	POST( not isSelected() );
}

//////////////////////////////////////////////////////////////////////

bool GuiFilledBorder::isDepressed() const { return isDepressed_; }
bool GuiFilledBorder::isSelected() const { return isSelected_; }

void GuiFilledBorder::isDepressed( bool depressed )
{
	isDepressed_ = depressed;
	POST( iff( depressed, isDepressed() ) );
}

void GuiFilledBorder::isSelected( bool selected )
{
	isSelected_ = selected;
	POST( iff( selected, isSelected() ) );
}

//////////////////////////////////////////////////////////////////////

void GuiFilledBorder::display( const Gui::Coord& absCoord,
								const GuiBorderDimensions& dim,
								const GuiBorderMetrics& m )
{
	GuiFilledBorderColours useColours = baseColours_;
	if( isSelected() )
		useColours.select();
	if( isDepressed() )
		useColours.invert();

	GuiPainter::instance().filledBorder( absCoord, dim, useColours, m );
}

//////////////////////////////////////////////////////////////////////

/* End BORDER.CPP ***************************************************/
