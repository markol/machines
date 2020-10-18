/*
 * M E N U T E X T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/menutext.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "gui/font.hpp"

MachGuiMenuText::MachGuiMenuText( GuiDisplayable* pParent, const Gui::Box& box, unsigned int stringId, const SysPathName& fontPath, Justification justification )
:	GuiDisplayable( pParent, box ),
	fontPath_( fontPath ),
	justification_( justification )
{
    strings_.reserve( 128 );
	GuiResourceString str( stringId );
	string wholeStr = str.asString();

	chopUpText( wholeStr, width(), GuiBmpFont::getFont( fontPath_ ), &strings_ );

    TEST_INVARIANT;
}

MachGuiMenuText::MachGuiMenuText( GuiDisplayable* pParent, const Gui::Box& box, const string& str, const SysPathName& fontPath, Justification justification )
:	GuiDisplayable( pParent, box ),
	fontPath_( fontPath ),
	justification_( justification )
{
    strings_.reserve( 128 );
	chopUpText( str, width(), GuiBmpFont::getFont( fontPath_ ), &strings_ );

    TEST_INVARIANT;
}

MachGuiMenuText::~MachGuiMenuText()
{
    TEST_INVARIANT;

}

void MachGuiMenuText::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiMenuText& t )
{

    o << "MachGuiMenuText " << (void*)&t << " start" << std::endl;
    o << "MachGuiMenuText " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiMenuText::doDisplay()
{
	GuiBmpFont font( GuiBmpFont::getFont( fontPath_ ) );

	size_t textHeight = font.charHeight();
	size_t totalHeight = ( strings_.size() * ( textHeight + 1 ) ) - 1;

	ASSERT_INFO( totalHeight );
	ASSERT_INFO( height() );
	ASSERT( totalHeight <= height(), "height required to render text is greater than the height assigned" );

	size_t startY = absoluteBoundary().minCorner().y() + ( ( height() - totalHeight ) / 2.0 );

	for ( int i = 0; i < strings_.size(); ++i )
	{
		size_t textWidth = font.textWidth( strings_[i] );

		size_t textX = 0;

		switch ( justification_ )
		{
			case CENTRE_JUSTIFY:
				textX = absoluteBoundary().minCorner().x() + ( ( width() - textWidth ) / 2.0 );
				break;
			case RIGHT_JUSTIFY:
				textX = absoluteBoundary().minCorner().x() + width() - textWidth;
				break;
			case LEFT_JUSTIFY:
				textX = absoluteBoundary().minCorner().x();
				break;
			DEFAULT_ASSERT_BAD_CASE( justification_ );
		}
		size_t textY = startY + ( i * ( textHeight + 1 ) );

		font.drawText( strings_[i], Gui::Coord( textX, textY ), 1000 );
	}
}


// static
void MachGuiMenuText::chopUpText( const string& text, size_t maxWidth, const GuiBmpFont& font, strings* pStrings  )
{
	// WARNING : this function breaks down if the width is not big enough to store at least 6 characters.

	string choppedUpText;
	int charPos = 0;
	int textLength = text.length();
	int curWidth = 0;
	bool beginningOfLine = true;

	while ( charPos < textLength )
	{
		// Ignore spaces at beginning of line
		if ( beginningOfLine and text[charPos] == ' ' )
		{}
		// Force new line
		else if ( text[charPos] == '\n' )
		{
			// Reset beginningOfLine flag
			beginningOfLine = true;
			pStrings->push_back( choppedUpText );
			choppedUpText = "";
			curWidth = 0;
		}
		// Is text gona be too wide if we add this character?
		else if ( curWidth + font.charWidth( text[charPos] ) + font.spacing() > maxWidth )
		{
			// Get pointer to character ten position from end of string.
			const char* almostStrEnd = &choppedUpText.c_str()[ choppedUpText.length() - 11 ];

			// Add part of text to pStrings...

			// Do we have a nice break, i.e. a space character, therefore not having to
			// chop a word in two...
			if ( text[charPos] == ' ' )
			{
				pStrings->push_back( choppedUpText );
				choppedUpText = "";
				curWidth = 0;

				// Reset beginningOfLine flag
				beginningOfLine = true;

			}
			// Does space exist in last ten characters? If it does it's worth moving the
			// beginning of the word onto the next line...
			else if ( strchr( almostStrEnd, ' ' ) )
			{
				// Remove beginning of last word from this line and stuff on next line...
				string newLine;
				curWidth = 0;
				// Remove last chars until we find a space
				while ( choppedUpText[choppedUpText.length() - 1 ] != ' ' )
				{
					char lastChar = choppedUpText[choppedUpText.length() - 1 ];
					newLine = lastChar + newLine;
					curWidth += font.charWidth( lastChar ) + font.spacing();
					//choppedUpText.remove( choppedUpText.length() - 1 );
					// TODO check
					choppedUpText.erase( choppedUpText.length() - 1 , 1);
				}
				newLine += text[charPos];

				curWidth += font.charWidth( text[charPos] ) + font.spacing();
				pStrings->push_back( choppedUpText );
				choppedUpText = newLine;
				// We are putting stuff onto the next line therefore the beginningOfLine flag should not be reset.
				beginningOfLine = false;
			}
			else
			{
				// Splitting word across two lines...
				string newLine;
				size_t hyphenWidth = font.charWidth('-') + font.spacing();
				curWidth = 0;

				// Remove as many characters as necessary so that a hyphen can be
				// added at the end of the line.
				while ( curWidth < hyphenWidth )
				{
					char lastChar = choppedUpText[choppedUpText.length() - 1 ];
					newLine = lastChar + newLine;
					curWidth += font.charWidth( lastChar ) + font.spacing();
					//choppedUpText.remove( choppedUpText.length() - 1 );
					choppedUpText.erase( choppedUpText.length() - 1 , 1);
				}

			    choppedUpText += '-';
				pStrings->push_back( choppedUpText );
				// Start of next line
				choppedUpText = newLine;
				choppedUpText += text[charPos];
				curWidth += font.charWidth( text[charPos] ) + font.spacing();

				// We are putting stuff onto the next line therefore the beginningOfLine flag should not be reset.
				beginningOfLine = false;
			}
		}
		else
		{
			// We CAN add this character without overflowing maxWidth...
			choppedUpText += text[charPos];
			curWidth += font.charWidth( text[charPos] ) + font.spacing();

			beginningOfLine = false;
		}

		++charPos;
	}

	if ( choppedUpText.length() != 0 )
	{
		pStrings->push_back( choppedUpText );
	}
}

/* End MENUTEXT.CPP *************************************************/
