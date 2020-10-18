/*
 * C M D P I C K U . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdpicku.hpp"

#include "device/butevent.hpp"

#include "ctl/algorith.hpp"

#include "machlog/actor.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mine.hpp"
#include "machlog/move.hpp"
#include "machlog/oppickup.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"

#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"

MachGuiPickUpCommand::MachGuiPickUpCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false ),
	addingOrReplacing_( REPLACING ),
	action_( PICKUP )
{
	suppliers_.reserve( 10 );
    TEST_INVARIANT;
}

MachGuiPickUpCommand::~MachGuiPickUpCommand()
{
	while( not suppliers_.empty() )
	{
		suppliers_.back()->detach( this );
		suppliers_.pop_back();
	}

    TEST_INVARIANT;

}

void MachGuiPickUpCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiPickUpCommand& t )
{

    o << "MachGuiPickUpCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiPickUpCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiPickUpCommand::pickOnTerrain( const MexPoint3d& , bool, bool, bool )
{

}

//virtual
void MachGuiPickUpCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check for a legal actor
    if( cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed )
        != MachGui::INVALID_CURSOR )
    {
		ASSERT( pActor->objectIsConstruction(), "Attempting to do a pickup on a non-construction actor." );

		MachLogConstruction* pCandidateConstruction = &pActor->asConstruction();

		if( not constructionIsDuplicate( pCandidateConstruction ) )
		{
			// Add to list of suppliers
        	suppliers_.push_back( pCandidateConstruction );

        	pCandidateConstruction->attach( this );
		}

		if( shiftPressed )
	        addingOrReplacing_ = ADDING;

		hadFinalPick_ = true;
    }

}

// virtual
bool MachGuiPickUpCommand::isEligableForVisibility() const
{
	return false;
}

//virtual
bool MachGuiPickUpCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Administrators and aggressors can PickUp
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::RESOURCE_CARRIER and actor.asResourceCarrier().isNormalResourceCarrier();
}

//virtual
bool MachGuiPickUpCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiPickUpCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsMachine() );
	PRE( pActor->objectType() == MachLog::RESOURCE_CARRIER and pActor->asResourceCarrier().isNormalResourceCarrier() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case PICKUP:
            canDo = applyPickUpObject( pActor, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiPickUpCommand::applyPickUpObject( MachActor* pActor, string* )
{
	ASSERT_INFO( pActor->objectType() );
	ASSERT( pActor->objectType() == MachLog::RESOURCE_CARRIER, "Unexpected actor type." );
	ASSERT( pActor->asResourceCarrier().isNormalResourceCarrier(), "Resource carrier is a scavenger!" );

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

	if( addingOrReplacing_ == ADDING )
		pActor->asResourceCarrier().addSuppliers( suppliers_ );
	else
		pActor->asResourceCarrier().setSuppliers( suppliers_ );
	// pOp = _NEW( MachLogPickUpOperation( &pActor->asResourceCarrier(), suppliers_ ) );

    return true;
}

//virtual
MachGui::Cursor2dType MachGuiPickUpCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::MENU_CURSOR;

    if( isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES  ) )
        cursor = MachGui::MOVETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiPickUpCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

	bool myRace = ( MachLogRaces::instance().pcController().race() == pActor->race() );

    //Check for a building or machine
	if( ( pActor->objectType() == MachLog::MINE and pActor->asMine().worthVisiting() )
		or ( pActor->objectIsCanSmelt() and not myRace ) )
    {
        //Set the PickUp object action
        action_ = PICKUP;
        cursor = MachGui::PICKUP_CURSOR;
    }

    return cursor;
}

//virtual
void MachGuiPickUpCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiPickUpCommand::clone() const
{
    return _NEW( MachGuiPickUpCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiPickUpCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/pickup.bmp", "gui/commands/pickup.bmp" );
    return names;
}

//virtual
uint MachGuiPickUpCommand::cursorPromptStringId() const
{
    return IDS_PICKUP_COMMAND;
}

//virtual
uint MachGuiPickUpCommand::commandPromptStringid() const
{
    return IDS_PICKUP_START;
}

//virtual
bool MachGuiPickUpCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_K and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

//virtual
bool MachGuiPickUpCommand::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int /*clientData*/ )
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
void MachGuiPickUpCommand::domainDeleted( W4dDomain*  )
{
	//intentionally empty...override as necessary
}

bool MachGuiPickUpCommand::constructionIsDuplicate( const MachLogConstruction* pCandidateConstruction ) const
{
	bool result = false;

	ctl_pvector< MachLogConstruction >::const_iterator i = find( suppliers_.begin(), suppliers_.end(), pCandidateConstruction );

	if( i != suppliers_.end() )
		result = true;

	return result;
}


/* End CMDPICKU.CPP **************************************************/
