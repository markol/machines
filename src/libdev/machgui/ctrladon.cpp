/*
 * C T R L A D O N . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctrladon.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/cameras.hpp"
#include "gui/painter.hpp"
#include "gui/gui.hpp"
#include "gui/icon.hpp"
#include "gui/restring.hpp"
#include "system/pathname.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"

///////////////////////////////////////////////////////////////////////////////

class MachGuiCameraToggleBtn : public GuiBitmapButtonWithFilledBorder
// cannonical from revoked
{
public:
	MachGuiCameraToggleBtn( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
	:  	GuiBitmapButtonWithFilledBorder(pParent, 
										rel, 
										GuiBorderMetrics(1,1,1), 
										GuiFilledBorderColours( Gui::Colour(144.0/255.0,148.0/255.0,160.0/255.0), 
																Gui::Colour(232.0/255.0,232.0/255.0,232.0/255.0), 
															   	Gui::Colour(62.0/255.0,62.0/255.0,62.0/255.0),
															    Gui::RED() ),
										Gui::bitmap( SysPathName( "gui/misc/zenith.bmp" ) ), 
										Gui::Coord(1,1) ),
		pInGameScreen_( pInGameScreen )
	{}

protected:
	// inherited from GuiIcon...
	virtual void doBeReleased( const GuiMouseEvent& )
	{
		if ( pInGameScreen_->cameras()->isZenithCameraActive() )
			pInGameScreen_->cameras()->useGroundCamera();
		else
			pInGameScreen_->cameras()->useZenithCamera();
	}

	virtual void doBeDepressed( const GuiMouseEvent& )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	}

	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel )
	{
		GuiBitmapButtonWithFilledBorder::doHandleMouseEnterEvent( rel ); 

		usingZenithCamera_ = pInGameScreen_->cameras()->isZenithCameraActive();
		//Load the resource string
		GuiResourceString prompt( usingZenithCamera_ ? IDS_GROUND_CAMERA : IDS_ZENITH_CAMERA );

	    //Set the cursor prompt
	    pInGameScreen_->cursorPromptText( prompt.asString() );
	}

	virtual const GuiBitmap& getBitmap() const
	{
		static GuiBitmap zenithBmp = Gui::bitmap( "gui/misc/zenith.bmp" );
		static GuiBitmap groundBmp = Gui::bitmap( "gui/misc/ground.bmp" );
		
		if ( pInGameScreen_->cameras()->isZenithCameraActive() )
			return groundBmp;

		return zenithBmp;
	}

	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel )
	{
		GuiBitmapButtonWithFilledBorder::doHandleMouseExitEvent( rel ); 

	    //Clear the cursor prompt string
	    pInGameScreen_->clearCursorPromptText();
	}

	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& )
	{
		// Change prompt text if keyboard has been used to switch cameras.
		if ( usingZenithCamera_	!= pInGameScreen_->cameras()->isZenithCameraActive() )
		{
			usingZenithCamera_ = not usingZenithCamera_;
	
			//Load the resource string
			GuiResourceString prompt( usingZenithCamera_ ? IDS_GROUND_CAMERA : IDS_ZENITH_CAMERA );

		    //Set the cursor prompt
		    pInGameScreen_->cursorPromptText( prompt.asString() );
		}
	}

private:
	MachGuiCameraToggleBtn( const MachGuiCameraToggleBtn& );
	MachGuiCameraToggleBtn& operator =( const MachGuiCameraToggleBtn& );
	bool operator ==( const MachGuiCameraToggleBtn& ) const;

	// Data members...
	MachInGameScreen* pInGameScreen_;
	bool usingZenithCamera_;
};

///////////////////////////////////////////////////////////////////////////////

class MachGuiReturnToMenuBtn : public GuiButton
// cannonical from revoked
{
public:
	MachGuiReturnToMenuBtn( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
	:	GuiButton(pParent, Gui::Box( rel, 20, 20 ) ),
		pInGameScreen_( pInGameScreen ),
		return1Bmp_( Gui::bitmap( "gui/misc/return1.bmp" ) ),
		return2Bmp_( Gui::bitmap( "gui/misc/return2.bmp" ) )
	{}

protected:
	// inherited from GuiIcon...
	virtual void doBeReleased( const GuiMouseEvent& )
	{
		pInGameScreen_->switchToMenus( true );
	}

	virtual void doBeDepressed( const GuiMouseEvent& )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	}

	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel )
	{
		GuiButton::doHandleMouseEnterEvent( rel );

		//Load the resource string
		GuiResourceString prompt( IDS_RETURNTOMENUS );

	    //Set the cursor prompt
	    pInGameScreen_->cursorPromptText( prompt.asString() );
	}

	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel )
	{
		GuiButton::doHandleMouseExitEvent( rel ); 

	    //Clear the cursor prompt string
	    pInGameScreen_->clearCursorPromptText();
	}

	virtual void doDisplayDepressedEnabled()
	{
		GuiPainter::instance().blit( return2Bmp_, absoluteBoundary().minCorner() );
	}

	virtual void doDisplayReleasedEnabled()
	{
		GuiPainter::instance().blit(return1Bmp_, absoluteBoundary().minCorner() );
	}

private:
	MachGuiReturnToMenuBtn( const MachGuiReturnToMenuBtn& );
	MachGuiReturnToMenuBtn& operator =( const MachGuiReturnToMenuBtn& );
	bool operator ==( const MachGuiReturnToMenuBtn& ) const;

	// Data members...
	MachInGameScreen* pInGameScreen_;
	GuiBitmap return1Bmp_;
	GuiBitmap return2Bmp_;
};

/*
class MachGuiReturnToMenuBtn : public GuiBitmapButtonWithFilledBorder
// cannonical from revoked
{
public:
	MachGuiReturnToMenuBtn( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
	:	GuiBitmapButtonWithFilledBorder(pParent, 
										rel, 
										GuiBorderMetrics(1,1,1), 
										GuiFilledBorderColours( Gui::Colour(96.0/255.0,108.0/255.0,104.0/255.0), 
																Gui::Colour(232.0/255.0,232.0/255.0,232.0/255.0), 
															   	Gui::Colour(62.0/255.0,62.0/255.0,62.0/255.0),
															    Gui::RED() ),
										Gui::bitmap( SysPathName( "gui/misc/return.bmp" ) ), 
										Gui::Coord(1,1) ),
		pInGameScreen_( pInGameScreen )
	{}

protected:
	// inherited from GuiIcon...
	virtual void doBeReleased( const GuiMouseEvent& )
	{
		pInGameScreen_->switchToMenus( true );
	}

	virtual void doBeDepressed( const GuiMouseEvent& )
	{}

	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& )
	{
		//Load the resource string
		GuiResourceString prompt( IDS_RETURNTOMENUS );

	    //Set the cursor prompt
	    pInGameScreen_->cursorPromptText( prompt.asString() );
	}

	virtual void doHandleMouseExitEvent( const GuiMouseEvent& )
	{
	    //Clear the cursor prompt string
	    pInGameScreen_->clearCursorPromptText();
	}

private:
	MachGuiReturnToMenuBtn( const MachGuiReturnToMenuBtn& );
	MachGuiReturnToMenuBtn& operator =( const MachGuiReturnToMenuBtn& );
	bool operator ==( const MachGuiReturnToMenuBtn& ) const;

	// Data members...
	MachInGameScreen* pInGameScreen_;
};
*/

