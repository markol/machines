/*
 * C O N T R O L P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/controlp.hpp"
#include "machgui/internal/controli.hpp"
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "gui/painter.hpp"
#include "gui/button.hpp"
#include "machgui/gui.hpp"
#include "machgui/machnav.hpp"
#include "machgui/ingame.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"

///////////////////////////////////////////////////////////////////////////////

class MachGuiControlPanelOnOffBtn : public GuiButton
// cannonical from revoked
{
public:
	MachGuiControlPanelOnOffBtn( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
	:	GuiButton(pParent, Gui::Box( rel, 6, 49 ) ),
		pInGameScreen_( pInGameScreen ),
		slideHoleBmp_( Gui::bitmap( "gui/misc/slidhole.bmp" ) ),
		sliderBmp_( Gui::bitmap( "gui/misc/slider.bmp" ) )
	{}

protected:
	virtual void doBeReleased( const GuiMouseEvent& )
	{
		pInGameScreen_->controlPanelOn( not pInGameScreen_->controlPanelOn() );
	}

	virtual void doBeDepressed( const GuiMouseEvent& )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	}

	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel )
	{
		GuiButton::doHandleMouseExitEvent( rel );

	    //Clear the cursor prompt string
	    pInGameScreen_->clearCursorPromptText();
	}

	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel )
	{
		GuiButton::doHandleContainsMouseEvent( rel );

		if ( pInGameScreen_->controlPanelOn() )
		{
			//Load the resource string
			GuiResourceString prompt( IDS_CONTROLPANELOFF );

		    //Set the cursor prompt
			pInGameScreen_->cursorPromptText( prompt.asString() );
		}
		else
		{
			//Load the resource string
			GuiResourceString prompt( IDS_CONTROLPANELON );

		    //Set the cursor prompt
			pInGameScreen_->cursorPromptText( prompt.asString() );
		}
	}

	virtual void doDisplayDepressedEnabled()
	{
		GuiPainter::instance().blit( slideHoleBmp_, absoluteBoundary().minCorner() );
		GuiPainter::instance().blit( sliderBmp_, Gui::Coord( absoluteBoundary().minCorner().x(), absoluteBoundary().minCorner().y() + 12 ) );
	}

	virtual void doDisplayReleasedEnabled()
	{
		GuiPainter::instance().blit( slideHoleBmp_, absoluteBoundary().minCorner() );

		if ( pInGameScreen_->controlPanelOn() )
			GuiPainter::instance().blit( sliderBmp_, absoluteBoundary().minCorner() );
		else
			GuiPainter::instance().blit( sliderBmp_, Gui::Coord( absoluteBoundary().minCorner().x(), absoluteBoundary().minCorner().y() + 24 ) );
	}

private:
	MachGuiControlPanelOnOffBtn( const MachGuiControlPanelOnOffBtn& );
	MachGuiControlPanelOnOffBtn& operator =( const MachGuiControlPanelOnOffBtn& );
	bool operator ==( const MachGuiControlPanelOnOffBtn& ) const;

	// Data members...
	MachInGameScreen* pInGameScreen_;
	GuiBitmap slideHoleBmp_;
	GuiBitmap sliderBmp_;
};

MachGuiControlPanel::MachGuiControlPanel( GuiDisplayable* pParent, const Gui::Box& box, MachInGameScreen* pInGameScreen )
:	GuiDisplayable( pParent, box, GuiDisplayable::LAYER3 ),
	pImpl_( _NEW( MachGuiControlPanelImpl() ) )
{
	CB_DEPIMPL_ARRAY( GuiBitmap, decals_ );
	CB_DEPIMPL( MachGuiControlPanelImpl::RedrawAreas, redrawAreas_ );

    redrawAreas_.reserve( 64 );

	setupDecalBitmaps();
	setupDecalCoords();

	_NEW( MachGuiControlPanelOnOffBtn( this, Gui::Coord( MachGui::controlPanelOutXPos() - 6, 8 ), pInGameScreen ) );

	useFastSecondDisplay( false );

    TEST_INVARIANT;
}

MachGuiControlPanel::~MachGuiControlPanel()
{
    TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachGuiControlPanel::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiControlPanel& t )
{

    o << "MachGuiControlPanel " << (void*)&t << " start" << std::endl;
    o << "MachGuiControlPanel " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiControlPanel::doDisplay()
{
	redrawAreaImmediate( Gui::Box( 0,0,width(),height() ) );
}

void MachGuiControlPanel::redrawArea( const GuiDisplayable& displayable )
{
	redrawArea( displayable.relativeBoundary( *this ) );
}

void MachGuiControlPanel::redrawArea( const Gui::Box& area )
{
	NEIL_STREAM( "***MachGuiControlPanel::redrawArea() " << area << std::endl );

	CB_DEPIMPL( MachGuiControlPanelImpl::RedrawAreas, redrawAreas_ );

	MachGuiControlPanelImpl::RedrawArea* pNewRedrawArea = _NEW( MachGuiControlPanelImpl::RedrawArea() );
	pNewRedrawArea->area_ = area;

	if ( GuiDisplayable::useFourTimesRender() )
	{
		pNewRedrawArea->count_ = 4;
	}
	else
	{
		pNewRedrawArea->count_ = 2;
	}

	redrawAreas_.push_back( pNewRedrawArea );
}

void MachGuiControlPanel::redrawAreaImmediate( const GuiDisplayable& displayable )
{
	redrawAreaImmediate( displayable.relativeBoundary( *this ) );
}

void MachGuiControlPanel::redrawAreaImmediate( const Gui::Box& area )
{
	CB_DEPIMPL_ARRAY( GuiBitmap, decals_ );
	CB_DEPIMPL_ARRAY( Gui::Coord, decalCoord_ );

	int y = 3;
	int maxY = getVisibleHeight();
	int yStep = MachGui::controlPanelBmp().height();

	int areaYMax = area.maxCorner().y();
	int areaXMax = area.maxCorner().x();
	int areaYMin = area.minCorner().y();
	int areaXMin = area.minCorner().x();

	//DEBUG_STREAM( DIAG_NEIL, "*** redraw area " << areaXMin << "," << areaYMin << "," << areaXMax << "," << areaYMax << std::endl );

	while ( y < maxY and y < areaYMax )
	{
		if ( areaYMin <= y + yStep )
		{
			Gui::Box drawArea( 	std::max( 0, areaXMin ),
								std::max( y, areaYMin ),
								std::min( (int)MachGui::controlPanelBmp().width(), areaXMax ),
								std::min( y + yStep, areaYMax ) );

			int drawAreaYMax = drawArea.maxCorner().y();
			int drawAreaXMax = drawArea.maxCorner().x();
			int drawAreaYMin = drawArea.minCorner().y();
			int drawAreaXMin = drawArea.minCorner().x();

			//DEBUG_STREAM( DIAG_NEIL, "     drawArea " << drawAreaXMin << "," << drawAreaYMin << "," << drawAreaXMax << "," << drawAreaYMax << std::endl );

		   	Gui::Coord screenCoords( drawAreaXMin + absoluteBoundary().minCorner().x(), drawAreaYMin + absoluteBoundary().minCorner().y() );

	   		GuiPainter::instance().blit( 	MachGui::controlPanelBmp(),
											Gui::Box( 	Gui::Coord(	drawAreaXMin, drawAreaYMin - y ),
														drawAreaXMax - drawAreaXMin,/* width */
														drawAreaYMax - drawAreaYMin /* height*/ ),
											screenCoords );

			//DEBUG_STREAM( DIAG_NEIL, "     blitFrom " << drawAreaXMin << "," <<  drawAreaYMin - y << "," << drawAreaXMax - drawAreaXMin << "," << drawAreaYMax - drawAreaYMin << std::endl );
	   	}

		y += MachGui::controlPanelBmp().height();
	}

	// Render decals
	for ( int loop = 0; loop < 8; ++loop )
	{
		// Move decal co-ord if screen has been scrolled to the left/right.
		Gui::Coord screenDecalCoord( decalCoord_[loop].x(), decalCoord_[loop].y() );
		Gui::Box decalBox( screenDecalCoord, decals_[loop].width(), decals_[loop].height() );

		if ( decalBox.intersects( area ) )
		{
			Gui::Box decalIntersect = decalBox;
			decalIntersect.intersectWith( area );

			GuiPainter::instance().blit( decals_[loop],
										 Gui::Box( Gui::Coord( 	decalIntersect.minCorner().x() - decalBox.minCorner().x(),
										 						decalIntersect.minCorner().y() - decalBox.minCorner().y() ),
										 		   decalIntersect.maxCorner().x() - decalIntersect.minCorner().x(),  // width
										 		   decalIntersect.maxCorner().y() - decalIntersect.minCorner().y() ),// height
										 Gui::Coord( decalIntersect.minCorner().x() + absoluteBoundary().minCorner().x(),
										 			 decalIntersect.minCorner().y() + absoluteBoundary().minCorner().y() ) );
		}
	}

	// TEMP
	//hollowRectangle( area, Gui::BLACK(), 1 );
}

