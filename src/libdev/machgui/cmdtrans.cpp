/*
 * C M D T R A N S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdtrans.hpp"

#include "ctl/algorith.hpp"

#include "device/butevent.hpp"

#include "machlog/actor.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mine.hpp"
#include "machlog/move.hpp"
#include "machlog/optransp.hpp"
#include "machlog/oppickup.hpp"
#include "machlog/races.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"

#include "machgui/ingame.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"


MachGuiTransportCommand::MachGuiTransportCommand( MachInGameScreen* pInGameScreen, bool explicitOrder )
:   MachGuiCommand( pInGameScreen ),
    pSmeltingBuilding_( NULL ),
	explicitOrder_( explicitOrder ),
    hadFinalPick_( false )
{
	suppliers_.reserve( 10 );
    TEST_INVARIANT;
}

MachGuiTransportCommand::~MachGuiTransportCommand()
{
	clearAndDetachFromAllSuppliers();
    TEST_INVARIANT;

}

void MachGuiTransportCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiTransportCommand& t )
{

    o << "MachGuiTransportCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiTransportCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiTransportCommand::pickOnTerrain( const MexPoint3d& , bool, bool, bool )
{

}

//virtual
void MachGuiTransportCommand::pickOnActor
(
    MachActor* pActor, bool, bool shiftPressed, bool
)
{
	bool myRace = ( MachLogRaces::instance().pcController().race() == pActor->race() );

    //Check for a building or machine
    if( ( pActor->objectType() == MachLog::MINE and pActor->asMine().worthVisiting() )
		or ( pActor->objectIsCanSmelt() and not myRace ) )
    {
		MachLogConstruction* pCandidateConstruction = &pActor->asConstruction();

		if( not supplierIsDuplicate( pCandidateConstruction ) )
		{
			if( not shiftPressed )
				clearAndDetachFromAllSuppliers();

			// Add to list of suppliers
        	suppliers_.push_back( pCandidateConstruction );

        	pCandidateConstruction->attach( this );
		}
    }

    if( pActor->objectIsCanSmelt() and myRace )
    {
		pSmeltingBuilding_ = &pActor->asConstruction();
       	hadFinalPick_ = true;
    }
	else
	{
		// Waypoint click (i.e. not final click)
		MachGuiSoundManager::instance().playSound( "gui/sounds/waypoint.wav" );
	}
}

//virtual
bool MachGuiTransportCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Administrators and aggressors can Transport
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::RESOURCE_CARRIER and actor.asResourceCarrier().isNormalResourceCarrier();
}

//virtual
bool MachGuiTransportCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiTransportCommand::doApply( MachActor* pActor, string* )
{
    PRE( pActor->objectIsMachine() );
	PRE( pActor->objectType() == MachLog::RESOURCE_CARRIER and pActor->asResourceCarrier().isNormalResourceCarrier() );

	if( not hasPlayedVoiceMail() )
	{
		// if we're already transporting, we're basically just changing implementation details, so give a "tasked" e-mail
		// instead( "OK." "Alright!" etc.

		if( pActor->asResourceCarrier().isTransporting() )
			MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_TASKED, pActor->id(),  pActor->race() );
		else
			MachLogVoiceMailManager::instance().postNewMail( VID_RESOURCE_CARRIER_TRANSPORTING, pActor->id(), pActor->race() );

		hasPlayedVoiceMail( true );
	}

	if( not suppliers_.empty() )
	{
		pActor->asResourceCarrier().setSuppliers( suppliers_ );
	}
	else
	{
		// if we're ONLY specifying a destination here, and this is an explicit order, then wipe out
		// any supplier list currently held in the resource carrier
		if( explicitOrder_ )
			pActor->asResourceCarrier().clearSuppliers();
	}

	ASSERT( pSmeltingBuilding_ , "Unexpected NULL for pSmeltingBuilding_" );

	pActor->asResourceCarrier().setSmeltingBuilding( pSmeltingBuilding_ );

    return true;
}

//virtual
MachGui::Cursor2dType MachGuiTransportCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::MENU_CURSOR;

    if( isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES  ) )
        cursor = MachGui::MOVETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiTransportCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    bool myRace = ( MachLogRaces::instance().pcController().race() == pActor->race() );

	MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    MachLog::ObjectType objectType = pActor->objectType();

    if( ( objectType == MachLog::MINE and pActor->asMine().worthVisiting() )
		or ( objectType == MachLog::SMELTER and not myRace) )
	{
		cursor = MachGui::PICKUP_CURSOR;
	}
    else if( ( objectType == MachLog::SMELTER or objectType == MachLog::POD ) and myRace )
        cursor = MachGui::TRANSPORT_CURSOR;

    return cursor;
}

//virtual
void MachGuiTransportCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiTransportCommand::clone() const
{
    return _NEW( MachGuiTransportCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiTransportCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/transprt.bmp", "gui/commands/transprt.bmp" );
    return names;
}

//virtual
uint MachGuiTransportCommand::cursorPromptStringId() const
{
    return IDS_TRANSPORT_COMMAND;
}

//virtual
uint MachGuiTransportCommand::commandPromptStringid() const
{
    return IDS_TRANSPORT_START;
}

//virtual
bool MachGuiTransportCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_T and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

bool MachGuiTransportCommand::supplierIsDuplicate( const MachLogConstruction* pCandidateConstruction ) const
{
	bool result = false;

	ctl_pvector< MachLogConstruction >::const_iterator i = find( suppliers_.begin(), suppliers_.end(), pCandidateConstruction );

	if( i != suppliers_.end() )
		result = true;

	return result;
}

void MachGuiTransportCommand::clearAndDetachFromAllSuppliers()
{
	while( not suppliers_.empty() )
	{
		suppliers_.back()->detach( this );
		suppliers_.pop_back();
	}
}

//virtual
bool MachGuiTransportCommand::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
	{
		ctl_pvector< MachLogConstruction >::iterator i = find( suppliers_.begin(), suppliers_.end(), pSubject );
		if( i != suppliers_.end() )
		{
			// one of our constructions has been destroyed
			stayAttached = false;
			suppliers_.erase( i );
		}
	}
	break;

	default:
		;
	}

	return stayAttached;
}


//virtual
void MachGuiTransportCommand::domainDeleted( W4dDomain*  )
{
	//intentionally empty...override as necessary
}

/* End CMDATTAC.CPP **************************************************/
