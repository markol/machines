/*
 * C M D A T T A C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdtrech.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/move.hpp"
#include "machlog/optreach.hpp"
#include "machlog/patrol.hpp"
#include "machlog/races.hpp"

MachGuiTreacheryCommand::MachGuiTreacheryCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    action_( ATTACK_OBJECT ),
    pDirectObject_( NULL ),
    hadFinalPick_( false )
{
    TEST_INVARIANT;
}

MachGuiTreacheryCommand::~MachGuiTreacheryCommand()
{
    TEST_INVARIANT;

}

void MachGuiTreacheryCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiTreacheryCommand& t )
{

    o << "MachGuiTreacheryCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiTreacheryCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiTreacheryCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed )
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) !=
        MachGui::INVALID_CURSOR )
    {
        //Store the location and set the action
        action_ = MOVE_TO_LOCATION;
        location_ = location;
        hadFinalPick_ = true;
    }
}

//virtual
void MachGuiTreacheryCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    hadFinalPick_ = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) !=
                        MachGui::INVALID_CURSOR;
}

//virtual
bool MachGuiTreacheryCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Administrators and aggressors can Treachery
    MachLog::ObjectType objectType = actor.objectType();
    return 	(objectType == MachLog::ADMINISTRATOR  or
    		objectType == MachLog::AGGRESSOR) and actor.asCanAttack().hasTreacheryWeapon();
}

//virtual
bool MachGuiTreacheryCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiTreacheryCommand::doApply( MachActor* pActor, string* pReason )
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
            canDo = applyTreacheryObject( pActor, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiTreacheryCommand::applyMove( MachActor* pActor, string* )
{
    //Construct a move operation
	bool result = false;
	if( pActor->objectIsMachine() )
	{
		MexPoint2d validPoint;
	 	bool valid = findClosestPointValidOnTerrain(location_,
					 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
					 					    		&pActor->asMachine(),
					 					    		&validPoint );

		if ( valid )
		{
		    MachLogMoveToOperation* pOp =
		        _NEW( MachLogMoveToOperation( &pActor->asMachine(), validPoint ) );

		    //Give it to the actor
		    pActor->newOperation( pOp );
			result = true;

			if( not hasPlayedVoiceMail() )
			{
				MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_MOVING, pActor->id(),  pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}
	}

    return result;
}

bool MachGuiTreacheryCommand::applyTreacheryObject( MachActor* pActor, string* )
{
    //Check not trying to Treachery actor of same race (note that this trivially includes oneself).
    bool canDo = pActor->race() != pDirectObject_->race();
    if( canDo )
    {
        //Construct appropriate type of operation
        MachLogOperation* pOp;

		ASSERT( ( pActor->objectType() == MachLog::ADMINISTRATOR or pActor->objectType() == MachLog::AGGRESSOR )
				and pActor->asCanAttack().hasTreacheryWeapon(),
				"Unexpected non-treachery-capable actor about to be issued a treachery op." );

		pOp = _NEW( MachLogTreacheryOperation( &pActor->asMachine(), pDirectObject_ ) );

        //Give it to the actor
        pActor->newOperation( pOp );

		if( not hasPlayedVoiceMail() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::ADMINISTRATOR, pActor->subType(), MachLogMachineVoiceMailManager::MEV_TREACHERY_TARGET, pActor->id(),  pActor->race() );
			hasPlayedVoiceMail( true );
		}
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiTreacheryCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES  ) )
        cursor = MachGui::MOVETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiTreacheryCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursorType = MachGui::INVALID_CURSOR;

	MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //Check for a building or machine
    if( pActor->objectIsMachine()
    	and pActor->race() != playerRace )
    {
        //Set the Treachery object action
        action_ = ATTACK_OBJECT;
        cursorType = MachGui::TREACHERY_CURSOR;
        pDirectObject_ = pActor;
    }

    return cursorType;
}

//virtual
void MachGuiTreacheryCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiTreacheryCommand::clone() const
{
    return _NEW( MachGuiTreacheryCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiTreacheryCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/treach.bmp", "gui/commands/treach.bmp" );
    return names;
}

//virtual
uint MachGuiTreacheryCommand::cursorPromptStringId() const
{
    return IDS_TREACHERY_COMMAND;
}

//virtual
uint MachGuiTreacheryCommand::commandPromptStringid() const
{
    return IDS_TREACHERY_START;
}

//virtual
bool MachGuiTreacheryCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiTreacheryCommand::doAdminApply( MachLogAdministrator* /*pAdministrator*/, string* )
{
    return false;
}

//virtual
bool MachGuiTreacheryCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_J and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDATTAC.CPP **************************************************/
