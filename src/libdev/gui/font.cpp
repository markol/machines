/*
 * F O N T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "gui/internal/fontcore.hpp"
#include "base/persist.hpp"

PER_DEFINE_PERSISTENT( GuiBmpFontCoreCharData );

GuiBmpFontCore::GuiBmpFontCore( const SysPathName& fontPath )
{
	fontBmp_ = Gui::bitmap( fontPath );
	fontBmp_.enableColourKeying();
	fontPath_ = fontPath;
	coreCount_ = 1;
	charHeight_ = fontBmp_.height() - 1 /*bitmap has extra undisplayable line at bottom*/;

	SysPathName persistFontPath = fontPath;
	persistFontPath.extension( "bin" );
	if ( persistFontPath.existsAsFile() )
	{
		createFromBinaryFile( persistFontPath );
	}
	else
	{
		calculateProportionalFontWidthData( persistFontPath );
	}
}

void GuiBmpFontCore::createFromBinaryFile( const SysPathName& persistFontPath )
{
	DEBUG_STREAM( DIAG_NEIL, "GuiBmpFontCore::createFromBinaryFile " << persistFontPath << std::endl );
	std::ifstream	str( persistFontPath.c_str(), std::ios::binary );
    PerIstream	istr( str );

    istr >> charData_;
	istr >> maxCharWidth_;
	DEBUG_STREAM( DIAG_NEIL, "GuiBmpFontCore::createFromBinaryFile exit maxCharWidth_ " << maxCharWidth_ << std::endl );
}

void GuiBmpFontCore::calculateProportionalFontWidthData( const SysPathName& persistFontPath )
{
	int bmpXPos = 0;
	maxCharWidth_ = 0;

	while ( bmpXPos < fontBmp_.width() )
	{
		int startXPos = bmpXPos;

		while ( bmpXPos < fontBmp_.width() and not endOfChar( bmpXPos ) )
		{
			++bmpXPos;
		}

		GuiBmpFontCoreCharData newCharData;
		newCharData.width_ = bmpXPos - startXPos;
		// If this is widest character then update maxCharWidth_
		maxCharWidth_ = std::max( maxCharWidth_, newCharData.width_ );
		newCharData.offset_ = startXPos;
		charData_.push_back( newCharData );

		++bmpXPos;
	}

	std::ofstream	str( persistFontPath.c_str(), std::ios::binary );
    PerOstream	ostr( str );

    ostr << charData_;
	ostr << maxCharWidth_;

	DEBUG_STREAM( DIAG_NEIL, "GuiBmpFontCore::calculateProportionalFontWidthData ( num chars : " << charData_.size() << " ) maxCharWidth_ " << maxCharWidth_ << std::endl );
}

bool GuiBmpFontCore::endOfChar( int x )
{
	RenColour colour;
	RenColour white( Gui::WHITE() );
	int height = fontBmp_.height();

	fontBmp_.getPixel( x, height - 1, &colour );

	return colour.operator == (white);
}

//static
//GuiBmpFonts& GuiBmpFont::getFontCache()
//{
//	static GuiBmpFonts fonts;
//	return fonts;
//}

// This should be a private static member function as above but Watcoms crappy compiler
// can't handle the template code.
typedef ctl_vector< GuiBmpFont > GuiBmpFonts;

GuiBmpFonts& getFontCache()
{
	static GuiBmpFonts fonts;
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        fonts.reserve( 10 );
    }

	return fonts;
}

//static
void GuiBmpFont::releaseFontMemory()
{
	GuiBmpFonts& fonts = getFontCache();
	// Remove all fonts from the cache. Assuming no other code is holding onto the
	// font core memory then this will be released.
	fonts.erase( fonts.begin(), fonts.end() );
}


