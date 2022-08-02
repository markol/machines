/*
 * P A I N T E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_PAINTER_HPP
#define _GUI_PAINTER_HPP

#include "stdlib/string.hpp"

#include "gui/gui.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiBorderMetrics;
class GuiBorderDimensions;
class GuiFilledBorderColours;

class GuiPainter
// Singleton
{
public:

	static GuiPainter& instance();
	
	///////////////////////////////
	
	void fill( 	const Gui::Box& destArea, const GuiColour& );

	// Blit bitmap to backbuffer
	void blit( 	const GuiBitmap& source, 
			   	const Gui::Box& sourceArea,
			   	const Gui::Coord& dest );

	// Same as above only sourceArea is entire area of the source GuiBitmap
	void blit( 	const GuiBitmap& source, 
			   	const Gui::Coord& dest );

	// Same as above only dest is defaulted to (0,0)
	void blit( 	const GuiBitmap& source );

	void tile( 	const GuiBitmap& source, 
				const Gui::Box& sourceArea, 
				const Gui::Box& destArea );

	void stretch( 	const GuiBitmap& source, 
					const Gui::Box& sourceArea, 
					const Gui::Box& destArea );

	void stretch( const GuiBitmap& source, 
				  const Gui::Box& destArea );

	///////////////////////////////

	void filledRectangle( const Gui::Box&, const Gui::Colour& );
	void hollowRectangle( const Gui::Box&, const Gui::Colour&, unsigned thickness );

	void bevel( const Gui::Box& b,
				unsigned thickness, 
				const Gui::Colour& hiCol, 
				const Gui::Colour& loCol );
	
	///////////////////////////////

	void line( const Gui::Coord& c1, 
				const Gui::Coord& c2, 
				const Gui::Colour&, 
				unsigned thickness );

	void horizontalLine( const Gui::Coord& c1,
							unsigned length,
							const Gui::Colour&, 
							unsigned thickness );

	void verticalLine( const Gui::Coord& c1,
							unsigned height,
							const Gui::Colour&, 
							unsigned thickness );

	void polyline( const GuiCoords& coords,
					const Gui::Colour&, 
					unsigned thickness );
	
	///////////////////////////////

	void text( const Gui::Coord& c, 
				const string& text, 
				const Gui::Colour& );

	void rightAlignText(const Gui::Coord& c, 
						const string& theText, 
						const Gui::Colour& col );
	
	///////////////////////////////

	void filledBorder( const Gui::Coord& absCoord, 
						const GuiBorderDimensions&, 
						const GuiFilledBorderColours&, 
						const GuiBorderMetrics& );

private:
	GuiPainter( const GuiPainter& );
	GuiPainter& operator =( const GuiPainter& );
	bool operator ==( const GuiPainter& ) const;

	GuiPainter();
};

/* //////////////////////////////////////////////////////////////// */

#endif	//	#ifndef _GUI_PAINTER_HPP

