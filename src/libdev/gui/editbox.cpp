/*
 * E D I T B O X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gui/editbox.hpp"
#include "gui/painter.hpp"
#include "gui/event.hpp"
#include "gui/manager.hpp"
#include "device/time.hpp"
#include "device/butevent.hpp"

GuiSingleLineEditBox::GuiSingleLineEditBox( GuiDisplayable* pParent, const Gui::Box& box, const GuiBmpFont& font )
:	GuiDisplayable( pParent, box ),
	font_( font ),
	showCaret_( true ),
	backgroundColour_( Gui::BLACK() ),
	caretColour_( Gui::WHITE() ),
	maxChars_( 0 ),
	caretPos_( 0 ),
	dontUpdate_( true ),
	border_( false ),
	borderColour_( Gui::WHITE() )
{

    TEST_INVARIANT;
}

GuiSingleLineEditBox::~GuiSingleLineEditBox()
{
    TEST_INVARIANT;

}

void GuiSingleLineEditBox::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GuiSingleLineEditBox& t )
{

    o << "GuiSingleLineEditBox " << (void*)&t << " start" << std::endl;
    o << "GuiSingleLineEditBox " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void GuiSingleLineEditBox::doDisplay()
{
	drawBackground();

	int offset = border_ ? 2 : 0;

	Gui::Coord startText = Gui::Coord( absoluteBoundary().minCorner().x() + caretPos_ + offset, absoluteBoundary().minCorner().y() + offset );

	font_.drawText( rightText_, startText, maxWidth() - caretPos_ );
	font_.drawText( leftText_, startText, caretPos_, GuiBmpFont::RIGHT_JUSTIFY );

	if ( GuiManager::instance().charFocusExists() and
		 &GuiManager::instance().charFocus() == this and
		 showCaret_ ) // Only show caret if we have focus
	{
		GuiPainter::instance().line(Gui::Coord( absoluteBoundary().minCorner().x() + caretPos_ + offset, absoluteBoundary().minCorner().y() + offset ),
								 	Gui::Coord( absoluteBoundary().minCorner().x() + caretPos_ + offset, absoluteBoundary().maxCorner().y() ),
								 	caretColour_, 1 );
	}

	if ( border_ )
	{
		GuiPainter::instance().hollowRectangle( absoluteBoundary(), borderColour_, 1 );
	}
}

void GuiSingleLineEditBox::update()
{
	double time = DevTime::instance().time();
	int itime = time * 2;

	bool newShowCaret = itime % 2;
	if ( not dontUpdate_ and newShowCaret != showCaret_ )
	{
		showCaret_ = newShowCaret;
		changed();
	}

	dontUpdate_ = false;
}

// virtual
bool GuiSingleLineEditBox::doHandleCharEvent( const GuiCharEvent& e )
{
	// Check that char is usable
	if ( font_.charWidth( e.getChar() ) != 0 )
	{
		// Check to see if adding this character is allowed, i.e. length of string will
		// be less than or equal to maxChars_ ( maximum length of string allowed ).
		if ( ( leftText_.length() + rightText_.length() ) < maxChars_ or maxChars_ == 0 )
		{
	 		leftText_ += e.getChar();

			caretPos_ += font_.charWidth( e.getChar() ) + font_.spacing();

			if ( caretPos_ >= maxWidth() - ( border_ ? 4 : 0 ) )
				caretPos_ = maxWidth() - ( border_ ? 5 : 1 );

			forceRedraw();
		}

		return true;
	}

	return false;
}

// virtual
bool GuiSingleLineEditBox::doHandleKeyEvent( const GuiKeyEvent& e )
{
	bool processed = false;

	if ( e.buttonEvent().action() == DevButtonEvent::PRESS )
	{
		processed = true;
		switch ( e.buttonEvent().scanCode() )
		{
			case DevKey::LEFT_ARROW :
				leftArrowEvent();
				break;
			case DevKey::RIGHT_ARROW :
				rightArrowEvent();
				break;
			case DevKey::BACK_SPACE :
				backspaceEvent();
				break;
			case DevKey::KEY_DELETE :
				deleteEvent();
				break;
			case DevKey::HOME :
				homeEvent();
				break;
			case DevKey::END :
				endEvent();
				break;
			default:
				processed = false;
				break;
		}
	}

	return processed;
}

void GuiSingleLineEditBox::leftArrowEvent()
{
	// Check to see if were not already at the beginning of the edit box
	if ( not leftText_.length() == 0 )
	{
		char c = leftText_[ leftText_.length() - 1 ];
		//leftText_.remove( leftText_.length() - 1 );
		leftText_.erase( leftText_.length() - 1, 1 );
		rightText_ = c + rightText_;
		caretPos_ -= font_.charWidth( c ) + font_.spacing();

		// Make sure caret is set to beginning of edit box if it has been
		// moved too far back
		if ( caretPos_ < 0 )
			caretPos_ = 0;

		forceRedraw();
	}
}

void GuiSingleLineEditBox::rightArrowEvent()
{
	// Check to see if were not already at the end of the edit box
	if ( not rightText_.length() == 0 )
	{
		char c = rightText_[ 0 ];
		//rightText_.remove( 0, 1 );
		rightText_.erase( 0, 1 );
		leftText_ = leftText_ + c;
		caretPos_ += font_.charWidth( c ) + font_.spacing();

		// Make sure caret is set to end of edit box if it has been
		// moved too far
		if ( caretPos_ >= maxWidth() - ( border_ ? 4 : 0 ) )
			caretPos_ = maxWidth() - ( border_ ? 5 : 1 );

		forceRedraw();
	}
}

void GuiSingleLineEditBox::homeEvent()
{
	// Check to see if were not already at the beginning of the edit box
	if ( not leftText_.length() == 0 )
	{
		rightText_ = leftText_ + rightText_;
		//leftText_.remove( 0 );
		leftText_.erase( 0 );
		caretPos_ = 0;

		forceRedraw();
	}
}

void GuiSingleLineEditBox::endEvent()
{
	// Check to see if were not already at the end of the edit box
	if ( not rightText_.length() == 0 )
	{
		leftText_ = leftText_ + rightText_;

		for ( int i = 0; i < rightText_.length(); ++i )
		{
			caretPos_ += font_.charWidth( rightText_[i] ) + font_.spacing();
		}
		if ( caretPos_ >= maxWidth() - ( border_ ? 4 : 0 ) )
			caretPos_ = maxWidth() - ( border_ ? 5 : 1 );

		//rightText_.remove( 0 );
		rightText_.erase( 0 );

		forceRedraw();
	}
}

void GuiSingleLineEditBox::backspaceEvent()
{
	// Check to see if were not already at the beginning of the edit box
	if ( not leftText_.length() == 0 )
	{
		char c = leftText_[ leftText_.length() - 1 ];
		//leftText_.remove( leftText_.length() - 1 );
		leftText_.erase( leftText_.length() - 1, 1);
		caretPos_ -= font_.charWidth( c ) + font_.spacing();

		if ( caretPos_ < 0 )
		{
			caretPos_ = 0;
			// Jump caret fowards 5 character widths
			int pos = leftText_.length();
			int count = 5;
			while ( pos and --count )
			{
				--pos;
				caretPos_ += font_.charWidth( leftText_[pos] ) + font_.spacing();
			}
		}

		forceRedraw();
	}
}

void GuiSingleLineEditBox::deleteEvent()
{
	// Check to see if were not already at the end of the edit box
	if ( not rightText_.length() == 0 )
	{
		char c = rightText_[ 0 ];
		//rightText_.remove( 0, 1 );
		rightText_.erase( 0, 1 );

		forceRedraw();
	}
}

void GuiSingleLineEditBox::forceRedraw()
{
	// Redraw edit box
	showCaret_ = true;
	dontUpdate_ = true;
	changed();
}

void GuiSingleLineEditBox::text( const string& newText )
{
	PRE( maxChars_ ? newText.length() <= maxChars_ : true );

	rightText_ = newText;
	//leftText_.remove( 0 );
	leftText_.erase( 0 );
	caretPos_ = 0;

	forceRedraw();
}

string GuiSingleLineEditBox::text() const
{
	string retVal;
	retVal = leftText_ + rightText_;

	return retVal;
}

//virtual
void GuiSingleLineEditBox::drawBackground()
{
	GuiPainter::instance().filledRectangle( absoluteBoundary(), backgroundColour_ );
}

void GuiSingleLineEditBox::maxChars( size_t newMaxChars )
{
	maxChars_ = newMaxChars;
}

void GuiSingleLineEditBox::backgroundColour( const GuiColour& colour )
{
	backgroundColour_ = colour;
}

void GuiSingleLineEditBox::caretColour( const GuiColour& colour )
{
	caretColour_ = colour;
}

void GuiSingleLineEditBox::border( bool b )
{
	border_ = b;
}

void GuiSingleLineEditBox::borderColour( const GuiColour& colour )
{
	borderColour_ = colour;
}

//virtual
void GuiSingleLineEditBox::doHandleMouseClickEvent( const GuiMouseEvent& )
{
	GuiManager::instance().charFocus( this );
}

const string& GuiSingleLineEditBox::leftText() const
{
	return leftText_;
}

const string& GuiSingleLineEditBox::rightText() const
{
	return rightText_;
}

Gui::XCoord GuiSingleLineEditBox::caretPos() const
{
	return caretPos_;
}

bool GuiSingleLineEditBox::showCaret() const
{
	return showCaret_;
}

const GuiColour& GuiSingleLineEditBox::caretColour() const
{
	return caretColour_;
}

//virtual
int GuiSingleLineEditBox::maxWidth() const
{
	return width();
}

/* End EDITBOX.CPP **************************************************/
