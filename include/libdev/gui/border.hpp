/*
 * B O R D E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_BORDER_HPP
#define _GUI_BORDER_HPP

#include "gui/gui.hpp"

//class ostream;

/* //////////////////////////////////////////////////////////////// */

class GuiBorderMetrics
// memberwise cannonical
{
public:

	GuiBorderMetrics( unsigned frameThickness,
						unsigned highlightThickness,
						unsigned shadowThickness );

	///////////////////////////////

	unsigned frameThickness() const;
	unsigned highlightThickness() const;
	unsigned shadowThickness() const;

	///////////////////////////////

	unsigned totalVerticalThickness() const;
	unsigned totalHorizontalThickness() const;

private:
	unsigned frameThickness_;
	unsigned highlightThickness_;
	unsigned shadowThickness_;
};

//////////////////////////////////////////////////////////////////////

class GuiBorderDimensions
// memberwise cannonical
{
public:

	static GuiBorderDimensions
	exteriorBorder( const Gui::WidthAndHeight& interior, const GuiBorderMetrics& );

	static GuiBorderDimensions
	interiorBorder( const Gui::WidthAndHeight& exterior, const GuiBorderMetrics& m );
	// PRE( exteriorWidth() >= m.totalHorizontalThickness() );
	// PRE( exteriorHeight() >= m.totalVerticalThickness() );

	unsigned exteriorWidth() const;
	unsigned exteriorHeight() const;
	unsigned interiorWidth() const;
	unsigned interiorHeight() const;

	Gui::WidthAndHeight exteriorDimensions() const;
	Gui::WidthAndHeight interiorDimensions() const;

private:

	GuiBorderDimensions( const Gui::WidthAndHeight& interior,
						const Gui::WidthAndHeight& exterior,
						const GuiBorderMetrics& m );
	// PRE( exterior.width() == interior.width() + totalHorizontalThickness( m ) );
	// PRE( exterior.height() == interior.height() + totalVerticalThickness( m ) );

	Gui::WidthAndHeight interiorDimensions_;
	Gui::WidthAndHeight exteriorDimensions_;

	friend ostream& operator <<( ostream& o, const GuiBorderDimensions& );
};

//////////////////////////////////////////////////////////////////////

class GuiFilledBorderColours
// memberwise cannonical
{
public:

	GuiFilledBorderColours( const Gui::Colour& frame,
							const Gui::Colour& highlight,
							const Gui::Colour& shadow,
							const Gui::Colour& selected );

	GuiFilledBorderColours( const Gui::Colour& frame,
							const Gui::Colour& highlight,
							const Gui::Colour& shadow,
							const Gui::Colour& selected,
							const Gui::Colour& depressedShadow );

	void frameColour( const Gui::Colour& );
	void highlightColour( const Gui::Colour& );
	void shadowColour( const Gui::Colour& );
	void selectedColour( const Gui::Colour& );
	void depressedShadowColour( const Gui::Colour& );

	const Gui::Colour& frameColour() const;
	const Gui::Colour& highlightColour() const;
	const Gui::Colour& shadowColour() const;
	const Gui::Colour& selectedColour() const;
	const Gui::Colour& depressedShadowColour() const;

	void select();
	// POST( frameColour() == old( selectedColour() ) );
	// POST( selectedColour() == old( frameColour() ) );
	// POST( highlighColour() == old( highlightColour() ) );
	// POST( shadowColour() == old( shadowColour() ) );

	void invert();
	// POST( frameColour() == old( frameColour() ) );
	// POST( highlighColour() == old( shadowColour() ) );
	// POST( shadowColour() == old( highlightColour() ) );
	// POST( selectedColour() == old( selectedColour() ) );

   static GuiFilledBorderColours invert( const GuiFilledBorderColours& c );
	// POST( result == c.invert() );

   static GuiFilledBorderColours select( const GuiFilledBorderColours& c );
	// POST( result == c.select() );

private:

	Gui::Colour	frameColour_;
	Gui::Colour	highlightColour_;
	Gui::Colour	shadowColour_;
	Gui::Colour	selectedColour_;
	Gui::Colour depressedShadowColour_;
};

//////////////////////////////////////////////////////////////////////

class GuiFilledBorder
// memberwise cannonical
{
public:

	GuiFilledBorder( const GuiFilledBorderColours& );
	// POST( not isDepressed() );
	// POST( not isSelected() );

	bool isDepressed() const;
	void isDepressed( bool depressed );
	// POST( iff( depressed, isDepressed() ) );

	bool isSelected() const;
	void isSelected( bool selected );
	// POST( iff( selected, isSelected() ) );

	void display( const Gui::Coord& absCoord,
					const GuiBorderDimensions&,
					const GuiBorderMetrics& );

private:
	GuiFilledBorderColours	baseColours_;
	bool					isDepressed_;
	bool					isSelected_;
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _GUI_BORDER_HPP

/* End BORDER.HPP ***************************************************/
