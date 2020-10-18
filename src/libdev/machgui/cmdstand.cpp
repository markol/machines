/*
 * C M D S T A N D  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdstand.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"

#include "device/butevent.hpp"

#include "world4d/domain.hpp"

#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/machvman.hpp"
#include "machlog/opstandg.hpp"


MachGuiStandGroundCommand::MachGuiStandGroundCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiStandGroundCommand::~MachGuiStandGroundCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );
}

void MachGuiStandGroundCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiStandGroundCommand& t )
{

    o << "MachGuiStandGroundCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiStandGroundCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiStandGroundCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
void MachGuiStandGroundCommand::pickOnActor
(
    MachActor* /*pActor*/, bool, bool, bool
)
{}

//virtual
bool MachGuiStandGroundCommand::canActorEverExecute( const MachActor& actor ) const
{
	// All machines can stand ground
	return actor.objectIsMachine();
}

//virtual
bool MachGuiStandGroundCommand::isInteractionComplete() const
{
    return true;
}

//virtual
MachGui::Cursor2dType MachGuiStandGroundCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool, bool )
{
    return MachGui::MENU_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiStandGroundCommand::cursorOnActor( MachActor* /*pActor*/, bool, bool, bool )
{
    return MachGui::MENU_CURSOR;
}

//virtal
void MachGuiStandGroundCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiStandGroundCommand::doApply( MachActor* pActor, string* )
{
	ASSERT( pActor->objectIsMachine(), "Tried to give stand ground operation to non-machine actor!" );

	bool result = false;
	MachLogMachine& machine = pActor->asMachine();

	if( not machine.isStandingGround() )
	{
		//Create a stand ground operation for the machine
	    MachLogStandGroundOperation* pOp =
	        _NEW( MachLogStandGroundOperation( &machine ) );

	    machine.newOperation( pOp );

		if( not hasPlayedVoiceMail() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( machine.objectType(), machine.subType(), MachLogMachineVoiceMailManager::MEV_TASKED, machine.id(), machine.race() );
			hasPlayedVoiceMail( true );
		}

		result = true;
	}

	return result;
}

//virtual
MachGuiCommand* MachGuiStandGroundCommand::clone() const
{
    return _NEW( MachGuiStandGroundCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiStandGroundCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/stand.bmp", "gui/commands/stand.bmp" );
    return names;
}

//virtual
void MachGuiStandGroundCommand::start()
{}

//virtual
void MachGuiStandGroundCommand::finish()
{}

//virtual
uint MachGuiStandGroundCommand::cursorPromptStringId() const
{
    return IDS_STANDGROUND_COMMAND;
}

//virtual
uint MachGuiStandGroundCommand::commandPromptStringid() const
{
    return IDS_STANDGROUND_START;
}

//virtual
bool MachGuiStandGroundCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiStandGroundCommand::doAdminApply( MachLogAdministrator* /*pAdministrator*/, string* )
{
	ASSERT( false, "There is no admin stand ground operation" );

    return true;
}

//virtual
bool MachGuiStandGroundCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( 	isVisible() and
			be.scanCode() == DevKey::KEY_W and
			be.action() == DevButtonEvent::PRESS and
			be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDDECON.CPP **************************************************/