//static
GuiBmpFont GuiBmpFont::getFont( const SysPathName& fontPath, FontType fontType /*= PROPORTIONAL*/, size_t spaceCharWidth /*= 7*/, size_t spacing /*= 1*/ )
{
	typedef ctl_vector< GuiBmpFont > GuiBmpFonts;

	GuiBmpFonts& fonts = getFontCache();

	for ( GuiBmpFonts::iterator iter = fonts.begin(); iter != fonts.end(); ++iter )
	{
		if ( (*iter).pFontCore_->fontPath_ == fontPath )
		{
			GuiBmpFont font( *iter );
			font.fontType( fontType );
			font.spaceCharWidth( spaceCharWidth );
			font.spacing( spacing );
			return font;
		}
	}

	GuiBmpFont newFont( fontPath );
	newFont.fontType( fontType );
	newFont.spaceCharWidth( spaceCharWidth );
	newFont.spacing( spacing );
	fonts.push_back( newFont );
	return newFont;
}

GuiBmpFont::GuiBmpFont()
{
	ASSERT( false, "Don't call the GuiBmpFont default constructor!!" );
}

GuiBmpFont::GuiBmpFont( const SysPathName& fontPath )
{
	pFontCore_ = _NEW( GuiBmpFontCore( fontPath ) );

	underlineColour_ = Gui::BLACK();
	underline_ = false;

    TEST_INVARIANT;
}

GuiBmpFont::~GuiBmpFont()
{
    TEST_INVARIANT;

	--pFontCore_->coreCount_;
	if ( pFontCore_->coreCount_ == 0 )
	{
		_DELETE( pFontCore_ );
	}
}

GuiBmpFont::GuiBmpFont( const GuiBmpFont& copy )
{
	pFontCore_ = copy.pFontCore_;
	fontType_ = copy.fontType_;
	spaceCharWidth_ = copy.spaceCharWidth_;
	spacing_ = copy.spacing_;
	underlineColour_ = copy.underlineColour_;
	underline_ = copy.underline_;

	++pFontCore_->coreCount_;
}

GuiBmpFont& GuiBmpFont::operator =( const GuiBmpFont& rhs )
{
	if ( this != &rhs )
	{
		pFontCore_ = rhs.pFontCore_;
		fontType_ = rhs.fontType_;
		spaceCharWidth_ = rhs.spaceCharWidth_;
		spacing_ = rhs.spacing_;
		underlineColour_ = rhs.underlineColour_;
		underline_ = rhs.underline_;

		++pFontCore_->coreCount_;
	}

	return *this;
}

void GuiBmpFont::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GuiBmpFont& t )
{

    o << "GuiBmpFont " << (void*)&t << " start" << std::endl;
    o << "GuiBmpFont " << (void*)&t << " end" << std::endl;

    return o;
}

size_t GuiBmpFont::charHeight() const
{
	return pFontCore_->charHeight_;
}

size_t GuiBmpFont::charWidth( char c ) const
{
	if ( c == ' ' )
	{
		return spaceCharWidth_;
	}
	unsigned int index = (unsigned char)c;
	if ( pFontCore_->charData_.size() > index )
	{
		return pFontCore_->charData_[index].width_;
	}
	return 0;
}

size_t GuiBmpFont::maxCharWidth() const
{
	return pFontCore_->maxCharWidth_;
}

void GuiBmpFont::drawText( const string& text, const Gui::Coord& startPos, int maxWidth, Justification justification /*= LEFT_JUSTIFY*/ ) const
{
	switch ( justification )
	{
		case LEFT_JUSTIFY:
			drawTextLeftJustify( text, startPos, maxWidth );
			break;
		case RIGHT_JUSTIFY:
			drawTextRightJustify( text, startPos, maxWidth );
			break;
		DEFAULT_ASSERT_BAD_CASE( justification );
	}
}

void GuiBmpFont::drawText( GuiBitmap* pBmp, const string& text, const Gui::Coord& startPos, int maxWidth, Justification justification /*= LEFT_JUSTIFY*/ ) const
{
	switch ( justification )
	{
		case LEFT_JUSTIFY:
			drawTextLeftJustify( pBmp, text, startPos, maxWidth );
			break;
		case RIGHT_JUSTIFY:
			drawTextRightJustify( pBmp, text, startPos, maxWidth );
			break;
		DEFAULT_ASSERT_BAD_CASE( justification );
	}
}

