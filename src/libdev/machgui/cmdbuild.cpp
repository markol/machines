/*
 * C M D B U I L D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdbuild.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"
#include "machlog/machlog.hpp"
#include "ctl/pvector.hpp"

MachGuiBuildCommand::MachGuiBuildCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiBuildCommand::~MachGuiBuildCommand()
{
    TEST_INVARIANT;

}

void MachGuiBuildCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiBuildCommand& t )
{

    o << "MachGuiBuildCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiBuildCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiBuildCommand::pickOnTerrain( const MexPoint3d&, bool, bool, bool )
{
    //Nothing to do
}

//virtual
void MachGuiBuildCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool, bool
)
{
    //Usual selection of the actor
    if( ctrlPressed )
    {
        if( inGameScreen().isSelected( *pActor ) )
            inGameScreen().deselect( pActor );
        else
            inGameScreen().select( pActor );
    }
    else
    {
        inGameScreen().deselectAll();
        inGameScreen().select( pActor );
    }
}

//virtual
bool MachGuiBuildCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Factories can build
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::FACTORY;
}

//virtual
bool MachGuiBuildCommand::isInteractionComplete() const
{
    //Is complete if the selection set no longer consists of single factory
    return not actorsCanExecute();
}

//virtual
bool MachGuiBuildCommand::doApply( MachActor*, string* )
{
    //This command interaction is all via the menu icons, so this function is not called
    return true;
}

//virtual
MachGui::Cursor2dType MachGuiBuildCommand::cursorOnTerrain( const MexPoint3d&, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiBuildCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::SELECT_CURSOR;

    return cursor;
}

//virtual
void MachGuiBuildCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiBuildCommand::clone() const
{
    return _NEW( MachGuiBuildCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiBuildCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/const.bmp", "gui/commands/const.bmp" );
    return names;
}

bool MachGuiBuildCommand::actorsCanExecute() const
{
    //Check there is exactly one selected entity, and it is a factory
    const MachInGameScreen::Actors& selectionSet = inGameScreen().selectedActors();
    return selectionSet.size() == 1  and
           selectionSet.front()->objectType() == MachLog::FACTORY;
}

void MachGuiBuildCommand::start()
{
    //Cache the current control panel context
    previousMenuContext_ = inGameScreen().currentContext();

    //Enter the build menu context
    inGameScreen().buildCommandContext();
}

void MachGuiBuildCommand::finish()
{
    //If still in the build command context, restore the cached context
    if( inGameScreen().isBuildCommandContext() )
        inGameScreen().currentContext( previousMenuContext_ );
}

//virtual
uint MachGuiBuildCommand::cursorPromptStringId() const
{
    return IDS_BUILD_COMMAND;
}

//virtual
uint MachGuiBuildCommand::commandPromptStringid() const
{
    return IDS_BUILD_START;
}

//virtual
bool MachGuiBuildCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_B and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDBUILD.CPP **************************************************/
