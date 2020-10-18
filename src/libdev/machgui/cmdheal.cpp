/*
 * C M D H E A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdheal.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/move.hpp"
#include "machlog/machvman.hpp"
#include "machlog/opheal.hpp"
#include "machlog/opadheal.hpp"
#include "ctl/pvector.hpp"

MachGuiHealCommand::MachGuiHealCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    action_( HEAL_OBJECT ),
    pDirectObject_( NULL ),
    hadFinalPick_( false )
{
    TEST_INVARIANT;
}

MachGuiHealCommand::~MachGuiHealCommand()
{
    TEST_INVARIANT;

}

void MachGuiHealCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiHealCommand& t )
{

    o << "MachGuiHealCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiHealCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiHealCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
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
void MachGuiHealCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    hadFinalPick_ = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) !=
                        MachGui::INVALID_CURSOR;
}

//virtual
bool MachGuiHealCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Administrators and aggressors can Heal
	bool result = false;
    MachLog::ObjectType objectType = actor.objectType();
	if( objectType == MachLog::ADMINISTRATOR )
	{
		const MachLogAdministrator& mla = actor.asAdministrator();
		if( mla.hasHealingWeapon() )
			result = true;
	}
    return result;
}

//virtual
bool MachGuiHealCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiHealCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsMachine() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case MOVE_TO_LOCATION:
            canDo = applyMove( pActor, pReason );
            break;

        case HEAL_OBJECT:
            canDo = applyHealObject( pActor, pReason );
            break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiHealCommand::applyMove( MachActor* pActor, string* )
{
	PRE( pActor->objectIsMachine() );

	MexPoint2d validPoint;

	bool valid = findClosestPointValidOnTerrain(location_,
				 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
				 					    		&pActor->asMachine(),
				 					    		&validPoint );

	if ( valid )
	{
	    //Construct a move operation
	    MachLogMoveToOperation* pOp =
	        _NEW( MachLogMoveToOperation( &pActor->asMachine(), validPoint, commandId() ) );

	    //Give it to the actor
	    pActor->newOperation( pOp );

		if( not hasPlayedVoiceMail() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_MOVING, pActor->id(),  pActor->race() );
			hasPlayedVoiceMail( true );
		}
	}

    return true;
}

bool MachGuiHealCommand::applyHealObject( MachActor* pActor, string* )
{
    //Check not trying to Heal oneself
    bool canDo = pActor != pDirectObject_;
    if( canDo )
    {
        //Construct appropriate type of operation
        MachLogOperation* pOp;

		ASSERT( pActor->objectType() == MachLog::ADMINISTRATOR
				and pActor->asAdministrator().hasHealingWeapon(),
				"Non-administrator or non-heal-capable administrator about to be issued heal op!" );

		pOp = _NEW( MachLogHealOperation( &pActor->asAdministrator(), pDirectObject_ ) );

        //Give it to the actor
        pActor->newOperation( pOp );

		if( not hasPlayedVoiceMail() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_HEAL_TARGET, pActor->id(),  pActor->race() );
			hasPlayedVoiceMail( true );
		}

    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiHealCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
        cursor = MachGui::MOVETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiHealCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursorType = MachGui::INVALID_CURSOR;

    //Check for a machine at less than 100% hps.
    if( pActor->objectIsMachine()
    	and pActor->hpRatio() != 1.0
    	and atLeastOneCanHeal( &pActor->asMachine() ) )
    {
        //Set the Heal object action
        action_ = HEAL_OBJECT;
        cursorType = MachGui::HEAL_CURSOR;
        pDirectObject_ = pActor;
    }

    return cursorType;
}

//virtual
void MachGuiHealCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiHealCommand::clone() const
{
    return _NEW( MachGuiHealCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiHealCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/heal.bmp", "gui/commands/heal.bmp" );
    return names;
}

//virtual
uint MachGuiHealCommand::cursorPromptStringId() const
{
    return IDS_HEAL_COMMAND;
}

//virtual
uint MachGuiHealCommand::commandPromptStringid() const
{
    return IDS_HEAL_START;
}

//virtual
bool MachGuiHealCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiHealCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );

    //Check not trying to Heal oneself
    bool canDo = pAdministrator != pDirectObject_;
    if( canDo )
    {
	    //Create an admin Heal operation for the administrator
	    MachLogAdminHealOperation* pOp =
	        _NEW( MachLogAdminHealOperation( pAdministrator, pDirectObject_ ) );
	    pAdministrator->newOperation( pOp );

		MachActor* pFirstHealingMachine = NULL;

		bool found = false;
		for( MachInGameScreen::Actors::const_iterator i = inGameScreen().selectedActors().begin(); not found and i != inGameScreen().selectedActors().end(); ++i )
			if( (*i)->objectIsMachine() and (*i)->objectIsCanAttack() and (*i)->asCanAttack().hasHealingWeapon() )
			{
				found = true;
				pFirstHealingMachine = (*i);
			}

		ASSERT( found, "No heal-capable machine found in corral!" );

		// give out voicemail
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::ADMINISTRATOR, pFirstHealingMachine->subType(), MachLogMachineVoiceMailManager::MEV_HEAL_TARGET, pFirstHealingMachine->id(),  pFirstHealingMachine->race() );

	}

    return canDo;
}

//virtual
bool MachGuiHealCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_H and be.action() == DevButtonEvent::PRESS and be.previous() == 0  )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

bool MachGuiHealCommand::atLeastOneCanHeal( const MachLogMachine* pTargetMachine ) const
{
	bool noneCanHealThis = true;

	for( MachInGameScreen::Actors::const_iterator iter = inGameScreen().selectedActors().begin();
		 iter != inGameScreen().selectedActors().end() and noneCanHealThis;
		 ++iter )
	{
		MachActor* pActor = (*iter);

		if( pActor->objectType() == MachLog::ADMINISTRATOR
			and pActor->asAdministrator().hasHealingWeapon()
			and pActor != pTargetMachine )					// can't heal oneself.......
		{
			noneCanHealThis = false;
		}
	}

	return not( noneCanHealThis );

}

/* End CMDATTAC.CPP **************************************************/