void GuiBmpFont::drawTextLeftJustify( const string& text, const Gui::Coord& startPos, int maxWidth ) const
{
	Gui::Coord absPos = startPos;
	Gui::XCoord endAbsPos = absPos.x() + maxWidth;

	for ( int i = 0; i < text.length(); ++i )
	{
		if ( text[i] == ' ' ) // Special handling for space character
		{
			absPos.x( absPos.x() + spaceCharWidth_ + spacing_ );
		}
		else if ( charWidth( text[i] ) ) // Check that character is supported by bitmap
		{
			if ( absPos.x() + charWidth( text[i] ) <= endAbsPos )
			{
				// Blit character
				GuiPainter::instance().blit(pFontCore_->fontBmp_,
											Gui::Box( 	Gui::Coord( pFontCore_->charData_[ (unsigned char)text[i] ].offset_, 0 ),
														charWidth( text[i] ) + 1,
														charHeight() ),
											absPos );

				// Add spacing ready for next character blit
				if ( fontType_ == PROPORTIONAL )
					absPos.x( absPos.x() + charWidth( text[i] ) + spacing_ );
				else
					absPos.x( absPos.x() + maxCharWidth() + spacing_ );
			}
			else
			{
				i = text.length();
			}
		}
	}

	if ( underline_ )
	{
		GuiPainter::instance().line( Gui::Coord( startPos.x(), startPos.y() + charHeight() + 1 ),
									 Gui::Coord( absPos.x(), startPos.y() + charHeight() + 1 ),
									 underlineColour_, 1 );
	}
}

void GuiBmpFont::drawTextRightJustify( const string& text, const Gui::Coord& startPos, int maxWidth ) const
{
	Gui::Coord absPos = startPos;
	Gui::XCoord endAbsPos = absPos.x() - maxWidth;

	for ( int i = text.length(); i > 0;  )
	{
		--i;

		if ( text[i] == ' ' ) // Special handling for space character
		{
			absPos.x( absPos.x() - spaceCharWidth_ - spacing_ );
		}
		else if ( charWidth( text[i] ) ) // Check that character is supported by bitmap
		{
			// Add spacing ready for next character blit
			if ( fontType_ == PROPORTIONAL )
				absPos.x( absPos.x() - charWidth( text[i] ) - spacing_ );
			else
				absPos.x( absPos.x() - maxCharWidth() - spacing_ );

			if ( absPos.x() >= endAbsPos )
			{
				// Blit character
				GuiPainter::instance().blit(pFontCore_->fontBmp_,
											Gui::Box( 	Gui::Coord(  pFontCore_->charData_[ (unsigned char)text[i] ].offset_, 0 ),
														charWidth( text[i] ) + 1,
														charHeight() ),
											absPos );
			}
			else
			{
				i = 0;
			}
		}
	}

	if ( underline_ )
	{
		GuiPainter::instance().line( Gui::Coord( startPos.x(), startPos.y() + charHeight() + 1 ),
									 Gui::Coord( absPos.x(), startPos.y() + charHeight() + 1 ),
									 underlineColour_, 1 );
	}
}

void GuiBmpFont::drawTextLeftJustify( GuiBitmap* pBmp, const string& text, const Gui::Coord& startPos, int maxWidth ) const
{
	Gui::Coord absPos = startPos;
	Gui::XCoord endAbsPos = absPos.x() + maxWidth;

	for ( int i = 0; i < text.length(); ++i )
	{
		if ( text[i] == ' ' ) // Special handling for space character
		{
			absPos.x( absPos.x() + spaceCharWidth_ + spacing_ );
		}
		else if ( charWidth( text[i] ) ) // Check that character is supported by bitmap
		{
			if ( absPos.x() + charWidth( text[i] ) <= endAbsPos )
			{
				// Blit character
				pBmp->simpleBlit( pFontCore_->fontBmp_,
								  Ren::Rect(pFontCore_->charData_[ (unsigned char)text[i] ].offset_, 0,
								  			charWidth( text[i] ) + 1,
										   	charHeight() ),
								  absPos.x(), absPos.y() );

				// Add spacing ready for next character blit
				if ( fontType_ == PROPORTIONAL )
					absPos.x( absPos.x() + charWidth( text[i] ) + spacing_ );
				else
					absPos.x( absPos.x() + maxCharWidth() + spacing_ );
			}
			else
			{
				i = text.length();
			}
		}
	}

	// TBD : underline on fonts render onto a bitmap
	//if ( underline_ )
	//{
	//	GuiPainter::instance().line( Gui::Coord( startPos.x(), startPos.y() + charHeight() + 1 ),
	// 								 Gui::Coord( absPos.x(), startPos.y() + charHeight() + 1 ),
	//								 underlineColour_, 1 );
	//}
}

