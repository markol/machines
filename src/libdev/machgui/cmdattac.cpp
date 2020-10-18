/*
 * C M D A T T A C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdattac.hpp"
#include "machgui/ingame.hpp"
#include "machphys/machphys.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/attack.hpp"
#include "machlog/machvman.hpp"
#include "machlog/move.hpp"
#include "machlog/missilem.hpp"
#include "machlog/opadatta.hpp"
#include "machlog/opadmove.hpp"
#include "machlog/opmeatta.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/patrol.hpp"
#include "machlog/pod.hpp"
#include "machlog/squad.hpp"
#include "device/butevent.hpp"
#include "ctl/pvector.hpp"

MachGuiAttackCommand::MachGuiAttackCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    action_( ATTACK_OBJECT ),
    pDirectObject_( NULL ),
    hadFinalPick_( false )
{
    TEST_INVARIANT;
}

MachGuiAttackCommand::~MachGuiAttackCommand()
{
    TEST_INVARIANT;

}

void MachGuiAttackCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiAttackCommand& t )
{

    o << "MachGuiAttackCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiAttackCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiAttackCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed )
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) !=
        MachGui::MENU_CURSOR )
    {
        //Store the location and set the action
        action_ = MOVE_TO_LOCATION;
        location_ = location;
        hadFinalPick_ = true;
    }
}

//virtual
void MachGuiAttackCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
	MachGui::Cursor2dType cursor = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed );

	hadFinalPick_ = ( cursor != MachGui::INVALID_CURSOR ) and
					( cursor != MachGui::MENU_CURSOR );
}

//virtual
bool MachGuiAttackCommand::canActorEverExecute( const MachActor& actor ) const
{

    //Administrators and aggressors can attack
    MachLog::ObjectType objectType = actor.objectType();
    return(	objectType == MachLog::ADMINISTRATOR  or
    		objectType == MachLog::AGGRESSOR or
			( objectType == MachLog::MISSILE_EMPLACEMENT
			  and actor.asMissileEmplacement().subType() != MachPhys::ICBM
			  and actor.asMissileEmplacement().isComplete() ) );
}

//virtual
bool MachGuiAttackCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiAttackCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsCanAttack() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case MOVE_TO_LOCATION:
            canDo = applyMove( pActor, pReason );
            break;

        case ATTACK_OBJECT:
            canDo = applyAttackObject( pActor, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiAttackCommand::applyMove( MachActor* pActor, string* )
{
    //Construct a move operation
	bool valid = false;
	if( pActor->objectIsMachine() )
	{
		MexPoint2d validPoint;
		valid = findClosestPointValidOnTerrain(location_,
					 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
					 					    		&pActor->asMachine(),
					 					    		&validPoint );

		if ( valid )
		{
		    MachLogMoveToOperation* pOp =
		        _NEW( MachLogMoveToOperation( &pActor->asMachine(), validPoint, commandId(), pathFindingPriority() ) );

		    //Give it to the actor

            // The operation is now given to the actor by the group move code
            add( &pActor->asMachine(), pOp );

			pActor->asMachine().manualCommandIssued();

			if( not hasPlayedVoiceMail() )
			{
				MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_MOVING, pActor->id(),  pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}
	}

    return valid;
}

bool MachGuiAttackCommand::applyAttackObject( MachActor* pActor, string* )
{
    bool canDo = pActor != pDirectObject_					//Check not trying to attack oneself
    			 and pActor->objectIsCanAttack()
				 and pActor->asCanAttack().canFireAt( *pDirectObject_ );

    if( canDo )
    {
        //Construct appropriate type of operation
        MachLogOperation* pOp;

        switch( pActor->objectType() )
        {
            case MachLog::AGGRESSOR:
            case MachLog::ADMINISTRATOR:
            {
                pOp = _NEW( MachLogAttackOperation( &pActor->asMachine(), pDirectObject_, commandId(), MachLogAttackOperation::TERMINATE_ON_CHANGE ) );

				if( not hasPlayedVoiceMail() )
				{
					MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_TARGET_ENEMY, pActor->id(),  pActor->race() );
					hasPlayedVoiceMail( true );
				}
				pActor->asMachine().manualCommandIssued();

                // The operation is now given to the actor by the group move code
                add( &pActor->asMachine(), pOp );

                break;
            }
            case MachLog::MISSILE_EMPLACEMENT:
            {
				MachLogMissileEmplacement& missileEmp = pActor->asMissileEmplacement();
				ASSERT_INFO( missileEmp.subType() );
				ASSERT( missileEmp.subType() != MachPhys::ICBM, "An ICBM missile emplacement should NOT be initiating a conventional attack operation." );

				// give a little leeway for this target to be picked up as the current target, otherwise
				// CAACT could wipe it off before it even has a chance to get to doStart
				missileEmp.suppressAttackingUrges( 3.0 );

			   	pOp = _NEW( MachLogMissileEmplacementAttackOperation( &missileEmp, pDirectObject_ ) );

                missileEmp.newOperation( pOp );

				break;
            }
            default:
                ASSERT_BAD_CASE;
        }
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiAttackCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::MENU_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES  ) )
        cursor = MachGui::MOVETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiAttackCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursorType = MachGui::INVALID_CURSOR;

    //Check for a building or machine
    if( pActor->objectIsConstruction() or pActor->objectIsMachine() or
          pActor->objectIsArtefact() or pActor->objectIsLandMine() )
	{
		// Make sure users not trying to attack same machine as selected
		// machine.
		if ( inGameScreen().selectedActors().size() == 1 and
			 inGameScreen().selectedActors().front() == pActor )
		{
			cursorType = MachGui::MENU_CURSOR;
		}
		else if( atLeastOneCanFireAt( *pActor ) )
		{
			//Set the attack object action
	        action_ = ATTACK_OBJECT;
	        cursorType = MachGui::ATTACK_CURSOR;
	        pDirectObject_ = pActor;
		}
    }
	else if ( pActor->objectIsDebris() )
	{
		// Allow actors to move on top of debris
        action_ = MOVE_TO_LOCATION;
	    cursorType = MachGui::MOVETO_CURSOR;
    }

    return cursorType;
}

//virtual
void MachGuiAttackCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiAttackCommand::clone() const
{
    return _NEW( MachGuiAttackCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiAttackCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/attack.bmp", "gui/commands/attack.bmp" );
    return names;
}

//virtual
uint MachGuiAttackCommand::cursorPromptStringId() const
{
    return IDS_ATTACK_COMMAND;
}

//virtual
uint MachGuiAttackCommand::commandPromptStringid() const
{
    return IDS_ATTACK_START;
}

//virtual
bool MachGuiAttackCommand::canAdminApply() const
{
    return true;
}

//virtual
bool MachGuiAttackCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* pReason )
{
	PRE( canAdminApply() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case MOVE_TO_LOCATION:
            canDo = applyAdminMove( pAdministrator, pReason );
            break;

        case ATTACK_OBJECT:
            canDo = applyAdminAttackObject( pAdministrator, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiAttackCommand::applyAdminAttackObject( MachLogAdministrator* pAdministrator, string* )
{
    //Check not trying to attack oneself
    bool canDo = pAdministrator != pDirectObject_;
    if( canDo )
    {
	    //Create an admin Attack operation for the administrator
	    MachLogAdminAttackOperation* pOp =
	        _NEW( MachLogAdminAttackOperation( pAdministrator, pDirectObject_, pathFindingPriority() ) );

        // The operation is now given to the actor by the group move code
        add( pAdministrator, pOp );

		ASSERT( pAdministrator->squadron(), "Administrator didn't have a squadron!" );
		pAdministrator->squadron()->manualCommandIssuedToSquadron();

		// give voicemail
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::ADMINISTRATOR, pAdministrator->subType(), MachLogMachineVoiceMailManager::MEV_TARGET_ENEMY, pAdministrator->id(),  pAdministrator->race() );
	}

    return canDo;
}

//virtual
bool MachGuiAttackCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( 	isVisible() and
			be.scanCode() == DevKey::KEY_A and
			be.action() == DevButtonEvent::PRESS and
			be.wasAltPressed() == false and
			be.wasCtrlPressed() == false and
			be.wasShiftPressed() == false and
			be.previous() == 0  )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

bool MachGuiAttackCommand::atLeastOneCanFireAt( const MachActor& potentialTargetActor ) const
{
	bool noneCanFireAt = true;

	for( MachInGameScreen::Actors::const_iterator iter = inGameScreen().selectedActors().begin();
		 iter != inGameScreen().selectedActors().end() and noneCanFireAt;
		 ++iter )
	{
		MachActor* pActor = (*iter);

		if( pActor->objectIsCanAttack()
			and pActor != &potentialTargetActor					// can't attack oneself.......
			and pActor->asCanAttack().canFireAt( potentialTargetActor ) )
		{
			noneCanFireAt = false;
		}
	}

	return not( noneCanFireAt );
}

bool MachGuiAttackCommand::applyAdminMove( MachLogAdministrator* pAdministrator, string* )
{
	MexPoint2d validPoint;

	bool valid = findClosestPointValidOnTerrain(location_,
					 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
					 					    		&pAdministrator->asMachine(),
					 					    		&validPoint );

	if( valid )
	{
	    //Create an admin Move operation for the administrator
	    MachLogAdminMoveToOperation* pOp =
	        _NEW( MachLogAdminMoveToOperation( pAdministrator, validPoint, pathFindingPriority() ) );

		// give voicemail
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::ADMINISTRATOR, pAdministrator->subType(), MachLogMachineVoiceMailManager::MEV_MOVING, pAdministrator->id(),  pAdministrator->race() );

        // The operation is now given to the actor by the group move code
        add( pAdministrator, pOp );

		ASSERT( pAdministrator->squadron(), "Administrator didn't have a squadron!" );
		pAdministrator->squadron()->manualCommandIssuedToSquadron();
	}

    return valid;
}


/* End CMDATTAC.CPP **************************************************/
