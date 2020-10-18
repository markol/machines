/*
 * I M A G E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_IMAGE_HPP
#define _GUI_IMAGE_HPP

#include "gui/gui.hpp"
#include "gui/displaya.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiImage 
: public GuiDisplayable
{
public:

	GuiImage( GuiDisplayable *pParent, const Gui::Coord& rel, const GuiBitmap& bmp );

	virtual ~GuiImage();

protected:

	// inherited from GuiDisplayable...	
	virtual void doDisplay();

private:
	GuiImage( const GuiImage& );
	GuiImage& operator =( const GuiImage& );
	bool operator ==( const GuiImage& ) const;

	GuiBitmap	image_;
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _GUI_IMAGE_HPP

/* End IMAGE.HPP ****************************************************/
