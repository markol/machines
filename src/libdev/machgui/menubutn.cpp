/*
 * M E N U B U T N . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/menubutn.hpp"
#include "machgui/gui.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndwavid.hpp"
#include "sound/smpparam.hpp"
#include "gui/event.hpp"
#include "gui/painter.hpp"
#include "gui/font.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "render/device.hpp"
#include "system/winapi.hpp"

MachGuiMenuButton::MachGuiMenuButton( MachGuiStartupScreens* pParent, const Gui::Box& box, unsigned int stringId, MachGuiStartupScreens::ButtonEvent buttonEvent )
:	GuiDisplayable( pParent, box ),
	MachGuiFocusCapableControl( pParent ),
	pStartupScreens_( pParent ),
	stringId_( stringId ),
	buttonEvent_( buttonEvent ),
	flash_( false ),
	highlighted_( false ),
	disabled_( false )
{

    TEST_INVARIANT;
}

MachGuiMenuButton::MachGuiMenuButton( 	MachGuiStartupScreens* pStartupScreens, const Gui::Box& box, unsigned int stringId, 
										MachGuiStartupScreens::ButtonEvent buttonEvent, GuiDisplayable* pParent )
:	GuiDisplayable( pParent, box ),
	MachGuiFocusCapableControl( pStartupScreens ),
	pStartupScreens_( pStartupScreens ),
	stringId_( stringId ),
	buttonEvent_( buttonEvent ),
	flash_( false ),
	highlighted_( false ),
	disabled_( false )
{

    TEST_INVARIANT;
}

MachGuiMenuButton::~MachGuiMenuButton()
{
    TEST_INVARIANT;

}

void MachGuiMenuButton::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiMenuButton& t )
{

    o << "MachGuiMenuButton " << (void*)&t << " start" << std::endl;
    o << "MachGuiMenuButton " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachGuiMenuButton::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	if ( disabled_ )
	{
		// Can not press button when disabled. Play discouraging sound...
		if ( rel.leftButton() == Gui::RELEASED )
		{
			MachGuiSoundManager::instance().playSound( "gui/sounds/clickbad.wav" ); 
		}

		return;
	}

	if ( rel.leftButton() == Gui::PRESSED )
	{
		flash_ = true;
		changed();
	}
	else
	{
		flash_ = false;
		changed();
		pStartupScreens_->buttonAction( buttonEvent_ );
	}
}

//virtual 
void MachGuiMenuButton::doHandleMouseExitEvent( const GuiMouseEvent& /*rel*/ )
{
	if ( disabled_ )
		return;

	highlighted_ = false;
	flash_ = false;
	changed();
}

//virtual 
void MachGuiMenuButton::doHandleMouseEnterEvent( const GuiMouseEvent& /*rel*/ )
{
	if ( disabled_ )
		return;

	//Set up the sound to be played
	MachGuiSoundManager::instance().playSound( "gui/sounds/highligh.wav" );

	highlighted_ = true;
	changed();
}

