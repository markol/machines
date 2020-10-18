/*
 * I M A G E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/image.hpp"
#include "gui/painter.hpp"

//////////////////////////////////////////////////////////////////////

GuiImage::GuiImage( GuiDisplayable *pParent, const Gui::Coord& rel, const GuiBitmap& bitmap )
: GuiDisplayable(  pParent, translateBox( Gui::Box( 0, 0, bitmap.width(), bitmap.height() ), rel ) ),
  image_( bitmap )
{
	// Intentionally Empty
}

GuiImage::~GuiImage()
{
	// Intentionally Empty
}

void GuiImage::doDisplay()
{
	GuiPainter::instance().blit( image_, absoluteCoord() );
}

//////////////////////////////////////////////////////////////////////

/* End IMAGE.CPP ****************************************************/
