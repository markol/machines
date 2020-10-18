/*
 * C M D L O C T O  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "machgui/cmdlocto.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/oplocate.hpp"
#include "machlog/opadloc.hpp"
#include "machlog/administ.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "ctl/pvector.hpp"
#include "system/pathname.hpp"

MachGuiLocateToCommand::MachGuiLocateToCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false )
{
    //Ensure reasonable allocation size
    path_.reserve( 8 );

    TEST_INVARIANT;
}

MachGuiLocateToCommand::~MachGuiLocateToCommand()
{
    TEST_INVARIANT;

}

void MachGuiLocateToCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiLocateToCommand& t )
{

    o << "MachGuiLocateToCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiLocateToCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiLocateToCommand::pickOnTerrain
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
void MachGuiLocateToCommand::pickOnActor
(
    MachActor*, bool, bool, bool
)
{
    //Ignored
}

//virtual
bool MachGuiLocateToCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Locators can locate
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::GEO_LOCATOR;
}

//virtual
bool MachGuiLocateToCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiLocateToCommand::doApply( MachActor* pActor, string* )
{
    PRE( pActor->objectIsMachine() );
    PRE( path_.size() != 0 );

    //Check locator type
    bool canDo = false;
    if( pActor->objectType() == MachLog::GEO_LOCATOR )
    {
        //Construct the geo locate op
        MachLogLocateOperation::Path path;

		if ( convertPointsToValidPoints( IGNORE_SELECTED_ACTOR_OBSTACLES, &pActor->asMachine(), path_, &path ) )
		{
	        MachLogLocateOperation* pOp =
	            _NEW( MachLogLocateOperation( &pActor->asGeoLocator(), path ) );

	        //Give to actor
	        pActor->newOperation( pOp );
	        canDo = true;

			if( not hasPlayedVoiceMail() )
			{
				MachLogVoiceMailManager::instance().postNewMail( VID_GEO_LOCATING, pActor->id(), pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiLocateToCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
        cursor = MachGui::LOCATETO_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiLocateToCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
void MachGuiLocateToCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiLocateToCommand::clone() const
{
    return _NEW( MachGuiLocateToCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiLocateToCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/locate.bmp", "gui/commands/locate.bmp" );
    return names;
}

//virtual
uint MachGuiLocateToCommand::cursorPromptStringId() const
{
    return IDS_LOCATETO_COMMAND;
}

//virtual
uint MachGuiLocateToCommand::commandPromptStringid() const
{
    return IDS_LOCATETO_START;
}

//virtual
bool MachGuiLocateToCommand::canAdminApply() const
{
    return true;
}

//virtual
bool MachGuiLocateToCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );;
    PRE( path_.size() != 0 );

    //Construct the geo locate op
    MachLogLocateOperation::Path path;

	if ( convertPointsToValidPoints( IGNORE_SELECTED_ACTOR_OBSTACLES, pAdministrator, path_, &path ) )
	{
	    //Create an admin Move operation for the administrator
	    MachLogAdminLocateOperation* pOp =
	        _NEW( MachLogAdminLocateOperation( pAdministrator, path ) );

	    pAdministrator->newOperation( pOp );

		MachActor* pFirstGeoLocator = NULL;

		bool found = false;
		for( MachInGameScreen::Actors::const_iterator i = inGameScreen().selectedActors().begin(); not found and i != inGameScreen().selectedActors().end(); ++i )
		if( (*i)->objectType() == MachLog::GEO_LOCATOR )
		{
			found = true;
			pFirstGeoLocator = (*i);
		}

		ASSERT( found, "No constructor found in corral!" );

		// give out voicemail
		MachLogVoiceMailManager::instance().postNewMail( VID_GEO_LOCATING, pFirstGeoLocator->id(), pFirstGeoLocator->race() );
	}

    return true;
}

//virtual
bool MachGuiLocateToCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( 	isVisible() and
			be.scanCode() == DevKey::KEY_L and
			be.action() == DevButtonEvent::PRESS and
			be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDLOCTO.CPP **************************************************/
