/*
 * C M D F R M S Q . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdfrmsq.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/machlog.hpp"


MachGuiFormSquadronCommand::MachGuiFormSquadronCommand( MachInGameScreen* pInGameScreen )
:   MachGuiDefaultCommand( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiFormSquadronCommand::~MachGuiFormSquadronCommand()
{
    TEST_INVARIANT;

}

void MachGuiFormSquadronCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiFormSquadronCommand& t )
{

    o << "MachGuiFormSquadronCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiFormSquadronCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachGuiFormSquadronCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Machines can be formed into squadrons
    return actor.objectIsMachine() and
           inGameScreen().currentContext() == MachGui::MAIN_MENU;
}

//virtual
bool MachGuiFormSquadronCommand::isInteractionComplete() const
{
    return false;
}

//virtual
bool MachGuiFormSquadronCommand::doApply( MachActor*, string* )
{
    //This command interaction is all via the menu icons, so this function is not called
    return true;
}

//virtual
void MachGuiFormSquadronCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiFormSquadronCommand::clone() const
{
    return _NEW( MachGuiFormSquadronCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiFormSquadronCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/formsqad.bmp", "gui/commands/formsqad.bmp" );
    return names;
}

//virtual
void MachGuiFormSquadronCommand::start()
{
    //Cache the current control panel context
    previousMenuContext_ = inGameScreen().currentContext();

    //Enter the form squadron command context
    inGameScreen().currentContext( MachGui::FORM_SQUADRON_COMMAND );
}

//virtual
void MachGuiFormSquadronCommand::finish()
{
    //If still in the form squadron command context, restore the cached context
    if( inGameScreen().currentContext() == MachGui::FORM_SQUADRON_COMMAND )
        inGameScreen().currentContext( previousMenuContext_ );
}

//virtual
uint MachGuiFormSquadronCommand::cursorPromptStringId() const
{
    return IDS_FORM_SQUADRON_COMMAND;
}

//virtual
uint MachGuiFormSquadronCommand::commandPromptStringid() const
{
    return IDS_FORM_SQUADRON_START;
}

//virtual
bool MachGuiFormSquadronCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_F and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

//virtual
bool MachGuiFormSquadronCommand::actorsCanExecute() const
{
	return MachGuiCommand::actorsCanExecute();
}

/* End CMDFRMSQ.CPP **************************************************/