void GuiBmpFont::drawTextRightJustify( GuiBitmap* pBmp, const string& text, const Gui::Coord& startPos, int maxWidth ) const
{
	Gui::Coord absPos = startPos;
	Gui::XCoord endAbsPos = absPos.x() - maxWidth;

	for ( int i = text.length(); i > 0;  )
	{
		--i;

		if ( text[i] == ' ' ) // Special handling for space character
		{
			absPos.x( absPos.x() - spaceCharWidth_ - spacing_ );
		}
		else if ( charWidth( text[i] ) ) // Check that character is supported by bitmap
		{
			// Add spacing ready for next character blit
			if ( fontType_ == PROPORTIONAL )
				absPos.x( absPos.x() - charWidth( text[i] ) - spacing_ );
			else
				absPos.x( absPos.x() - maxCharWidth() - spacing_ );

			if ( absPos.x() >= endAbsPos )
			{
				// Blit character
				pBmp->simpleBlit( pFontCore_->fontBmp_,
								  Ren::Rect(pFontCore_->charData_[ (unsigned char)text[i] ].offset_, 0,
								  			charWidth( text[i] ) + 1,
										   	charHeight() ),
								  absPos.x(), absPos.y() );
			}
			else
			{
				i = 0;
			}
		}
	}

	// TBD : underline on fonts render onto a bitmap
	//if ( underline_ )
	//{
	//	GuiPainter::instance().line( Gui::Coord( startPos.x(), startPos.y() + charHeight() + 1 ),
	//								 Gui::Coord( absPos.x(), startPos.y() + charHeight() + 1 ),
	//								 underlineColour_, 1 );
	//}
}

GuiBmpFont::FontType GuiBmpFont::fontType() const
{
	return fontType_;
}

void GuiBmpFont::fontType( GuiBmpFont::FontType fontType )
{
	fontType_ = fontType;
}

size_t GuiBmpFont::spaceCharWidth() const
{
	return spaceCharWidth_;
}

void GuiBmpFont::spaceCharWidth( size_t spaceCharWidth )
{
	spaceCharWidth_ = spaceCharWidth;
}

size_t GuiBmpFont::spacing() const
{
	return spacing_;
}

void GuiBmpFont::spacing( size_t spacing )
{
	spacing_ = spacing;
}

void GuiBmpFont::underline( bool underl )
{
	underline_ = underl;
}

bool GuiBmpFont::underline() const
{
	return underline_;
}

void GuiBmpFont::underlineColour( const GuiColour& colour )
{
	underlineColour_ = colour;
}

size_t GuiBmpFont::textWidth( const string& text ) const
{
	Gui::Coord absPos(0,0);

	for ( int i = 0; i < text.length(); ++i )
	{
		if ( text[i] == ' ' ) // Special handling for space character
		{
			absPos.x( absPos.x() + spaceCharWidth_ + spacing_ );
		}
		else if ( charWidth( text[i] ) ) // Check that character is supported by bitmap
		{
			// Add character width and spacing width
			if ( fontType_ == PROPORTIONAL )
				absPos.x( absPos.x() + charWidth( text[i] ) + spacing_ );
			else
				absPos.x( absPos.x() + maxCharWidth() + spacing_ );
		}
	}

	return absPos.x();
}

void perWrite( PerOstream& ostr, const GuiBmpFontCoreCharData& data )
{
	ostr << data.offset_;
	ostr << data.width_;
}

void perRead( PerIstream& istr, GuiBmpFontCoreCharData& data )
{
	istr >> data.offset_;
	istr >> data.width_;
}

/* End FONT.CPP *****************************************************/
