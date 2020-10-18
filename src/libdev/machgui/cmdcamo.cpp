/*
 * C M D C A M O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdcamo.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/network.hpp"
#include "world4d/domain.hpp"
#include "device/butevent.hpp"


MachGuiCamouflageCommand::MachGuiCamouflageCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiCamouflageCommand::~MachGuiCamouflageCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );
}

void MachGuiCamouflageCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCamouflageCommand& t )
{

    o << "MachGuiCamouflageCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiCamouflageCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCamouflageCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
void MachGuiCamouflageCommand::pickOnActor
(
    MachActor* /*pActor*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
bool MachGuiCamouflageCommand::canActorEverExecute( const MachActor& actor ) const
{
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    return 	MachLogNetwork::instance().isNetworkGame() and
    		actor.objectType() == MachLog::SPY_LOCATOR and
    		actor.race() == playerRace;
}

//virtual
bool MachGuiCamouflageCommand::isInteractionComplete() const
{
    return true;
}

//virtual
MachGui::Cursor2dType MachGuiCamouflageCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/ )
{
    return MachGui::INVALID_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiCamouflageCommand::cursorOnActor( MachActor* /*pActor*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/ )
{
    return MachGui::INVALID_CURSOR;
}

//virtal
void MachGuiCamouflageCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiCamouflageCommand::doApply( MachActor* pActor, string* )
{
	PRE( pActor->objectIsMachine() );
	PRE( pActor->objectType() == MachLog::SPY_LOCATOR );

    // Toggle camouflage
    pActor->asMachine().camouflaged( not pActor->asMachine().camouflaged() );

    return true;
}

//virtual
MachGuiCommand* MachGuiCamouflageCommand::clone() const
{
    return _NEW( MachGuiCamouflageCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiCamouflageCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/camo.bmp", "gui/commands/camo.bmp" );
    return names;
}

//virtual
void MachGuiCamouflageCommand::start()
{}

//virtual
void MachGuiCamouflageCommand::finish()
{}

//virtual
uint MachGuiCamouflageCommand::cursorPromptStringId() const
{
    return IDS_CAMOUFLAGE_COMMAND;
}

//virtual
uint MachGuiCamouflageCommand::commandPromptStringid() const
{
    return IDS_CAMOUFLAGE_START;
}

//virtual
bool MachGuiCamouflageCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiCamouflageCommand::doAdminApply( MachLogAdministrator*, string* )
{
    PRE( canAdminApply() );

    return false;
}

//virtual
bool MachGuiCamouflageCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_O and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDCAMO.CPP **************************************************/
