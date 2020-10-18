/*
 * C M D R E F I L L  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "machgui/cmdrefil.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/machvman.hpp"
#include "machlog/oprefill.hpp"
#include "machlog/administ.hpp"

MachGuiRefillLandMineCommand::MachGuiRefillLandMineCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( true )
{
    TEST_INVARIANT;
}

MachGuiRefillLandMineCommand::~MachGuiRefillLandMineCommand()
{
    TEST_INVARIANT;

}

void MachGuiRefillLandMineCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiRefillLandMineCommand& t )
{

    o << "MachGuiRefillLandMineCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiRefillLandMineCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiRefillLandMineCommand::pickOnTerrain
(
    const MexPoint3d&, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{
	//Ignored
}

//virtual
void MachGuiRefillLandMineCommand::pickOnActor
(
    MachActor*, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{
    //Ignored
}

//virtual
bool MachGuiRefillLandMineCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Locators can locate
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::SPY_LOCATOR;
}

//virtual
bool MachGuiRefillLandMineCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiRefillLandMineCommand::doApply( MachActor* pActor, string* /*pReason*/ )
{
    PRE( pActor->objectIsMachine() );

    //Check locator type
    bool canDo = false;
    if( pActor->objectType() == MachLog::SPY_LOCATOR )
    {
        MachLogRefillLandMinesOperation* pOp =
            _NEW( MachLogRefillLandMinesOperation( &pActor->asSpyLocator() ) );

        //Give to actor
        pActor->newOperation( pOp );
        canDo = true;

		if( not hasPlayedVoiceMail() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::SPY_LOCATOR, pActor->subType(), MachLogMachineVoiceMailManager::MEV_TASKED, pActor->id(),  pActor->race() );
			hasPlayedVoiceMail( true );
		}

    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiRefillLandMineCommand::cursorOnTerrain( const MexPoint3d&, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiRefillLandMineCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
void MachGuiRefillLandMineCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiRefillLandMineCommand::clone() const
{
    return _NEW( MachGuiRefillLandMineCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiRefillLandMineCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/refill.bmp", "gui/commands/refill.bmp" );
    return names;
}

//virtual
uint MachGuiRefillLandMineCommand::cursorPromptStringId() const
{
    return IDS_REFILLLANDMINE_COMMAND;
}

//virtual
uint MachGuiRefillLandMineCommand::commandPromptStringid() const
{
    return IDS_REFILLLANDMINE_START;
}

//virtual
bool MachGuiRefillLandMineCommand::canAdminApply() const
{
	return false;
}

//virtual
bool MachGuiRefillLandMineCommand::doAdminApply( MachLogAdministrator* /*pAdministrator*/, string* )
{
    PRE( canAdminApply() );;
	return false;

}

//virtual
bool MachGuiRefillLandMineCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_G and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDLOCTO.CPP **************************************************/
