/*
 * C M D A S S E M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdassem.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "sound/sndparam.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "world4d/domain.hpp"
#include "machlog/actor.hpp"
#include "machlog/factory.hpp"
#include "machphys/mcmovinf.hpp"
#include "device/butevent.hpp"

MachGuiAssemblyPointCommand::MachGuiAssemblyPointCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
	interactionComplete_( false )
{
    TEST_INVARIANT;
}

MachGuiAssemblyPointCommand::~MachGuiAssemblyPointCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );
}

void MachGuiAssemblyPointCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiAssemblyPointCommand& t )
{

    o << "MachGuiAssemblyPointCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiAssemblyPointCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiAssemblyPointCommand::pickOnTerrain
(
    const MexPoint3d& location, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{
	if ( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
	{
		interactionComplete_ = true;
		assemblyPoint_ = location;
	}
}

//virtual
void MachGuiAssemblyPointCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
	interactionComplete_ = ( cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) != MachGui::INVALID_CURSOR );
	assemblyPoint_ = pActor->globalTransform().position();
}

//virtual
bool MachGuiAssemblyPointCommand::canActorEverExecute( const MachActor& actor ) const
{
	// All factories can specify an assembly point
	return actor.objectType() == MachLog::FACTORY;
}

//virtual
bool MachGuiAssemblyPointCommand::isInteractionComplete() const
{
    return interactionComplete_;
}

//virtual
MachGui::Cursor2dType MachGuiAssemblyPointCommand::cursorOnTerrain( const MexPoint3d& location, bool /*ctrlPressed*/, bool, bool )
{
	if ( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
    	return MachGui::ASSEMBLEPOINT_CURSOR;

	return MachGui::MENU_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiAssemblyPointCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
	if ( pActor->objectIsConstruction() )
	{
		return MachGui::INVALID_CURSOR;
	}
    return MachGui::ASSEMBLEPOINT_CURSOR;
}

//virtal
void MachGuiAssemblyPointCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiAssemblyPointCommand::doApply( MachActor* pActor, string* )
{
	pActor->asFactory().specifiedDeployPoint( assemblyPoint_ );

	return true;
}

//virtual
MachGuiCommand* MachGuiAssemblyPointCommand::clone() const
{
    return _NEW( MachGuiAssemblyPointCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiAssemblyPointCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/assemble.bmp", "gui/commands/assemble.bmp" );
    return names;
}

//virtual
void MachGuiAssemblyPointCommand::start()
{}

//virtual
void MachGuiAssemblyPointCommand::finish()
{}

//virtual
uint MachGuiAssemblyPointCommand::cursorPromptStringId() const
{
    return IDS_ASSEMBLEAT_COMMAND;
}

//virtual
uint MachGuiAssemblyPointCommand::commandPromptStringid() const
{
    return IDS_ASSEMBLEAT_START;
}

//virtual
bool MachGuiAssemblyPointCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiAssemblyPointCommand::doAdminApply( MachLogAdministrator* /*pAdministrator*/, string* )
{
	ASSERT( false, "no admin assembly point command" );

    return true;
}

//virtual
bool MachGuiAssemblyPointCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_B and be.action() == DevButtonEvent::PRESS )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDASSEM.CPP **************************************************/
