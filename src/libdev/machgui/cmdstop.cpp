/*
 * C M D S T O P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdstop.hpp"
#include "machgui/ingame.hpp"
#include "machgui/cmddestr.hpp"
#include "machgui/internal/strings.hpp"
#include "world4d/domain.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/strategy.hpp"
#include "machlog/administ.hpp"
#include "machlog/races.hpp"
#include "machlog/squad.hpp"
#include "machlog/cntrl_pc.hpp"
#include "device/butevent.hpp"

MachGuiStopCommand::MachGuiStopCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiStopCommand::~MachGuiStopCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );
}

void MachGuiStopCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiStopCommand& t )
{

    o << "MachGuiStopCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiStopCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiStopCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
void MachGuiStopCommand::pickOnActor
(
    MachActor* /*pActor*/, bool, bool /*shiftPressed*/, bool
)
{}

//virtual
bool MachGuiStopCommand::canActorEverExecute( const MachActor& actor ) const
{
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //Machines/Missile emplacements can stop what they are doing, must be players machine
    return ( actor.objectIsMachine() or actor.objectType() == MachLog::MISSILE_EMPLACEMENT ) and actor.race() == playerRace;
}

//virtual
bool MachGuiStopCommand::isInteractionComplete() const
{
    return true;
}

//virtual
MachGui::Cursor2dType MachGuiStopCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool, bool )
{
    return MachGui::INVALID_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiStopCommand::cursorOnActor( MachActor* /*pActor*/, bool, bool, bool )
{
    return MachGui::INVALID_CURSOR;
}

//virtal
void MachGuiStopCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiStopCommand::doApply( MachActor* pActor, string* )
{
    // Stop what machine is currently doing
    pActor->strategy().tryToRemoveAllOperations();

	if( pActor->objectIsMachine() )
    	pActor->asMachine().manualCommandIssued();

	//Cancel self-destruct
	if ( pActor->selfDestruct() )
		pActor->selfDestruct( 0.0 );

    MachGuiSelfDestructCommand::stopPressed();

    return true;
}

//virtual
MachGuiCommand* MachGuiStopCommand::clone() const
{
    return _NEW( MachGuiStopCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiStopCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/stop.bmp", "gui/commands/stop.bmp" );
    return names;
}

//virtual
void MachGuiStopCommand::start()
{}

//virtual
void MachGuiStopCommand::finish()
{}

//virtual
uint MachGuiStopCommand::cursorPromptStringId() const
{
    return IDS_STOP_COMMAND;
}

//virtual
uint MachGuiStopCommand::commandPromptStringid() const
{
    return IDS_STOP_START;
}

//virtual
bool MachGuiStopCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiStopCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );

    // Stop what machine is currently doing
    pAdministrator->strategy().tryToRemoveAllOperations();
	ASSERT( pAdministrator->squadron(), "Administrator didn't have a squadron!" );
	pAdministrator->squadron()->manualCommandIssuedToSquadron();

    return true;
}

//virtual
bool MachGuiStopCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_S and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDSTOP.CPP **************************************************/
