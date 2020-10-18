/*
 * G U I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_GUI_HPP
#define _GUI_GUI_HPP

#include "ctl/utility.hpp"

#include "device/mouse.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec2.hpp"

#include "render/colour.hpp"
#include "render/surface.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;

/************

class GuiColour
{
public:

	GuiColour( double r, double g, double b )
	: r_( r ), g_( g ), b_( b ) {}

	double r() const { return r_; }
	double g() const { return g_; }
	double b() const { return b_; }

	friend bool operator ==( const GuiColour&, const GuiColour& );
	friend ostream& operator <<( ostream&, const GuiColour& );

private:

	double	r_;
	double	g_;
	double	b_;
};

************/

typedef RenColour	GuiColour;

class GuiCoords;
// class GuiBitmap;
typedef RenSurface GuiBitmap;

bool operator <( const GuiBitmap&, const GuiBitmap& );

//namespace Gui
struct Gui
{
	static bool initialised();

	typedef GuiColour		Colour;
	typedef int				XCoord;
	typedef int				YCoord;
	typedef MexPoint2d		Coord;
	typedef MexVec2			Vec;
	typedef MexAlignedBox2d	Boundary;
	typedef MexAlignedBox2d	Box;

	CTL_PAIR( WidthAndHeight, unsigned, width, unsigned, height );

	static unsigned width( const Gui::Box& b );
	static unsigned height( const Gui::Box& b );

	static RenSurface& backBuffer();
	// PRE( Gui::initialised(); )

	///////////////////////////////

	enum ButtonState
	{
		PRESSED		= DevMouse::PRESSED,
		RELEASED	= DevMouse::RELEASED,
		NO_CHANGE	= DevMouse::NO_CHANGE
	};

	///////////////////////////////

	static Gui::Box bitmapDimensions( const SysPathName& path );
	// PRE( path.existsAsFile() );

	static Gui::Box translateBitmapDimensions( const SysPathName& path, const Gui::Coord& rel );
	// PRE( path.existsAsFile() );

	///////////////////////////////

	static GuiBitmap bitmap( const SysPathName& );

	///////////////////////////////

	static const Gui::Colour& BLACK();
	static const Gui::Colour& WHITE();
	static const Gui::Colour& RED();
	static const Gui::Colour& GREEN();
	static const Gui::Colour& BLUE();
	static const Gui::Colour& YELLOW();
	static const Gui::Colour& MAGENTA();
	static const Gui::Colour& CYAN();
	static const Gui::Colour& LIGHTGREY();
	static const Gui::Colour& DARKGREY();
	static const Gui::Colour& GREY();

	///////////////////////////////

	static void backBuffer( const RenSurface& );
	// PRE( pNewBuffer != NULL );

	///////////////////////////////

	// Saves the screen as a bitmap file. startFilename is the first few
	// characters of the bitmap filename, the function will add a four digit
	// number to startFilename ( e.g. writeScreenAsBmp( "XXXX" ) would store
	// a bitmap with the name "XXXX0000.bmp" followed by "XXXX0001.bmp" etc ).
	static void writeScreenAsBmp( const char* startFilename );

private:

	static RenSurface&	staticBackBuffer();
	static bool			initialised_;
};

/* //////////////////////////////////////////////////////////////// */

/****************************

class GuiBitmap
: public RenSurface
{
public:

	GuiBitmap();
	GuiBitmap( const SysPathName& path )
	{
		readFromFile( path, Gui::backBuffer() );
	}

	~GuiBitmap() {};

};

****************************/

/* //////////////////////////////////////////////////////////////// */


#endif	//	#ifndef _GUI_GUI_HPP
