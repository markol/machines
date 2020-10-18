/*
 * C O R R A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

//#include "ctl/algorith.hpp"
#include <algorithm>
#include "gui/manager.hpp"
#include "gui/painter.hpp"

#include "machphys/objdata.hpp"

#include "machgui/corral.hpp"
#include "machgui/gui.hpp"
#include "machgui/guictrl.hpp"
#include "machgui/health.hpp"
#include "machgui/ingame.hpp"
#include "machgui/restorer.hpp"
#include "machgui/cameras.hpp"
#include "machgui/actbmpnm.hpp"
#include "machgui/controlp.hpp"
#include "machgui/machnav.hpp"

#include "machlog/apc.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/beacon.hpp"
#include "machlog/canattac.hpp"
#include "machlog/construc.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spy.hpp"
#include "machlog/technici.hpp"
#include "machlog/vmman.hpp"
#include "machlog/pod.hpp"
#include "machgui/internal/mgsndman.hpp"

SysPathName MachGuiCorralResource_hack( MachActor * pActor )
{
	MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
	size_t level = 0;

	if ( pActor->objectIsCanAttack() )
	{
		wc = pActor->asCanAttack().weaponCombo();

		// Special check for POD to see if the ion cannon is functional
		if ( pActor->objectType() == MachLog::POD and not pActor->asPod().hasIonCannon() )
		{
			wc = MachPhys::N_WEAPON_COMBOS;
		}
	}

	if ( pActor->objectIsMachine() )
	{
		level = pActor->asMachine().hwLevel();
	}
	else if ( pActor->objectIsConstruction() )
	{
		level = pActor->asConstruction().level();
	}

	SysPathName pathName = MachActorBitmaps::name( pActor->objectType(), pActor->subType(), level, wc, pActor->displayMapAndIconRace() );

	if ( pActor->objectType() == MachLog::SPY_LOCATOR and pActor->asSpyLocator().camouflaged() )
	{
		pathName.extension( "" );
		string strPathName = pathName.c_str();
		strPathName += "camo.bmp";
		pathName = strPathName;
	}

	return pathName;
}

//////////////////////////////////////////////////////////////////////

MachGuiCorralResource::MachGuiCorralResource( 	GuiDisplayable *pParent,
												const Gui::Coord& rel,
												MachActor* pActor,
												MachInGameScreen* pInGameScreen )
: 	pActor_( pActor ),
  	pHealthBar_( NULL ),
  	isObservingActor_( false ),
	pInGameScreen_( pInGameScreen ),
	MachGuiNewCorralIcon( 	pParent,
							rel,
							Gui::bitmap( MachGuiCorralResource_hack( pActor ) ) )
{
	const MachPhysObjectData& objData = pActor->objectData();

	pActor_->attach( this );
	isObservingActor_ = true;

	MachPhys::ArmourUnits maxAp = objData.armour();
	MachPhys::HitPointUnits maxHp = objData.hitPoints();

	if( maxAp == 0 )
		++maxAp;
	if( maxHp == 0 )
		++maxHp;

	pHealthBar_ = _NEW( MachGuiHealthBar( this,
									Gui::Coord( 2, 2 ),
									width() - 4,
									maxHp, maxAp ) );

	updateHealthBar();
}

MachGuiCorralResource::~MachGuiCorralResource()
{
    if( isObservingActor_ )
    	pActor_->detach( this );
}

//////////////////////////////////////////////////////////////////////

void MachGuiCorralResource::updateHealthBar()
{
	HAL_STREAM("(" << pActor_->id() << ") MGCorralResource::updateHealthBar\n" );
	MachPhys::ArmourUnits ap = pActor_->armour();
	MachPhys::HitPointUnits hp = pActor_->hp();

	// TBD: remove this check when MachPhysObjectData behaves reasonably
	if( ap > pHealthBar_->maxArmour() )
		ap = pHealthBar_->maxArmour();
    else if( ap < 0 )
        ap = 0;

	// TBD: remove this check when MachPhysObjectData behaves reasonably
	if( hp > pHealthBar_->maxHits() )
		hp = pHealthBar_->maxHits();
    else if( hp < 0 )
        hp = 0;

	pHealthBar_->hp( hp );
	pHealthBar_->armour( ap );

	changed();
}

//////////////////////////////////////////////////////////////////////

// virtual
bool MachGuiCorralResource::beNotified( W4dSubject* pSubject,
										W4dSubject::NotificationEvent event,
										int clientData )
{
	PRE( pSubject == pActor_ );
    switch( event )
    {
		case W4dSubject::DELETED :
			isObservingActor_ = false;
			isVisible( false );
		break;
		case W4dSubject::CLIENT_SPECIFIC :
		{
			HAL_STREAM("MGCorralRespource::beNotified with HEALTH_STATUIS_CHANGED\n" );
			if( clientData == MachLog::HEALTH_STATUS_CHANGED or clientData == MachLog::PERCENTAGE_COMPLETE_CHANGED )
				updateHealthBar();
		} break;
    }

	return isObservingActor_;
}

// virtual
void MachGuiCorralResource::domainDeleted( W4dDomain * )
{
    // Intentionally Empty
}

void MachGuiCorralResource::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	pHealthBar_->depress( true );
}

void MachGuiCorralResource::doBeReleased( const GuiMouseEvent& event )
{
	pHealthBar_->depress( false );

	if ( event.isAltPressed() )
	{
		const MachActor* pActor = pActor_;

		// If inside APC look at APC instead
		if ( pActor_->objectIsMachine() and pActor_->asMachine().insideAPC() )
		{
			pActor = &(pActor_->asMachine().APCImInside());
		}

		//Switch camera to look at actor
		pInGameScreen_->cameras()->lookAt( *pActor );
	}
	else if ( event.isCtrlPressed() ) 	// Remove this icon from the corral
	{
		if ( pInGameScreen_->isSelected( *pActor_ ) )
		{
			pInGameScreen_->deselect( pActor_ );
		}
	}
	else // Remove everything but this from the corral
	{
		MachActor *pActor = pActor_; // Keep copy because this is about to be deleted
		MachInGameScreen* pInGameScreen = pInGameScreen_;
		pInGameScreen->deselectAll();
		pInGameScreen->select( pActor );
	}
}

//virtual
void MachGuiCorralResource::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
	pHealthBar_->depress( false );

    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	MachGuiNewCorralIcon::doHandleMouseExitEvent( mouseEvent );
}

//virtual
void MachGuiCorralResource::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiNewCorralIcon::doHandleMouseEnterEvent( mouseEvent );

    pInGameScreen_->displayActorPromptText( pActor_ );
}

//virtual
void MachGuiCorralResource::doHandleContainsMouseEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiNewCorralIcon::doHandleContainsMouseEvent( mouseEvent );

    pInGameScreen_->displayActorPromptText( pActor_ );
}

//static
size_t MachGuiCorralResource::buttonWidth()
{
	return 42; // TODO : Remove hard coded value
}

//static
size_t MachGuiCorralResource::buttonHeight()
{
	// TODO : Remove hard coded values
	return MachGuiHealthBar::healthBarHeight() + 38 /* Bitmap height */ + 4 /* Border */;
}

