/*
 * C M D R E P A R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdrepar.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"

#include "ctl/algorith.hpp"

#include "system/pathname.hpp"

#include "world4d/domain.hpp"

#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/machvman.hpp"
#include "machlog/constron.hpp"
#include "machlog/opadsupc.hpp"
#include "machlog/opsupcon.hpp"
#include "machlog/squad.hpp"

#include "device/butevent.hpp"

#include "machphys/objdata.hpp"

MachGuiRepairCommand::MachGuiRepairCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false )
{
    TEST_INVARIANT;
}

MachGuiRepairCommand::~MachGuiRepairCommand()
{
    TEST_INVARIANT;

    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );

	while( not constructions_.empty() )
	{
		constructions_.back()->detach( this );
		constructions_.pop_back();
	}
}

void MachGuiRepairCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiRepairCommand& t )
{

    o << "MachGuiRepairCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiRepairCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiRepairCommand::pickOnTerrain
(
    const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool /*shiftPressed*/, bool /*altPressed*/
)
{}

//virtual
void MachGuiRepairCommand::pickOnActor
(
    MachActor* pActor, bool, bool shiftPressed, bool
)
{
    //Check for a pick on construction
    if( pActor->objectIsConstruction() and 	// It's a construction
    	pActor->asConstruction().isComplete() and // It's been built
    	pActor->hp() < pActor->objectData().hitPoints() ) // It's damaged
    {
		MachLogConstruction* pCandidateConstruction = &pActor->asConstruction();

		if( not constructionIsDuplicate( pCandidateConstruction ) )
		{
			// Add to list of constructions to repair
        	constructions_.push_back( pCandidateConstruction );

			pCandidateConstruction->attach( this );
		}


		if ( not shiftPressed )
		{
		  	hadFinalPick_ = true;
		}
		else
		{
			// Waypoint click (i.e. not final click)
			MachGuiSoundManager::instance().playSound( "gui/sounds/waypoint.wav" );
		}
    }
}

bool MachGuiRepairCommand::constructionIsDuplicate( const MachLogConstruction* pCandidateConstruction ) const
{
	ctl_pvector< MachLogConstruction >::const_iterator i = find( constructions_.begin(), constructions_.end(), pCandidateConstruction );

	return i != constructions_.end();
}

//virtual
bool MachGuiRepairCommand::canActorEverExecute( const MachActor& actor ) const
{
    // Constructors can repair
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::CONSTRUCTOR;
}

//virtual
bool MachGuiRepairCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
MachGui::Cursor2dType MachGuiRepairCommand::cursorOnTerrain( const MexPoint3d& /*location*/, bool /*ctrlPressed*/, bool, bool )
{
    return MachGui::MENU_CURSOR;
}

//virtual
MachGui::Cursor2dType MachGuiRepairCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

	if ( pActor->objectIsConstruction() and // It's a construction
		 pActor->asConstruction().isComplete() and 	// It's been built
		 pActor->hp() < pActor->objectData().hitPoints() ) // It's damaged
	{
		cursor = MachGui::REPAIR_CURSOR;
	}

    return cursor;
}

//virtal
void MachGuiRepairCommand::typeData( MachLog::ObjectType /*objectType*/, int /*subType*/, uint /*level*/ )
{}

//virtual
bool MachGuiRepairCommand::doApply( MachActor* pActor, string* )
{
	PRE( hadFinalPick_ );

    //Create a superconstruct(repair) operation for the constructor
    MachLogSuperConstructOperation* pOp =
        _NEW( MachLogSuperConstructOperation( &pActor->asConstructor(), constructions_, MachLogOperation::REPAIR_OPERATION ) );
    pActor->newOperation( pOp );

	ASSERT( pActor->objectIsMachine(), "Hey! That actor should have been a machine!" );
   	pActor->asMachine().manualCommandIssued();

	if( not hasPlayedVoiceMail() )
	{
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pActor->subType(), MachLogMachineVoiceMailManager::MEV_MOVE_TO_SITE, pActor->id(),  pActor->race() );
		hasPlayedVoiceMail( true );
	}

    return true;

}

//virtual
MachGuiCommand* MachGuiRepairCommand::clone() const
{
    return _NEW( MachGuiRepairCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiRepairCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/repair.bmp", "gui/commands/repair.bmp" );
    return names;
}

//virtual
void MachGuiRepairCommand::start()
{}

//virtual
void MachGuiRepairCommand::finish()
{}

//virtual
uint MachGuiRepairCommand::cursorPromptStringId() const
{
    return IDS_REPAIR_COMMAND;
}

//virtual
uint MachGuiRepairCommand::commandPromptStringid() const
{
    return IDS_REPAIR_START;
}

//virtual
bool MachGuiRepairCommand::canAdminApply() const
{
    return true;
}

//virtual
bool MachGuiRepairCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );

    //Create an admin superconstruct(repair) operation for the administrator
	MachLogAdminSuperConstructOperation* pOp =
        _NEW( MachLogAdminSuperConstructOperation( pAdministrator, constructions_, MachLogOperation::REPAIR_OPERATION ) );

	pAdministrator->newOperation( pOp );
	ASSERT( pAdministrator->squadron(), "Administrator didn't have a squadron!" );
	pAdministrator->squadron()->manualCommandIssuedToSquadron();

	MachActor* pFirstConstructor = NULL;

	bool found = false;
	for( MachInGameScreen::Actors::const_iterator i = inGameScreen().selectedActors().begin(); not found and i != inGameScreen().selectedActors().end(); ++i )
		if( (*i)->objectType() == MachLog::CONSTRUCTOR )
		{
			found = true;
			pFirstConstructor = (*i);
		}

	ASSERT( found, "No constructor found in corral!" );

	// give out voicemail
	MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pFirstConstructor->subType(), MachLogMachineVoiceMailManager::MEV_MOVE_TO_SITE, pFirstConstructor->id(),  pFirstConstructor->race() );

    return true;
}

//virtual
bool MachGuiRepairCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_R and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

//virtual
bool MachGuiRepairCommand::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
	{
		ctl_pvector< MachLogConstruction >::iterator i = find( constructions_.begin(), constructions_.end(), pSubject );
		if( i != constructions_.end() )
		{
			// one of our constructions has been destroyed
			stayAttached = false;
			constructions_.erase( i );
		}
	}
	break;

	default:
		;
	}

	return stayAttached;
}


//virtual
void MachGuiRepairCommand::domainDeleted( W4dDomain*  )
{
	//inentionally empty...override as necessary
}


/* End CMDREPAR.CPP **************************************************/
