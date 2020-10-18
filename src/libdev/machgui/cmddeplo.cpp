/*
 * C M D D E P L O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmddeplo.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/apc.hpp"
#include "machlog/opdapc.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "ctl/pvector.hpp"

MachGuiDeployCommand::MachGuiDeployCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false )
{
    TEST_INVARIANT;
}

MachGuiDeployCommand::~MachGuiDeployCommand()
{
    TEST_INVARIANT;

}

void MachGuiDeployCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDeployCommand& t )
{

    o << "MachGuiDeployCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiDeployCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiDeployCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                        bool shiftPressed, bool altPressed )
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) !=
        MachGui::INVALID_CURSOR )
    {
        //Store the location
        location_ = location;
        hadFinalPick_ = true;
    }
}

//virtual
void MachGuiDeployCommand::pickOnActor
(
    MachActor*, bool, bool, bool
)
{
    //Nothing to do
}

//virtual
bool MachGuiDeployCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Only laden APCs can deploy.
    return actor.objectType() == MachLog::APC and actor.asAPC().amountCarried() > 0;
}

//virtual
bool MachGuiDeployCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiDeployCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsMachine() );

    //Ensure the APC has some contents
    MachLogAPC& apc = pActor->asAPC();
    bool canDo = apc.machines().size() > 0;
    if( canDo )
    {
    	MexPoint2d validPoint;
		bool valid = findClosestPointValidOnTerrain(location_,
					 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
					 					    		&pActor->asMachine(),
					 					    		&validPoint );

		if ( valid )
		{
		    //Construct the operation
	        MachLogDeployAPCOperation* pOp = _NEW( MachLogDeployAPCOperation ( &apc, validPoint ) );

	        //Give to apc
	        pActor->newOperation( pOp );

			if( not hasPlayedVoiceMail() )
			{
				MachLogVoiceMailManager::instance().postNewMail( VID_APC_MOVING_TO_DEPLOY_POINT, pActor->id(), pActor->race() );
				hasPlayedVoiceMail( true );
			}
		}

    }
    else
        *pReason = "Nothing to deploy";

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiDeployCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
        cursor = MachGui::DEPLOY_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiDeployCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
void MachGuiDeployCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiDeployCommand::clone() const
{
    return _NEW( MachGuiDeployCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiDeployCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/deploy.bmp", "gui/commands/deploy.bmp" );
    return names;
}

//virtual
uint MachGuiDeployCommand::cursorPromptStringId() const
{
    return IDS_DEPLOY_COMMAND;
}

//virtual
uint MachGuiDeployCommand::commandPromptStringid() const
{
    return IDS_DEPLOY_START;
}

//virtual
bool MachGuiDeployCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_E and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDDEPLO.CPP **************************************************/
