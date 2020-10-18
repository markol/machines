/*
 * M O U S E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "gui/manager.hpp"
#include "gui/mouse.hpp"
#include "gui/painter.hpp"

#include "system/pathname.hpp"

/* //////////////////////////////////////////////////////////////// */

//static
GuiMouse&
GuiMouse::instance()
{
	static GuiMouse instance_;
	return instance_;
}

/* //////////////////////////////////////////////////////////////// */

GuiMouse::GuiMouse()
: frame_( 0 )
{
//	DevMouse::instance().range( DevMouse::Position( 0, 0 ),
//									DevMouse::Position( GuiManager::instance().width() - cursor_.width(),
//															GuiManager::instance().height() - cursor_.height() ) );

	position_[ 0 ] = DevMouse::instance().position();
	position_[ 1 ] = position_[ 0 ];

//	cursor( GuiMouse::defaultCursor() );

	pickUpBackground( 0 );
	pickUpBackground( 1 );
}

void GuiMouse::cursor( const GuiBitmap& /* curs */ )
{
 //	pCursor_ = &curs;
//	_DELETE( pBackground_[ 0 ] );
//	_DELETE( pBackground_[ 1 ] );
//	pBackground_[ 0 ] = _NEW( GuiBitmap( curs.width(), curs.height(), Gui::backBuffer() ) );
//	pBackground_[ 1 ] = _NEW( GuiBitmap( curs.width(), curs.height(), Gui::backBuffer() ) );
}

GuiMouse::~GuiMouse()
{
//	_DELETE( pBackground_[ 0 ] );
//	_DELETE( pBackground_[ 1 ] );
}

/* //////////////////////////////////////////////////////////////// */

void GuiMouse::pickUpBackground( unsigned /* frame */ )
{
//	pBackground_[ frame ]->simpleBlit( Gui::backBuffer(),
//								RenSurface::Rect( position_[ frame ].first, position_[ frame ].second,
//											pBackground_[ frame ]->width() - 1,
//											pBackground_[ frame ]->height() - 1 ) );
}

void GuiMouse::putDownBackground( unsigned /* frame */ )
{
//	Gui::backBuffer().simpleBlit( *pBackground_[ frame ],
//									RenSurface::Rect( 0, 0, pBackground_[ frame ]->width(),
//															pBackground_[ frame ]->height() ),
//									position_[ frame ].first, position_[ frame ].second );
}

void GuiMouse::update( const Gui::Coord& mouseCoord )
{
	unsigned nextFrame = ( frame_ == 1 ? 0 : 1 );
//	putDownBackground( frame_ );
	position_[ frame_ ] = position_[ nextFrame ];
	position_[ nextFrame ] = DevMouse::Position( mouseCoord.x(), mouseCoord.y() );
	// DevMouse::instance().position();

	frame_ = nextFrame;
	pickUpBackground( frame_ );

//	Gui::backBuffer().simpleBlit( *pCursor_,
//									RenSurface::Rect( 0, 0, pCursor_->width(), pCursor_->height() ),
//									position_[ frame_ ].first, position_[ frame_ ].second );
}

/* //////////////////////////////////////////////////////////////// */

/* End MOUSE.CPP ****************************************************/