void MachGuiControlPanel::redrawAllAreas()
{
	CB_DEPIMPL( MachGuiControlPanelImpl::RedrawAreas, redrawAreas_ );

	size_t n = redrawAreas_.size();
	for( size_t i = n; i-- != 0; )
    {
        MachGuiControlPanelImpl::RedrawArea* pRedrawArea = redrawAreas_[i];

		NEIL_STREAM( "***MachGuiControlPanel::redrawAllAreas() " << pRedrawArea->area_ << std::endl );

		redrawAreaImmediate( pRedrawArea->area_ );
		--(pRedrawArea->count_);

		// Clean up any redraw areas that have been used twice
		if ( pRedrawArea->count_ == 0 )
		{
			_DELETE( pRedrawArea );
			redrawAreas_.erase( redrawAreas_.begin() + i );
		}
	}
}

int MachGuiControlPanel::getVisibleHeight() const
{
	int screenHeight = W4dManager::instance().sceneManager()->pDevice()->windowHeight();
	int controlPanelY = absoluteBoundary().minCorner().y();

	return screenHeight - controlPanelY;
}

void MachGuiControlPanel::setupDecalCoords()
{
	CB_DEPIMPL_ARRAY( Gui::Coord, decalCoord_ );
	CB_DEPIMPL_ARRAY( GuiBitmap, decals_ );

	decalCoord_[0] = Gui::Coord( 0,  78 );
	decalCoord_[1] = Gui::Coord( 1,  175 );
	decalCoord_[2] = Gui::Coord( 127,60 );
	decalCoord_[3] = Gui::Coord( 79, 155 );
	decalCoord_[4] = Gui::Coord( 0,  0 );
	decalCoord_[5] = Gui::Coord( 60, getVisibleHeight() - ( decals_[5].height() + 5 ) );
	decalCoord_[6] = Gui::Coord( 5,  getVisibleHeight() - ( decals_[6].height() + 3 ) );
	decalCoord_[7] = Gui::Coord( 136,getVisibleHeight() - ( decals_[7].height() + 3 ) );
}