class MachGuiControlPanelAddOnImpl
{
public:
	MachGuiControlPanelAddOnImpl();

	GuiBitmap backgroundBmp_;
	MachInGameScreen* pInGameScreen_;
};

MachGuiControlPanelAddOnImpl::MachGuiControlPanelAddOnImpl()
:	backgroundBmp_( Gui::bitmap( "gui/misc/camtab.bmp" ) )
{
	backgroundBmp_.enableColourKeying();
}

#define MachGuiControlPanelAddOnWidth 30
#define MachGuiControlPanelAddOnHeight 66

MachGuiControlPanelAddOn::MachGuiControlPanelAddOn( GuiDisplayable* pParent, const Gui::Coord& coord, MachInGameScreen* pInGameScreen )
:	GuiDisplayable( pParent, Gui::Box( coord, MachGuiControlPanelAddOnWidth, MachGuiControlPanelAddOnHeight ), GuiDisplayable::LAYER3 )
{
	pImpl_ = _NEW( MachGuiControlPanelAddOnImpl() );

	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );

	pInGameScreen_ = pInGameScreen;

	_NEW( MachGuiReturnToMenuBtn( this, Gui::Coord(4,0), pInGameScreen ) );
	_NEW( MachGuiCameraToggleBtn( this, Gui::Coord(4,22), pInGameScreen ) );

	redrawEveryFrame( true );

    TEST_INVARIANT;
}

MachGuiControlPanelAddOn::~MachGuiControlPanelAddOn()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

//virtual
void MachGuiControlPanelAddOn::doDisplay()
{
	CB_DEPIMPL( GuiBitmap, backgroundBmp_ );

	GuiPainter::instance().blit( backgroundBmp_, absoluteBoundary().minCorner() );
}

void MachGuiControlPanelAddOn::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiControlPanelAddOn& t )
{

    o << "MachGuiControlPanelAddOn " << (void*)&t << " start" << std::endl;
    o << "MachGuiControlPanelAddOn " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CTRLADON.CPP *************************************************/
