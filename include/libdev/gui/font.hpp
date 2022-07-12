/*
 * F O N T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiBmpFont

    Support for bitmap fonts. Characters can be different widths so you can set the font as
	proportional or fixed spacing. At present, this font only draws text in a single line, it
	will ignore carridge return etc.
*/

#ifndef _GUI_FONT_HPP
#define _GUI_FONT_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"
#include "gui/gui.hpp"

class GuiBmpFontCore;

class GuiBmpFont
// Canonical form revoked
{
public:
	enum Justification { LEFT_JUSTIFY, RIGHT_JUSTIFY };
	enum FontType { PROPORTIONAL, FIXED_SPACE };

	static GuiBmpFont getFont( const SysPathName& fontPath, FontType = PROPORTIONAL, size_t spaceCharWidth = 7, size_t spacing = 1 );

	GuiBmpFont(); // Default constructor to satisfy ctl_vector. DO NOT USE!!!
    GuiBmpFont( const GuiBmpFont& );
    GuiBmpFont& operator =( const GuiBmpFont& );
    ~GuiBmpFont();

	size_t charHeight() const;
    
    size_t charWidth( char c ) const;

    // The maximum width of a character in the font. The spacing that will be used by
	// Fixed space fonts
    size_t maxCharWidth() const;

	// Display text on screen.
	void drawText( const string& text, const Gui::Coord& startPos, int maxWidth, Justification = LEFT_JUSTIFY ) const;

	// Display text on bitmap.
	void drawText( GuiBitmap*, const string& text, const Gui::Coord& startPos, int maxWidth, Justification = LEFT_JUSTIFY ) const;

	// Get/Set font type ( proportional or fixed space ).
	FontType fontType() const;
	void fontType( FontType );

	// Set/Get width of space character. Note that the "spacing" is still used after a space char is displayed.
	size_t spaceCharWidth() const;
	void spaceCharWidth( size_t );

	// Set/Get spacing between characters
	size_t spacing() const;
	void spacing( size_t );

	// Switch underline on/off
	void underline( bool );
	bool underline() const;
	void underlineColour( const GuiColour& );

	// Return the display width of "text" without actually rendering the text.
	size_t textWidth( const string& text ) const;

	// Free's up all the memory used by the cached fonts.
	static void releaseFontMemory();

protected:
    GuiBmpFont( const SysPathName& fontPath );
	void drawTextLeftJustify( const string& text, const Gui::Coord& startPos, int maxWidth ) const;
	void drawTextRightJustify( const string& text, const Gui::Coord& startPos, int maxWidth ) const;
	void drawTextLeftJustify( GuiBitmap*, const string& text, const Gui::Coord& startPos, int maxWidth ) const;
	void drawTextRightJustify( GuiBitmap*, const string& text, const Gui::Coord& startPos, int maxWidth ) const;

private:
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GuiBmpFont& t );

	GuiBmpFontCore* pFontCore_;
	FontType fontType_;
	size_t spaceCharWidth_;
	size_t spacing_;
	bool underline_;
	GuiColour underlineColour_;
};


#endif

/* End FONT.HPP *****************************************************/