void MachGuiControlPanel::repositionNavigators( GuiDisplayable* pMachineNav, GuiDisplayable* pConstructionNav, GuiDisplayable* pSquadNav )
{
	Gui::Coord navTopLeft( 1, getVisibleHeight() - MachGuiNavigatorBase::reqHeight() );

	positionChildRelative( pMachineNav, navTopLeft );
	positionChildRelative( pConstructionNav, navTopLeft );
	positionChildRelative( pSquadNav, navTopLeft );
}

string MachGuiControlPanel::getDecalRootDirectory() const
{
	string retValue = "gui/misc/red/";

	if ( MachLogRaces::instance().hasPCRace() )
	{
		switch ( MachLogRaces::instance().pcController().race() )
		{
			case MachPhys::RED:
				break;
			case MachPhys::GREEN:
				retValue = "gui/misc/green/";
				break;
			case MachPhys::BLUE:
				retValue = "gui/misc/blue/";
				break;
			case MachPhys::YELLOW:
				retValue = "gui/misc/yellow/";
				break;
			DEFAULT_ASSERT_BAD_CASE( MachLogRaces::instance().pcController().race() );
		}
	}

	return retValue;
}

void MachGuiControlPanel::setupDecalBitmaps()
{
	CB_DEPIMPL_ARRAY( GuiBitmap, decals_ );

	// Setup decal bitmaps
	decals_[0] = Gui::bitmap( getDecalRootDirectory() + "decal1.bmp" );
	decals_[1] = Gui::bitmap( getDecalRootDirectory() + "decal2.bmp" );
	decals_[2] = Gui::bitmap( getDecalRootDirectory() + "decal3.bmp" );
	decals_[3] = Gui::bitmap( getDecalRootDirectory() + "decal4.bmp" );
	decals_[4] = Gui::bitmap( "gui/misc/decal5.bmp" );
	decals_[5] = Gui::bitmap( "gui/misc/decal6.bmp" );
	decals_[6] = Gui::bitmap( "gui/misc/decal7.bmp" );
	decals_[7] = Gui::bitmap( "gui/misc/decal7.bmp" );

	for ( int loop = 0; loop < 8; ++loop )
	{
		decals_[loop].enableColourKeying();
	}
}

/* End CONTROLP.CPP *************************************************/
