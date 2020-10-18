/*
 * C A M S C R O L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/camscrol.hpp"
#include "machgui/cameras.hpp"
#include "machgui/ingame.hpp"
#include "machgui/gui.hpp"
#include "gui/event.hpp"

MachGuiCameraScrollArea::MachGuiCameraScrollArea( 	GuiDisplayable* pParent, 
													const Gui::Box& rel, 
													POSITION pos, 
													MachCameras* pCameras, 
													MachInGameScreen* pInGameScreen )
:	GuiDisplayable( pParent, rel, GuiDisplayable::LAYER5 ),
	pos_( pos ),
	pCameras_( pCameras ),
	pInGameScreen_( pInGameScreen )
{

    TEST_INVARIANT;
}

MachGuiCameraScrollArea::~MachGuiCameraScrollArea()
{
    TEST_INVARIANT;

}

void MachGuiCameraScrollArea::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCameraScrollArea& t )
{

    o << "MachGuiCameraScrollArea " << (void*)&t << " start" << std::endl;
    o << "MachGuiCameraScrollArea " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachGuiCameraScrollArea::doHandleContainsMouseEvent( const GuiMouseEvent& event )
{
	// Scroll LEFT
	if ( pos_ == Left )
	{
		pCameras_->scroll( MachCameras::LEFT, event );

		if ( event.coord().y() < 10 )
		{
			pCameras_->scroll( MachCameras::UP, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_NW_CURSOR );
		}
		else if ( event.coord().y() > height() - 10 )
		{
			pCameras_->scroll( MachCameras::DOWN, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_SW_CURSOR );
		}
		else
		{
			pInGameScreen_->cursor2d( MachGui::SCROLL_W_CURSOR );
		}
	}
	// Scroll RIGHT
	else if ( pos_ == Right )
	{
		pCameras_->scroll( MachCameras::RIGHT, event );

		if ( event.coord().y() < 10 )
		{
			pCameras_->scroll( MachCameras::UP, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_NE_CURSOR );
		}
		else if ( event.coord().y() > height() - 10 )
		{
			pCameras_->scroll( MachCameras::DOWN, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_SE_CURSOR );
		}
		else
		{
			pInGameScreen_->cursor2d( MachGui::SCROLL_E_CURSOR );
		}
	}
	// Scroll UP
	else if ( pos_ == Top )
	{
		pCameras_->scroll( MachCameras::UP, event );

		if ( event.coord().x() < 10 )
		{
			pCameras_->scroll( MachCameras::LEFT, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_NW_CURSOR );
		}
		else if ( event.coord().x() > width() - 10 )
		{
			pCameras_->scroll( MachCameras::RIGHT, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_NE_CURSOR );
		}
		else
		{
			pInGameScreen_->cursor2d( MachGui::SCROLL_N_CURSOR );
		}
	}
	// Scroll DOWN
	else if ( pos_ == Bottom )
	{
		pCameras_->scroll( MachCameras::DOWN, event );

		if ( event.coord().x() < 10 )
		{
			pCameras_->scroll( MachCameras::LEFT, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_SW_CURSOR );
		}
		else if ( event.coord().x() > width() - 10 )
		{
			pCameras_->scroll( MachCameras::RIGHT, event );
			pInGameScreen_->cursor2d( MachGui::SCROLL_SE_CURSOR );
		}
		else
		{
			pInGameScreen_->cursor2d( MachGui::SCROLL_S_CURSOR );
		}
	}
}

//virtual 
void MachGuiCameraScrollArea::doDisplay()
{}

//virtual 
void MachGuiCameraScrollArea::doHandleMouseExitEvent( const GuiMouseEvent& )
{
	pInGameScreen_->cursor2d( MachGui::MENU_CURSOR );
}


/* End CAMSCROL.CPP *************************************************/
