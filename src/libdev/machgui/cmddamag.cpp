/*
 * C M D R E F I L L  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "machgui/cmddamag.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "device/butevent.hpp"
#include "machlog/actor.hpp"

MachGuiDamageCommand::MachGuiDamageCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( true )
{
    TEST_INVARIANT;
}

MachGuiDamageCommand::~MachGuiDamageCommand()
{
    TEST_INVARIANT;

}

void MachGuiDamageCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDamageCommand& t )
{

    o << "MachGuiDamageCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiDamageCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiDamageCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{

}

//virtual
void MachGuiDamageCommand::pickOnActor
(
    MachActor*, bool, bool, bool
)
{
    //Ignored
}

//virtual
bool MachGuiDamageCommand::canActorEverExecute( const MachActor& actor ) const
{
	return actor.objectIsMachine() or actor.objectIsConstruction();
}

//virtual
bool MachGuiDamageCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiDamageCommand::doApply( MachActor* pActor, string* /*pReason*/ )
{
    PRE( pActor->objectIsMachine() or pActor->objectIsConstruction() );

    //Check locator type
    bool canDo = true;
    pActor->beHit( ( pActor->hp() / 2 ) );

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiDamageCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiDamageCommand::cursorOnActor( MachActor*, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    return cursor;
}

//virtual
void MachGuiDamageCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiDamageCommand::clone() const
{
    return _NEW( MachGuiDamageCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiDamageCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/damage.bmp", "gui/commands/damage.bmp" );
    return names;
}

//virtual
uint MachGuiDamageCommand::cursorPromptStringId() const
{
    return IDS_SELF_DESTRUCT_COMMAND;
}

//virtual
uint MachGuiDamageCommand::commandPromptStringid() const
{
    return IDS_SELF_DESTRUCT_START;
}

//virtual
bool MachGuiDamageCommand::canAdminApply() const
{
	return false;
}

/* End CMDLOCTO.CPP **************************************************/
