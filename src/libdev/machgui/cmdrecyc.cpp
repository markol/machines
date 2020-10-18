/*
 * C M D R E C Y C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdrecyc.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/strings.hpp"
#include "world4d/domain.hpp"
#include "machlog/actor.hpp"
#include "machlog/machvman.hpp"
#include "machlog/smelter.hpp"
#include "machlog/oprecycl.hpp"
#include "device/butevent.hpp"

MachGuiRecycleCommand::MachGuiRecycleCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false ),
	pSmeltingBuilding_( NULL )
{
    TEST_INVARIANT;
}

MachGuiRecycleCommand::~MachGuiRecycleCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );
}

void MachGuiRecycleCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiRecycleCommand& t )
{

    o << "MachGuiRecycleCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiRecycleCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiRecycleCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
void MachGuiRecycleCommand::pickOnActor
(
    MachActor* pActor, bool, bool, bool
)
{
    //Check for a pick on construction
	//note that use of static cast relies on short-circuiting of AND operator
    if( pActor->objectIsCanSmelt() and pActor->asConstruction().isComplete() )
    {
    	hadFinalPick_ = true;
		pSmeltingBuilding_ = &pActor->asConstruction();
    }
	else
   		pSmeltingBuilding_ = NULL;
}

//virtual
bool MachGuiRecycleCommand::canActorEverExecute( const MachActor& actor ) const
{
    //All machine types can be recycled
    return actor.objectIsMachine();
}

//virtual
bool MachGuiRecycleCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
MachGui::Cursor2dType MachGuiRecycleCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool, bool )
{
    return MachGui::MENU_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiRecycleCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

	if ( pActor->objectIsCanSmelt() and ( _STATIC_CAST( MachLogConstruction*, pActor ) )->isComplete() )
	{
		cursor = MachGui::RECYCLE_CURSOR;
	}

    return cursor;
}

//virtal
void MachGuiRecycleCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiRecycleCommand::doApply( MachActor* pActor, string* )
{
    ASSERT( pSmeltingBuilding_ != NULL, "pSmeltingBuilding_ should not have a NULL assignment if MachGuiRecycleCommand::doApply is called" );

	MachLogRecycleOperation* pOp =
	    _NEW( MachLogRecycleOperation( &pActor->asMachine(), pSmeltingBuilding_ ) );

	pActor->newOperation( pOp );

	if( not hasPlayedVoiceMail() )
	{
		MachLogMachineVoiceMailManager::instance().postNewMail( pActor->objectType(), pActor->subType(), MachLogMachineVoiceMailManager::MEV_RECYCLE, pActor->id(),  pActor->race() );
		hasPlayedVoiceMail( true );
	}

	return true;

}

//virtual
MachGuiCommand* MachGuiRecycleCommand::clone() const
{
    return _NEW( MachGuiRecycleCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiRecycleCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/recycle.bmp", "gui/commands/recycle.bmp" );
    return names;
}

//virtual
void MachGuiRecycleCommand::start()
{}

//virtual
void MachGuiRecycleCommand::finish()
{}

//virtual
uint MachGuiRecycleCommand::cursorPromptStringId() const
{
    return IDS_RECYCLE_COMMAND;
}

//virtual
uint MachGuiRecycleCommand::commandPromptStringid() const
{
    return IDS_RECYCLE_START;
}

//virtual
bool MachGuiRecycleCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiRecycleCommand::doAdminApply( MachLogAdministrator* /*pAdministrator*/, string* )
{
    PRE( canAdminApply() );

    return false;
}


//virtual
bool MachGuiRecycleCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_Q and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

/* End CMDRECYC.CPP **************************************************/