//virtual 
void MachGuiMenuButton::doDisplay()
{
	static uint glowWidth = MachGui::buttonGlowBmp().width();
	static uint glowHeight = MachGui::buttonGlowBmp().height();

	ASSERT( glowWidth >= width(), "glow bitmap not wide enough" );
	ASSERT( glowHeight >= height(), "glow bitmap not high enough" );
	ASSERT( MachGui::buttonDisableBmp().width() >= width(), "disable bitmap not wide enough" );
	ASSERT( MachGui::buttonDisableBmp().height() >= height(), "disable bitmap not high enough" );

	uint glowX = ( glowWidth - width() ) / 2.0;
	uint glowY = ( glowHeight - height() ) / 2.0;

	// Draw background to button ( glow or normal backdrop ).
	if ( flash_ or highlighted_ )
	{
		GuiPainter::instance().blit( 	MachGui::buttonGlowBmp(), 
										Gui::Box( 	Gui::Coord( glowX, glowY ), 
													width(), height() ), 
										absoluteBoundary().minCorner() );
	}
	else
	{
		pStartupScreens_->blitBackdrop( absoluteBoundary(), 
										absoluteBoundary().minCorner() );
	}

	GuiBmpFont darkfont( GuiBmpFont::getFont( "gui/menu/largdfnt.bmp" ) );
	GuiBmpFont lightfont( GuiBmpFont::getFont( "gui/menu/largefnt.bmp" ) );
	GuiBmpFont focusfont( GuiBmpFont::getFont( "gui/menu/largyfnt.bmp" ) );

	GuiResourceString str( stringId_ );
	string text = str.asString();

	size_t textWidth = darkfont.textWidth( text );
	size_t textHeight = darkfont.charHeight();

	size_t textX = absoluteBoundary().minCorner().x() + ( width() - textWidth ) / 2.0;
	size_t textY = absoluteBoundary().minCorner().y() + ( height() - textHeight ) / 2.0;

	// Draw text
	if ( flash_ )
	{
		darkfont.underline( true );
		darkfont.drawText( text, Gui::Coord( textX, textY ), 1000 );
	}
	else if ( highlighted_ )
	{
		darkfont.drawText( text, Gui::Coord( textX, textY ), 1000 );
	}
	else
	{
		if ( isFocusControl() )
		{
			focusfont.drawText( text, Gui::Coord( textX, textY ), 1000 );
		}
		else
		{
			lightfont.drawText( text, Gui::Coord( textX, textY ), 1000 );
		}
	}

	// Show disabled button if necessary
	if ( disabled_ )
	{
		GuiPainter::instance().blit( 	MachGui::buttonDisableBmp(), 
										Gui::Box( 	Gui::Coord( 0, 0 ), 
													width(), height() ), 
										absoluteBoundary().minCorner() );
	}

}

void MachGuiMenuButton::disabled( bool disable )
{
	if ( disabled_ != disable )
	{
		disabled_ = disable;
		flash_ = false;
		highlighted_ = false;
		changed();
	}
}

bool MachGuiMenuButton::highlighted() const
{
	return highlighted_;
}

uint MachGuiMenuButton::stringId() const
{
	return stringId_;
}

//virtual 
bool MachGuiMenuButton::isDisabled() const
{
	return disabled_ or MachGuiFocusCapableControl::isDisabled();
}

//virtual 
bool MachGuiMenuButton::executeControl()
{
	static uint glowWidth = MachGui::buttonGlowBmp().width();
	static uint glowHeight = MachGui::buttonGlowBmp().height();

	ASSERT( glowWidth >= width(), "glow bitmap not wide enough" );
	ASSERT( glowHeight >= height(), "glow bitmap not high enough" );
	ASSERT( MachGui::buttonDisableBmp().width() >= width(), "disable bitmap not wide enough" );
	ASSERT( MachGui::buttonDisableBmp().height() >= height(), "disable bitmap not high enough" );

	uint glowX = ( glowWidth - width() ) / 2.0;
	uint glowY = ( glowHeight - height() ) / 2.0;

	// Draw glow background to button.
	RenSurface frontSurface = RenDevice::current()->frontSurface();
	frontSurface.simpleBlit(MachGui::buttonGlowBmp(), 
							Ren::Rect( glowX, glowY, width(), height() ), 
							absoluteBoundary().minCorner().x(),
							absoluteBoundary().minCorner().y() );

	// Draw dark text
	GuiBmpFont darkfont( GuiBmpFont::getFont( "gui/menu/largdfnt.bmp" ) );
	
	GuiResourceString str( stringId_ );
	string text = str.asString();

	size_t textWidth = darkfont.textWidth( text );
	size_t textHeight = darkfont.charHeight();

	size_t textX = absoluteBoundary().minCorner().x() + ( width() - textWidth ) / 2.0;
	size_t textY = absoluteBoundary().minCorner().y() + ( height() - textHeight ) / 2.0;

	// Draw text
	darkfont.drawText( &frontSurface, text, Gui::Coord( textX, textY ), 1000 );

	// Slight pause so on fast PCs you can still see buttons flash
	SysWindowsAPI::sleep( 100 );

	// Refresh button
	changed();

	// Initiate button action
	pStartupScreens_->buttonAction( buttonEvent_ );
		
	return true;
}

//virtual 
void MachGuiMenuButton::hasFocus( bool newValue )
{
	MachGuiFocusCapableControl::hasFocus( newValue );

	changed();
}

/* End MENUBUTN.CPP *************************************************/