//virtual
void MachGuiCorralResource::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	MachGuiNewCorralIcon::doDisplayInteriorEnabled( abs );

	if ( pActor_->objectIsMachine() and pActor_->asMachine().insideAPC() )
	{
		Gui::Coord absCopy( abs );
		absCopy.y( absCopy.y() + MachGuiHealthBar::healthBarHeight() );

		// Grey out icon if inside APC
		GuiPainter::instance().blit( MachGui::inTransitBmp(), absCopy );
	}
}

//////////////////////////////////////////////////////////////////////

MachCorralIcons::MachCorralIcons( MachGuiCorral * pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
: GuiSimpleScrollableList(	pParent,
							Gui::Box( Gui::Coord( rel.x(), rel.y() ), MachCorralIcons::reqWidth(), MachCorralIcons::reqHeight(pInGameScreen) ),
							MachGuiCorralResource::buttonWidth(),
							MachGuiCorralResource::buttonHeight(),
							MachCorralIcons::columns() ),
  pCorral_( pParent ),
  pInGameScreen_( pInGameScreen )
{
	// Intentionally Empty
}


MachCorralIcons::~MachCorralIcons()
{
	// Intentionally Empty
}

void MachCorralIcons::change()
{
	deleteAllChildren();
	MachGuiCorral::Actors::iterator i = pCorral_->actors_.begin();
	for( size_t j=0; i != pCorral_->actors_.end(); ++i, ++j )
		_NEW( MachGuiCorralResource( this, Gui::Coord( 0, 0 ), *i, pInGameScreen_ ) );

	childrenUpdated();
}

void MachCorralIcons::add( MachActor* pActor )
{
	_NEW( MachGuiCorralResource( this, Gui::Coord( 0, 0 ), pActor, pInGameScreen_ ) );

	// note that "selected" voicemail is only given in the case of the first actor put into the corral
	if( pActor->objectIsMachine() and pActor->race() == MachLogRaces::instance().pcController().race() and pCorral_->actors_.size()==1 )
	{
		// give voicemail
		MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_SELECTED, pActor->id(),  pActor->race() );
		// need INSTANT acknowledgement in the case of actor selection
		MachLogVoiceMailManager::instance().update();
	}

	childAdded();
}

//////////////////////////////////////////////////////////////////////

void MachCorralIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );

	GuiSimpleScrollableList::doDisplay();
}

//////////////////////////////////////////////////////////////////////

MachGuiCorral::MachGuiCorral( GuiDisplayable* pParent, const Gui::Coord& coord, MachInGameScreen* pInGameScreen )
: MachGuiScrollArea( pParent, Gui::Boundary( coord, reqWidth(), reqHeight(pInGameScreen) ), pInGameScreen ),
  pInGameScreen_( pInGameScreen )
{}

MachGuiCorral::~MachGuiCorral()
{
	// Intentionally Empty
}


