#include <iostream>
#include <stdio.h>
#include "stdlib/string.hpp"
#include "gui/gui.hpp"
#include "gui/displaya.hpp"
#include "system/pathname.hpp"

/* //////////////////////////////////////////////////////////////// */

//static
RenSurface&	Gui::staticBackBuffer()
{
	static RenSurface backBuffer_;
	return backBuffer_;
}

bool Gui::initialised_ = false;

/* //////////////////////////////////////////////////////////////// */

bool Gui::initialised()
{
	return initialised_;
}

RenSurface&
Gui::backBuffer()
{
	PRE( Gui::initialised() );
	return staticBackBuffer();
}

void Gui::backBuffer( const RenSurface& pNewBuffer )
{
	initialised_ = true;
	staticBackBuffer() = pNewBuffer;
}

/* //////////////////////////////////////////////////////////////// */

unsigned Gui::width( const Gui::Box& b )
{
	return b.maxCorner().x() - b.minCorner().x();
}

unsigned Gui::height( const Gui::Box& b )
{
	return b.maxCorner().y() - b.minCorner().y();
}

/* //////////////////////////////////////////////////////////////// */

bool operator ==( const GuiColour& a, const GuiColour& b )
{
	return a.r() == b.r() and a.g() == b.g() and a.b() == b.b();
}

/* //////////////////////////////////////////////////////////////// */

const Gui::Colour& Gui::BLACK()
{
	static Gui::Colour col_( 0, 0, 0 );
	return col_;
}

const Gui::Colour& Gui::WHITE()
{
	static Gui::Colour col_( 1, 1, 1 );
	return col_;
}

const Gui::Colour& Gui::RED()
{
	static Gui::Colour col_( 1, 0, 0 );
	return col_;
}

const Gui::Colour& Gui::GREEN()
{
	static Gui::Colour col_( 0, 1, 0 );
	return col_;
}

const Gui::Colour& Gui::BLUE()
{
	static Gui::Colour col_( 0, 0, 1 );
	return col_;
}

const Gui::Colour& Gui::YELLOW()
{
	static Gui::Colour col_( 1, 1, 0 );
	return col_;
}

const Gui::Colour& Gui::MAGENTA()
{
	static Gui::Colour col_( 1, 0, 1 );
	return col_;
}

const Gui::Colour& Gui::CYAN()
{
	static Gui::Colour col_( 0, 1, 1 );
	return col_;
}

const Gui::Colour& Gui::LIGHTGREY()
{
	static Gui::Colour col_( 0.8, 0.8, 0.8 );
	return col_;
}

const Gui::Colour& Gui::DARKGREY()
{
	static Gui::Colour col_( 0.3, 0.3, 0.3 );
	return col_;
}

const Gui::Colour& Gui::GREY()
{
	static Gui::Colour col_( 0.5, 0.5, 0.5 );
	return col_;
}

/* //////////////////////////////////////////////////////////////// */

// static
GuiBitmap
Gui::bitmap( const SysPathName& path )
{
	ASSERT_FILE_EXISTS( path.pathname().c_str() );
	return GuiBitmap::createSharedSurface( path.pathname(), Gui::backBuffer() );
}

// static
Gui::Box
Gui::bitmapDimensions( const SysPathName& path )
{
	PRE( path.existsAsFile() );
	GuiBitmap bmp = GuiBitmap::createSharedSurface( path.pathname(), Gui::backBuffer() );
	return Gui::Box( 0, 0, bmp.width(), bmp.height() );
}

// static
Gui::Box
Gui::translateBitmapDimensions( const SysPathName& path, const Gui::Coord& rel )
{
	return GuiDisplayable::translateBox( bitmapDimensions( path ), rel );
}

// static
void Gui::writeScreenAsBmp( const char* startFilename )
{
    const RenSurface& surface = Gui::backBuffer();

	// Create next filename...

    bool    gotBmpPathName = false;
    SysPathName bmpPathName;
    size_t  count = 0;

	while ( not gotBmpPathName )
	{
		char buffer[20];

        sprintf( buffer, "%04ld", count );

        //bmpPathName = string( startFilename ) + buffer + ".bmp";
        bmpPathName = SysPathName(string( startFilename ) + buffer + ".bmp");

        if( not bmpPathName.existsAsFile() )
            gotBmpPathName = true;

        ++count;
	}

	surface.saveAsBmp( RenSurface::Rect( 0, 0, surface.width(), surface.height() ), bmpPathName );
}

/* //////////////////////////////////////////////////////////////// */

/* End **************************************************************/
