/*
 * C M D L O C T O  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "machgui/cmdlmine.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "device/butevent.hpp"
#include "system/pathname.hpp"
#include "machlog/actor.hpp"
#include "machlog/oplmine.hpp"
#include "machlog/oplocate.hpp"
#include "machlog/opadloc.hpp"
#include "machlog/spy.hpp"
#include "machlog/administ.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "ctl/pvector.hpp"

MachGuiDropLandMineCommand::MachGuiDropLandMineCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false )
{
    //Ensure reasonable allocation size
    path_.reserve( 8 );

    TEST_INVARIANT;
}

MachGuiDropLandMineCommand::~MachGuiDropLandMineCommand()
{
    TEST_INVARIANT;

}

void MachGuiDropLandMineCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDropLandMineCommand& t )
{

    o << "MachGuiDropLandMineCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiDropLandMineCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiDropLandMineCommand::pickOnTerrain
(
    const MexPoint3d& location, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) !=
        MachGui::INVALID_CURSOR )
    {
        //Store the location
        path_.push_back( MexPoint2d( location ) );

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
void MachGuiDropLandMineCommand::pickOnActor
(
    MachActor*, bool, bool, bool
)
{
    //Ignored
}

//virtual
bool MachGuiDropLandMineCommand::canActorEverExecute( const MachActor& actor ) const
{
    // Spies with at least one mine can initiate a drop land mine operation.
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::SPY_LOCATOR and actor.asSpyLocator().nMines() > 0;
}

//virtual
bool MachGuiDropLandMineCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiDropLandMineCommand::doApply( MachActor* pActor, string* /*pReason*/ )
{
    PRE( pActor->objectIsMachine() );
    PRE( path_.size() != 0 );

    //Check locator type
    bool canDo = false;
    if( pActor->objectType() == MachLog::SPY_LOCATOR )
    {
        //Construct the geo locate op
        MachLogDropLandMineOperation::Path path;

		if ( convertPointsToValidPoints( USE_ALL_OBSTACLES, &pActor->asMachine(), path_, &path ) )
		{
	        MachLogDropLandMineOperation* pOp =
	            _NEW( MachLogDropLandMineOperation( &pActor->asSpyLocator(), path ) );

	        //Give to actor
	        pActor->newOperation( pOp );
	        canDo = true;

			if( not hasPlayedVoiceMail() )
			{
				MachLogVoiceMailManager::instance().postNewMail( VID_SPY_TASKED_LAY_MINE, pActor->id(), pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiDropLandMineCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, USE_ALL_OBSTACLES  ) )
        cursor = MachGui::DROPLANDMINE_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiDropLandMineCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
void MachGuiDropLandMineCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiDropLandMineCommand::clone() const
{
    return _NEW( MachGuiDropLandMineCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiDropLandMineCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/dropmine.bmp", "gui/commands/dropmine.bmp" );
    return names;
}

//virtual
uint MachGuiDropLandMineCommand::cursorPromptStringId() const
{
    return IDS_DROPLANDMINE_COMMAND;
}

//virtual
uint MachGuiDropLandMineCommand::commandPromptStringid() const
{
    return IDS_DROPLANDMINE_START;
}

//virtual
bool MachGuiDropLandMineCommand::canAdminApply() const
{
	return false;
//    return true;
}

//virtual
bool MachGuiDropLandMineCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );;
    PRE( path_.size() != 0 );

    //Construct the spy locate op
    MachLogLocateOperation::Path path;

	if ( convertPointsToValidPoints( USE_ALL_OBSTACLES, pAdministrator, path_, &path ) )
	{
	    //Create an admin Move operation for the administrator
	    MachLogAdminLocateOperation* pOp =
	        _NEW( MachLogAdminLocateOperation( pAdministrator, path ) );

	    pAdministrator->newOperation( pOp );

		MachActor* pFirstSpyLocator = NULL;

		bool found = false;
		for( MachInGameScreen::Actors::const_iterator i = inGameScreen().selectedActors().begin(); not found and i != inGameScreen().selectedActors().end(); ++i )
			if( (*i)->objectType() == MachLog::SPY_LOCATOR )
			{
				found = true;
				pFirstSpyLocator = (*i);
			}

		ASSERT( found, "No constructor found in corral!" );

		// give out voicemail
		MachLogVoiceMailManager::instance().postNewMail( VID_SPY_TASKED_LAY_MINE, pFirstSpyLocator->id(), pFirstSpyLocator->race() );
	}

    return true;
}

//virtual
bool MachGuiDropLandMineCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and
		 be.scanCode() == DevKey::KEY_M and
		 be.action() == DevButtonEvent::PRESS and
		 be.wasShiftPressed() and
		 be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDLOCTO.CPP **************************************************/