//static
size_t MachCorralIcons::reqWidth()
{
	return ( columns() * MachGuiCorralResource::buttonWidth() );
}

//static
size_t MachCorralIcons::reqHeight( MachInGameScreen* pInGameScreen )
{
	// Find height available in control panel
	int height = pInGameScreen->controlPanel().getVisibleHeight() - MachGuiNavigatorBase::reqHeight() - 2;

	// Make height a multiple of MachGuiCorralResource::buttonHeight
	height -= height % MachGuiCorralResource::buttonHeight();

	return height;
}

MachPhys::Race MachGuiCorral::race() const
{
	return MachLogRaces::instance().pcController().race();
}

MachGuiCorral::Actors& MachGuiCorral::actors()
{
	return actors_;
}

const MachGuiCorral::Actors& MachGuiCorral::actors() const
{
	return actors_;
}

void MachGuiCorral::eraseAllActors()
{
	if( not actors_.empty() )
		actors_.erase( actors_.begin(), actors_.end() );
}

void MachGuiCorral::clear()
{
	eraseAllActors();
	pIcons_->change();
	update();
}

bool MachGuiCorral::isSelected( MachActor* pActor ) const
{
    return find( actors_.begin(), actors_.end(), pActor ) != actors_.end();
}

void MachGuiCorral::add( MachActor* pActor )
{
    PRE( not isSelected( pActor ) );

    //Add to the collection
    actors_.push_back( pActor );

    //Ensure updated
    pIcons_->add( pActor );
	update();
}

void MachGuiCorral::add( const ctl_pvector<MachActor>& actors )
{
	for ( ctl_pvector<MachActor>::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		MachActor *pActor = (*iter);

    	ASSERT( not isSelected( pActor ), "actor already selected" );

	    //Add to the collection
    	actors_.push_back( pActor );

    	//Ensure updated
    	pIcons_->add( pActor );
	}

	update();
}

void MachGuiCorral::remove( MachActor* pActor )
{
    PRE( isSelected( pActor ) );

    //Remove from the collection
    Actors::iterator it = find( actors_.begin(), actors_.end(), pActor );
    ASSERT( it != actors_.end(), "" );

    actors_.erase( it );

    //Ensure updated
    pIcons_->change();
	update();
}

void MachGuiCorral::remove( const ctl_pvector<MachActor>& actors )
{
	for ( ctl_pvector<MachActor>::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		MachActor *pActor = (*iter);
	    ASSERT( isSelected( pActor ), "actor not selected" );

	    //Remove from the collection
	    Actors::iterator it = find( actors_.begin(), actors_.end(), pActor );
	    ASSERT( it != actors_.end(), "" );

    	actors_.erase( it );
	}

    //Ensure updated
    pIcons_->change();
	update();
}

//static
size_t MachGuiCorral::reqWidth()
{
	return MachGuiScrollBar::reqWidth() + MachCorralIcons::reqWidth() + MachGuiScrollBar::reqWidth();
}

//static
size_t MachGuiCorral::reqHeight( MachInGameScreen* pInGameScreen )
{
	return MachCorralIcons::reqHeight( pInGameScreen ) + 2;
}

//static
size_t MachCorralIcons::columns()
{
	return 3;
}

//virtual
GuiSimpleScrollableList* MachGuiCorral::createList( GuiDisplayable*, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
{
	pIcons_ = _NEW( MachCorralIcons( this, rel, pInGameScreen ) );
	return pIcons_;
}


MachGuiNewCorralIcon::MachGuiNewCorralIcon(	GuiDisplayable *pParent,
											const Gui::Coord& rel,
											const GuiBitmap& bitmap )
: GuiButtonWithFilledBorder(	pParent,
								MachGuiNewCorralIcon::exteriorRelativeBoundary( bitmap, GuiBorderMetrics( 1, 1, 1 ), rel ),
								GuiBorderMetrics( 1, 1, 1 ),
								GuiFilledBorderColours( Gui::BLACK(),
														MachGui::OFFWHITE(),
														MachGui::ALMOSTBLACK(),
														Gui::RED() ),
								Gui::Coord( 1, 1 ) ),
  bitmap_( bitmap )
{
	// Intentionally Empty
}

MachGuiNewCorralIcon::~MachGuiNewCorralIcon()
{
	// Intentionally Empty
}

void MachGuiNewCorralIcon::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	Gui::Coord absCopy( abs );
	absCopy.y( absCopy.y() + MachGuiHealthBar::healthBarHeight() );

	GuiPainter::instance().blit( bitmap_, absCopy );
}

//////////////////////////////////////////////////////////////////////

// static
Gui::Box
MachGuiNewCorralIcon::exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel )
{
	return Gui::Box( rel, m.totalHorizontalThickness() + bitmap.width(),
						m.totalVerticalThickness() + bitmap.height() + MachGuiHealthBar::healthBarHeight() );
}

GuiBitmap& MachGuiNewCorralIcon::corralBitmap()
{
	return bitmap_;
}

//////////////////////////////////////////////////////////////////////

/* End CORRAL.CPP ***************************************************/
