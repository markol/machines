/*
 * P A I N T E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/painter.hpp"

#include "gui/border.hpp"

#include "ctl/vector.hpp"
#include "render/surface.hpp"

/* //////////////////////////////////////////////////////////////// */

GuiPainter&
GuiPainter::instance()
{
	static GuiPainter instance_;
	return instance_;
}

/* //////////////////////////////////////////////////////////////// */

GuiPainter::GuiPainter()
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////

RenSurface::Rect
map_GuiBox_to_RenSurfaceRect( const Gui::Box& b )
{
	return RenSurface::Rect( b.minCorner().x(),
								b.minCorner().y(),
								Gui::width( b ),
								Gui::height( b ) );
}

void GuiPainter::filledRectangle( const Gui::Box& b, const Gui::Colour& col )
{
	Gui::backBuffer().filledRectangle( map_GuiBox_to_RenSurfaceRect( b ), col );
}

//////////////////////////////////////////////////////////////////////

void GuiPainter::hollowRectangle( const Gui::Box& b,
									const Gui::Colour& col,
									unsigned thickness )
{
	Ren::Rect area(b.minCorner().x(), b.minCorner().y(), Gui::width( b ) - 1, Gui::height( b ) - 1 );

	Gui::backBuffer().hollowRectangle(area, col, thickness);
}

/* //////////////////////////////////////////////////////////////// */

void GuiPainter::line( const Gui::Coord& c1,
						const Gui::Coord& c2,
						const Gui::Colour& col,
						unsigned thickness )
{
	RenSurface::Points p;
    p.reserve( 2 );
	p.push_back( c1 );
	p.push_back( c2 );
	Gui::backBuffer().polyLine( p, col, thickness );
}

void GuiPainter::horizontalLine( const Gui::Coord& c1,
							unsigned length,
							const Gui::Colour& col,
							unsigned thickness )
{
	if(thickness > 1)
	{
        Gui::Coord c(c1);
        c.y(c.y() - thickness/2.0);
        line( c, Gui::Coord( c.x() + length, c.y() ), col, thickness );
    }
    else
        line( c1, Gui::Coord( c1.x() + length, c1.y() ), col, thickness );

}

void GuiPainter::verticalLine( const Gui::Coord& c1,
							unsigned height,
							const Gui::Colour& col,
							unsigned thickness )
{
	line( c1, Gui::Coord( c1.x(), c1.y() + height ), col, thickness );
}

/* //////////////////////////////////////////////////////////////// */

void GuiPainter::bevel( const Gui::Box& b,
							unsigned thickness,
							const Gui::Colour& hiCol,
							const Gui::Colour& loCol )
{
	const Gui::Coord& c = b.minCorner();
	Gui::XCoord x1 = c.x();
	Gui::YCoord y1 = c.y();
	Gui::XCoord x2 = x1 + Gui::width( b );
	Gui::YCoord y2 = y1 + Gui::height( b );

	for( unsigned i=0; i<thickness; ++i, ++x1, ++y1, --x2, --y2 )
	{
		hollowRectangle( Gui::Box( x1, y1, x2, y1 ), hiCol, 1 );
		hollowRectangle( Gui::Box( x1, y1, x1, y2 ), hiCol, 1 );
		hollowRectangle( Gui::Box( x2, y1+1, x2, y2 ), loCol, 1 );
		hollowRectangle( Gui::Box( x1+1,y2, x2, y2 ), loCol, 1 );
	}
}

//////////////////////////////////////////////////////////////////////

void GuiPainter::text( const Gui::Coord& c, const string& theText, const Gui::Colour& col )
{
    RenSurface backBuffer = Gui::backBuffer();
    backBuffer.useFontHeight(12);
    backBuffer.drawText( c.x(), c.y(), theText, col );
}

void GuiPainter::rightAlignText( const Gui::Coord& c, const string& theText, const Gui::Colour& col )
{
    Gui::Coord cCopy( c );

    RenSurface backBuffer = Gui::backBuffer();
    backBuffer.useFontHeight(12);

	Ren::Rect textDimensions;
	backBuffer.textDimensions( theText, &textDimensions );
	cCopy.x( cCopy.x() - textDimensions.width );

    backBuffer.drawText( cCopy.x(), cCopy.y(), theText, col );
}

//////////////////////////////////////////////////////////////////////

void GuiPainter::blit( 	const GuiBitmap& source,
						const Gui::Box& sourceArea,
						const Gui::Coord& dest )
{
	Gui::backBuffer().simpleBlit( 	source,
									map_GuiBox_to_RenSurfaceRect( sourceArea ),
									dest.x(), dest.y() );
}

void GuiPainter::blit( 	const GuiBitmap& source,
						const Gui::Coord& dest )
{
	Gui::backBuffer().simpleBlit( 	source,
									dest.x(), dest.y() );
}

