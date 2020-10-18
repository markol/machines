/*
 * C M D S C A V . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdscav.hpp"

#include "ctl/algorith.hpp"

#include "device/butevent.hpp"

#include "machlog/actor.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/debris.hpp"
#include "machlog/move.hpp"
#include "machlog/opscav.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"

#include "machgui/ingame.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"

MachGuiScavengeCommand::MachGuiScavengeCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false ),
	action_( SCAVENGE )
{
    TEST_INVARIANT;
}

MachGuiScavengeCommand::~MachGuiScavengeCommand()
{
	while( not suppliers_.empty() )
	{
		suppliers_.back()->detach( this );
		suppliers_.pop_back();
	}

    TEST_INVARIANT;
}

void MachGuiScavengeCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiScavengeCommand& t )
{

    o << "MachGuiScavengeCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiScavengeCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiScavengeCommand::pickOnTerrain( const MexPoint3d& , bool, bool, bool )
{

}

//virtual
void MachGuiScavengeCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check for a legal actor
    if( cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed )
        != MachGui::INVALID_CURSOR )
    {
		ASSERT( pActor->objectIsDebris(), "Attempting to do scavenge on a non-debris actor." );

		MachLogDebris* pCandidateDebris = &pActor->asDebris();

		if( not debrisIsDuplicate( pCandidateDebris ) )
		{
			// Add to list of constructions to deconstruct
        	suppliers_.push_back( pCandidateDebris );

        	pCandidateDebris->attach( this );
		}

		if( not shiftPressed )
		{
	        hadFinalPick_ = true;
		}
		else
		{
			// Waypoint click (i.e. not final click)
			MachGuiSoundManager::instance().playSound( "gui/sounds/waypoint.wav" );
		}
    }
}

//virtual
bool MachGuiScavengeCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Administrators and aggressors can Scavenge
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::RESOURCE_CARRIER and actor.asResourceCarrier().isScavenger();
}

//virtual
bool MachGuiScavengeCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiScavengeCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsMachine() );
	PRE( pActor->objectType() == MachLog::RESOURCE_CARRIER and pActor->asResourceCarrier().isScavenger() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case SCAVENGE:
            canDo = applyScavengeObject( pActor, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiScavengeCommand::applyScavengeObject( MachActor* pActor, string* )
{
    //Construct appropriate type of operation
    MachLogOperation* pOp;

	ASSERT_INFO( pActor->objectType() );
	ASSERT( pActor->objectType() == MachLog::RESOURCE_CARRIER, "Unexpected actor type" );
	ASSERT( pActor->asResourceCarrier().isScavenger(), "Resource carrier is not a scavenger!" );

	pOp = _NEW( MachLogScavengeOperation( &pActor->asResourceCarrier(), suppliers_ ) );

    //Give it to the actor
    pActor->newOperation( pOp );

	if( not hasPlayedVoiceMail() )
	{
		MachLogVoiceMailManager::instance().postNewMail( VID_RESOURCE_CARRIER_SCAVENGING, pActor->id(), pActor->race() );
		hasPlayedVoiceMail( true );
	}

    return true;
}

//virtual
MachGui::Cursor2dType MachGuiScavengeCommand::cursorOnTerrain( const MexPoint3d&, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiScavengeCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

	bool myRace = ( MachLogRaces::instance().pcController().race() == pActor->race() );

    //Check for a building or machine
    if( pActor->objectType() == MachLog::DEBRIS )
    {
        //Set the Scavenge object action
        action_ = SCAVENGE;
        cursor = MachGui::SCAVENGE_CURSOR;
    }

    return cursor;
}

//virtual
void MachGuiScavengeCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiScavengeCommand::clone() const
{
    return _NEW( MachGuiScavengeCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiScavengeCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/scavenge.bmp", "gui/commands/scavenge.bmp" );
    return names;
}

//virtual
uint MachGuiScavengeCommand::cursorPromptStringId() const
{
    return IDS_SCAVENGE_COMMAND;
}

//virtual
uint MachGuiScavengeCommand::commandPromptStringid() const
{
    return IDS_SCAVENGE_START;
}

//virtual
bool MachGuiScavengeCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_Y and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

bool MachGuiScavengeCommand::debrisIsDuplicate( const MachLogDebris* pCandidateDebris ) const
{
	bool result = false;

	ctl_pvector< MachLogDebris >::const_iterator i = find( suppliers_.begin(), suppliers_.end(), pCandidateDebris );

	if( i != suppliers_.end() )
		result = true;

	return result;
}

//virtual
bool MachGuiScavengeCommand::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
	{
		ctl_pvector< MachLogDebris >::iterator i = find( suppliers_.begin(), suppliers_.end(), pSubject );
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
void MachGuiScavengeCommand::domainDeleted( W4dDomain*  )
{
	//intentionally empty...override as necessary
}

/* End CMDSCAV.CPP **************************************************/
