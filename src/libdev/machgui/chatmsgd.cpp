/*
 * C H A T M S G D . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/chatmsgd.hpp"
#include "machgui/chatmsgs.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "gui/event.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "render/surface.hpp"
#include "stdlib/string.hpp"
#include "ctl/list.hpp"

struct MachGuiInGameChatMessagesDisplayImpl
{
	MachGuiInGameChatMessagesDisplayImpl();

	GuiBitmap textBmp_;
	GuiBmpFont font_;
	GuiBmpFont shadowFont_;
	bool forceUpdate_;
	GuiDisplayable* pPassEventsTo_;
};

MachGuiInGameChatMessagesDisplayImpl::MachGuiInGameChatMessagesDisplayImpl()
:	font_( GuiBmpFont::getFont( "gui/menu/promtfnt.bmp" ) ),
	shadowFont_( GuiBmpFont::getFont( "gui/menu/promdfnt.bmp" ) ),
	forceUpdate_( true ),
	pPassEventsTo_( NULL )
{}

MachGuiInGameChatMessagesDisplay::MachGuiInGameChatMessagesDisplay( GuiDisplayable* pParent, const Gui::Box& relBox )
:	GuiDisplayable( pParent, relBox, GuiDisplayable::LAYER4 ),
	pImpl_( _NEW( MachGuiInGameChatMessagesDisplayImpl() ) )
{
	CB_DEPIMPL( GuiBitmap, textBmp_ );

	textBmp_ = RenSurface::createAnonymousSurface( width(), height(), W4dManager::instance().sceneManager()->pDevice()->backSurface() );
	textBmp_.enableColourKeying();

	redrawEveryFrame( true );

    TEST_INVARIANT;
}

MachGuiInGameChatMessagesDisplay::MachGuiInGameChatMessagesDisplay( GuiDisplayable* pParent, const Gui::Box& relBox, GuiDisplayable* pPassEventsTo )
:	GuiDisplayable( pParent, relBox, GuiDisplayable::LAYER4 ),
	pImpl_( _NEW( MachGuiInGameChatMessagesDisplayImpl() ) )
{
	CB_DEPIMPL( GuiBitmap, textBmp_ );
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	textBmp_ = RenSurface::createAnonymousSurface( width(), height(), W4dManager::instance().sceneManager()->pDevice()->backSurface() );
	textBmp_.enableColourKeying();

	pPassEventsTo_ = pPassEventsTo;

	redrawEveryFrame( true );

    TEST_INVARIANT;
}

MachGuiInGameChatMessagesDisplay::~MachGuiInGameChatMessagesDisplay()
{
    TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachGuiInGameChatMessagesDisplay::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiInGameChatMessagesDisplay& t )
{

    o << "MachGuiInGameChatMessagesDisplay " << (void*)&t << " start" << std::endl;
    o << "MachGuiInGameChatMessagesDisplay " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void MachGuiInGameChatMessagesDisplay::doDisplay()
{
	CB_DEPIMPL( GuiBitmap, textBmp_ );
	CB_DEPIMPL( GuiBmpFont, font_ );
	CB_DEPIMPL( GuiBmpFont, shadowFont_ );
	CB_DEPIMPL( bool, forceUpdate_ );

	bool needsUpdate = MachGuiInGameChatMessages::instance().update();

	if ( ( needsUpdate and MachGuiInGameChatMessages::instance().messages().size() > 0 ) or forceUpdate_ )
	{
		forceUpdate_ = false;

		// Reset bitmap to magenta ( transparent )
		textBmp_.filledRectangle( RenSurface::Rect( 0, 0, textBmp_.width(), textBmp_.height() ), Gui::MAGENTA() );

		// Render up to five lines of chat messages
		int fontHeight = shadowFont_.charHeight() + 1;
		int index = 0;

		for ( ctl_list<string>::const_iterator iter = MachGuiInGameChatMessages::instance().messages().begin();
			  iter != MachGuiInGameChatMessages::instance().messages().end();
			  ++iter )
		{
			shadowFont_.drawText( &textBmp_, *iter, Gui::Coord( 1, 1 + ( fontHeight * index ) ), width() );
			font_.drawText( &textBmp_, *iter, Gui::Coord( 0, fontHeight * index ), width() );
			++index;
		}
	}
	
	if ( MachGuiInGameChatMessages::instance().messages().size() > 0 )
	{
		// Blit text.
		GuiPainter::instance().blit( textBmp_, absoluteBoundary().minCorner() );
	}
}

void MachGuiInGameChatMessagesDisplay::forceUpdate()
{
	CB_DEPIMPL( bool, forceUpdate_ );

	forceUpdate_ = true;
}

/* NA 30/11/98. New processesMouseEvents added to GuiDisplayable means I don't need these
//virtual 
void MachGuiInGameChatMessagesDisplay::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;
	
	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleMouseClickEvent( relToOther );	
	}
}

//virtual 
void MachGuiInGameChatMessagesDisplay::doHandleMouseEnterEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;
	
	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleMouseEnterEvent( relToOther );	
	}
}

//virtual 
void MachGuiInGameChatMessagesDisplay::doHandleMouseExitEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;
	
	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleContainsMouseEvent( relToOther );	
	}
	else
	{
		pPassEventsTo_->doHandleMouseExitEvent( relToOther );	
	}
}

//virtual 
void MachGuiInGameChatMessagesDisplay::doHandleContainsMouseEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;
	
	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleContainsMouseEvent( relToOther );	
	}
}


bool MachGuiInGameChatMessagesDisplay::passEventTo( GuiMouseEvent* pMouseEvent )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	bool returnValue = false;

	if ( pPassEventsTo_ )
	{
		Gui::Coord absEventPos = absoluteBoundary().minCorner();
		absEventPos.x( absEventPos.x() + pMouseEvent->coord().x() );
		absEventPos.y( absEventPos.y() + pMouseEvent->coord().y() );

		if ( pPassEventsTo_->contains( absEventPos ) )
		{
			pMouseEvent->translate( Gui::Vec( ( absEventPos.x() - pPassEventsTo_->absoluteCoord().x() ) - pMouseEvent->coord().x(), 
											  ( absEventPos.y() - pPassEventsTo_->absoluteCoord().y() ) - pMouseEvent->coord().y() ) );
			returnValue = true;
		}
	}

	return returnValue;
} */

//virtual 
bool MachGuiInGameChatMessagesDisplay::processesMouseEvents()	const
{
	return false;
}

/* End CHATMSGD.CPP *************************************************/
