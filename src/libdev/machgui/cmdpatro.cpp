/*
 * C M D P A T R O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdpatro.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/actor.hpp"
#include "machlog/machvman.hpp"
#include "machlog/patrol.hpp"
#include "device/butevent.hpp"
#include "mathex/transf3d.hpp"
#include "system/pathname.hpp"

MachGuiPatrolCommand::MachGuiPatrolCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false )
{
    //Ensure reasonable vector size
    path_.reserve( 8 );

    TEST_INVARIANT;
}

MachGuiPatrolCommand::~MachGuiPatrolCommand()
{
    TEST_INVARIANT;

}

void MachGuiPatrolCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiPatrolCommand& t )
{

    o << "MachGuiPatrolCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiPatrolCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiPatrolCommand::pickOnTerrain
(
    const MexPoint3d& location, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) == MachGui::PATROL_CURSOR )
    {
        //Store the location and set the action
        path_.push_back( location );

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
void MachGuiPatrolCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check the location is on the ground - not up a hill
    if( cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) == MachGui::PATROL_CURSOR )
    {
        //Store the location and set the action
        path_.push_back( pActor->globalTransform().position() );

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
bool MachGuiPatrolCommand::canActorEverExecute( const MachActor& actor ) const
{
    return actor.objectIsMachine();
}

//virtual
bool MachGuiPatrolCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiPatrolCommand::doApply( MachActor* pActor, string* )
{
    PRE( path_.size() != 0 );

    //Only administrator's can do it at present
    bool canDo = pActor->objectIsMachine();
    if( canDo )
    {
        MachLogPatrolOperation::Path path;

		if ( convertPointsToValidPoints( IGNORE_SELECTED_ACTOR_OBSTACLES, &pActor->asMachine(), path_, &path ) )
		{
	        //Make the current position the last on the path
        	path.push_back( pActor->position() );

		    //Construct the patrol op
	        MachLogPatrolOperation* pOp =
	            _NEW( MachLogPatrolOperation( &pActor->asMachine(), path, true ) );

	        //Give to actor
	        pActor->newOperation( pOp );

		   	if( not hasPlayedVoiceMail() )
			{
				MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_TASKED, pActor->id(),  pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiPatrolCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::MENU_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
        cursor = MachGui::PATROL_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiPatrolCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
	if ( pActor->objectIsConstruction() )
	{
    	return MachGui::INVALID_CURSOR;
	}

    return MachGui::PATROL_CURSOR;
}

//virtual
void MachGuiPatrolCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiPatrolCommand::clone() const
{
    return _NEW( MachGuiPatrolCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiPatrolCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/patrol.bmp", "gui/commands/patrol.bmp" );
    return names;
}

//virtual
uint MachGuiPatrolCommand::cursorPromptStringId() const
{
    return IDS_PATROL_COMMAND;
}

//virtual
uint MachGuiPatrolCommand::commandPromptStringid() const
{
    return IDS_PATROL_START;
}

//virtual
bool MachGuiPatrolCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_P and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDPATRO.CPP **************************************************/