void GuiPainter::blit( 	const GuiBitmap& source )
{
	Gui::backBuffer().simpleBlit( source );
}

void GuiPainter::tile( 	const GuiBitmap& source,
						const Gui::Box& sourceArea,
						const Gui::Box& destArea )
{
	Gui::backBuffer().tileBlit( source,
								map_GuiBox_to_RenSurfaceRect( sourceArea ),
								map_GuiBox_to_RenSurfaceRect( destArea ) );
}

void GuiPainter::stretch( 	const GuiBitmap&/*source*/,
							const Gui::Box& /*sourceArea*/,
							const Gui::Box& /*destArea*/ )
{}

void GuiPainter::stretch( 	const GuiBitmap& /*source*/,
							const Gui::Box& /*destArea*/ )
{}

//////////////////////////////////////////////////////////////////////

void GuiPainter::filledBorder( const Gui::Coord& absCoord,
								const GuiBorderDimensions& dim,
								const GuiFilledBorderColours& colours,
								const GuiBorderMetrics& m )
{
	unsigned exteriorHeight = dim.exteriorHeight();
	unsigned exteriorWidth = dim.exteriorWidth();
	unsigned interiorHeight = dim.interiorHeight();
	unsigned interiorWidth = dim.interiorWidth();

	// draw frame
	Gui::Box boundary( absCoord, exteriorWidth, exteriorHeight );
	hollowRectangle( boundary, colours.frameColour(), m.frameThickness() );

	unsigned borderWidth = exteriorWidth - ( 2 * m.frameThickness() );
	unsigned borderHeight = exteriorHeight - ( 2 * m.frameThickness() );

	///////////////////////////////

	unsigned halfShadowThickness = ( m.shadowThickness() % 2 == 0 )
										? ( m.shadowThickness() - 1 ) / 2
										: ( m.shadowThickness() ) / 2;

	enum { TBD_REMOVE_HACK_OFFSET = 1 };

	Gui::Coord borderBottomLeft( absCoord.x() + m.frameThickness(),
									absCoord.y() + exteriorHeight
									- m.frameThickness()
									- halfShadowThickness
									- TBD_REMOVE_HACK_OFFSET );

	// bottom border
	horizontalLine( borderBottomLeft,
						exteriorWidth - ( 2 * m.frameThickness() ), // + 1,
						colours.shadowColour(),
						m.shadowThickness() );

	///////////////////////////////

	// right border
	Gui::Coord borderTopRight( absCoord.x()
								+ exteriorWidth
								- m.frameThickness()
								- halfShadowThickness
								- TBD_REMOVE_HACK_OFFSET,
								absCoord.y() + m.frameThickness() );

	verticalLine( borderTopRight,
					exteriorHeight - ( 2 * m.frameThickness() ) - m.shadowThickness() + 1,
					colours.shadowColour(),
					m.shadowThickness() );

	///////////////////////////////

	// top and left borders

	Gui::Coord borderTopLeft( absCoord.x() + m.frameThickness(),
								absCoord.y() + m.frameThickness() );

	for( unsigned i=0; i<m.highlightThickness(); ++i )
	{
		Gui::Coord hBorderCoord( borderTopLeft.x() + i, borderTopLeft.y() + i );

		horizontalLine( hBorderCoord,
							borderWidth - i,
							colours.highlightColour(), 1 );

		Gui::Coord vBorderCoord( borderTopLeft.x() + i, borderTopLeft.y() + i );

		verticalLine( vBorderCoord,
							borderHeight - i,
							colours.highlightColour(), 1 );
	}
}

/*********************************************************************

void GuiPainter::filledBorder( const Gui::Coord& absCoord,
								const GuiBorderDimensions& dim,
								const GuiFilledBorderColours& colours )
{

	unsigned w = dim.exteriorWidth() - 1;
	unsigned h = dim.exteriorHeight() - 1;

	const GuiBorderMetrics& metrics = dim.metrics();

	horizontalLineGW( colours.outlineColour(), absCoord.x() + 1, absCoord.y(), w );
	horizontalLineGW( colours.outlineColour(), absCoord.x() + 1, absCoord.y() + h + 1, w );

	verticalLineGW( colours.outlineColour(), absCoord().x(), absCoord().y() + 1, h );
	verticalLineGW( colours.outlineColour(), absCoord().x() + w + 1, absCoord().y() + 1, h );

	bevelGW( colours.highlightColour(),
				colours.shadowColour(),
				absCoord().x() + 2,
				absCoord().y() + 1, 1,
				width() - 2, height() - 2 );
}

*********************************************************************/

//////////////////////////////////////////////////////////////////////

/* End PAINTER.CPP **************************************************/

