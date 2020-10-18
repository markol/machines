/*
 * C M D I O N A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdnukat.hpp"


#include "ctl/pvector.hpp"

#include "mathex/transf3d.hpp"

#include "device/butevent.hpp"

#include "machphys/genedata.hpp"
#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/actor.hpp"
#include "machlog/attack.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/missilem.hpp"
#include "machlog/opnukeat.hpp"
#include "machlog/races.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"
#include "machlog/weapon.hpp"

#include "machgui/ingame.hpp"

#include "machgui/internal/strings.hpp"

MachGuiNukeAttackCommand::MachGuiNukeAttackCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    action_( DO_NOTHING ),
    pDirectObject_( NULL ),
    hadFinalPick_( not atLeastOneActorHasChargedNuke() )
{
    TEST_INVARIANT;
}

MachGuiNukeAttackCommand::~MachGuiNukeAttackCommand()
{
    TEST_INVARIANT;

}

void MachGuiNukeAttackCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiNukeAttackCommand& t )
{

    o << "MachGuiNukeAttackCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiNukeAttackCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiNukeAttackCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed )
{
    //Check the location is on the ground - not up a hill
    if( cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed ) !=
        MachGui::INVALID_CURSOR )
    {
        //Store the location and set the action
        action_ = ATTACK_LOCATION;
        location_ = location;
        hadFinalPick_ = true;
    }
}

//virtual
void MachGuiNukeAttackCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    hadFinalPick_ = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) !=
                        MachGui::INVALID_CURSOR;
}

//virtual
bool MachGuiNukeAttackCommand::canActorEverExecute( const MachActor& actor ) const
{
	//this function has multiple exit points
  	if	(	actor.objectType() == MachLog::MISSILE_EMPLACEMENT and
			actor.asMissileEmplacement().isNukeSilo() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//virtual
bool MachGuiNukeAttackCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiNukeAttackCommand::doApply( MachActor* pActor, string* pReason )
{
    PRE( pActor->objectIsCanAttack() );

    //Take appropriate action
    bool canDo = false;
    switch( action_ )
    {
        case ATTACK_LOCATION:
            canDo = applyAttackLocation( pActor, pReason );
            break;

        case ATTACK_OBJECT:
            canDo = applyAttackObject( pActor, pReason );
            break;

		case DO_NOTHING:
			canDo = true;
			break;

        default:
            ASSERT_BAD_CASE;
    }

    return canDo;
}

bool MachGuiNukeAttackCommand::applyAttackLocation( MachActor* pActor, string* )
{
	ASSERT_INFO( *pActor );
	ASSERT(  pActor->objectType() == MachLog::MISSILE_EMPLACEMENT
			 and
			 pActor->asMissileEmplacement().isNukeSilo(), "A non-ICBM has somehow been allowed to initiate a nuclear attack." );

	MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();
	MachPhys::BuildingMaterialUnits	nukeLaunchCost = MachPhysData::instance().generalData().nukeLaunchCost();

	bool sufficientBMUsForLaunch = races.nBuildingMaterialUnits( playerRace ) >= nukeLaunchCost;

	bool canDo = false;

	if( sufficientBMUsForLaunch )
	{

		MexPoint2d validPoint;
		canDo = pActor->asMissileEmplacement().nukeReady()
				and findClosestPointValidOnTerrain(location_,
													pActor->globalTransform().position(),
					 							    IGNORE_ALL_ACTOR_OBSTACLES,
					 					    		0.1, // Clearance
													5,	 // Step distance
					 					    		&validPoint );

		if ( canDo )
		{
			//Construct appropriate type of operation
		    MachLogOperation* pOp = _NEW( MachLogNukeAttackOperation( &pActor->asMissileEmplacement(), validPoint ) );

		    //Give it to the actor
		    pActor->newOperation( pOp );

			// deduct the dosh
			races.smartSubtractBMUs( playerRace, nukeLaunchCost );
		}
	}
	else
	{
		giveNotEnoughBMUsVoicemail();
	}

	return canDo;
}

bool MachGuiNukeAttackCommand::applyAttackObject( MachActor* pActor, string* )
{
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();
	MachPhys::BuildingMaterialUnits	nukeLaunchCost = MachPhysData::instance().generalData().nukeLaunchCost();

	bool sufficientBMUsForLaunch = races.nBuildingMaterialUnits( playerRace ) >= nukeLaunchCost;

	bool canDo = false;

	if( sufficientBMUsForLaunch )
	{
	    bool canDo =  pActor->asMissileEmplacement().nukeReady()
	    			  and pActor != pDirectObject_;		// check not trying to attack oneself.
	    if( canDo )
	    {
			ASSERT_INFO( *pActor );
	      	ASSERT(  pActor->objectType() == MachLog::MISSILE_EMPLACEMENT
				 	 and
				 	 pActor->asMissileEmplacement().isNukeSilo(), "A non-ICBM has somehow been allowed to initiate a nuclear attack." );

			//Construct appropriate type of operation
	        MachLogOperation* pOp = _NEW( MachLogNukeAttackOperation( &pActor->asMissileEmplacement(), pDirectObject_ ) );

			//Give it to the actor
		    pActor->newOperation( pOp );

			// deduct the dosh
			races.smartSubtractBMUs( playerRace, nukeLaunchCost );
	    }
	}
	else
	{
		giveNotEnoughBMUsVoicemail();
	}

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiNukeAttackCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
    MachGui::Cursor2dType cursorType = MachGui::MENU_CURSOR;

	if( atLeastOneActorHasChargedNuke() )
	{
		cursorType = MachGui::INVALID_CURSOR;

		if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_ALL_ACTOR_OBSTACLES ) )
	    {
	    	action_ = ATTACK_LOCATION;
	        cursorType = MachGui::NUKE_ATTACK_CURSOR;
	        location_ = location;
	    }
	}

    return cursorType;
}

//virtual
MachGui::Cursor2dType MachGuiNukeAttackCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
    MachGui::Cursor2dType cursorType = MachGui::MENU_CURSOR;

	if( atLeastOneActorHasChargedNuke() )
	{
		//Set the attack object action
        action_ = ATTACK_OBJECT;
        cursorType = MachGui::NUKE_ATTACK_CURSOR;
        pDirectObject_ = pActor;
	}

    return cursorType;
}

//virtual
void MachGuiNukeAttackCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiNukeAttackCommand::clone() const
{
    return _NEW( MachGuiNukeAttackCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiNukeAttackCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/nukeattk.bmp", "gui/commands/nukeattk.bmp" );
    return names;
}

//virtual
uint MachGuiNukeAttackCommand::cursorPromptStringId() const
{
    return IDS_NUKE_ATTACK_COMMAND;
}

//virtual
uint MachGuiNukeAttackCommand::commandPromptStringid() const
{
    return IDS_NUKE_ATTACK_START;
}

//virtual
bool MachGuiNukeAttackCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_N and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

//static
void MachGuiNukeAttackCommand::update( const Actors& actors )
{
	// Work out how recharged the nukes are
	int nHighestPercentageRecharged = 0;

	for ( Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		if ( (*iter)->objectType() == MachLog::MISSILE_EMPLACEMENT and
			 (*iter)->asMissileEmplacement().isNukeSilo() )
		{
			int percentageRecharged = (*iter)->asMissileEmplacement().weapons().front()->percentageRecharge();

			if ( percentageRecharged > nHighestPercentageRecharged )
			{
				nHighestPercentageRecharged = percentageRecharged;
			}
		}
	}

	highestPercentageRecharged() = 	nHighestPercentageRecharged;
}

//static
int& MachGuiNukeAttackCommand::highestPercentageRecharged()
{
	static int nHighestPercentageRecharged = 0;

	return nHighestPercentageRecharged;
}

void MachGuiNukeAttackCommand::giveNotEnoughBMUsVoicemail() const
{
	MachLogVoiceMailManager::instance().postNewMail( VID_POD_INSUFFICIENT_CASH_FOR_NUKE, MachLogRaces::instance().pcController().race() );
}

bool MachGuiNukeAttackCommand::atLeastOneActorHasChargedNuke() const
{
	bool noneHasChargedNuke = true;

	for( MachInGameScreen::Actors::const_iterator iter = inGameScreen().selectedActors().begin();
		 iter != inGameScreen().selectedActors().end() and noneHasChargedNuke;
		 ++iter )
	{
		MachActor& actor = (**iter);

		if( actor.objectIsMissileEmplacement() )
		{
			const MachLogMissileEmplacement& missileEmp = actor.asMissileEmplacement();
			if( missileEmp.isNukeSilo()
				and missileEmp.nukeReady() )
			{
				noneHasChargedNuke = false;
			}
		}
	}

	return not( noneHasChargedNuke );
}

// Forced recompile 19/2/99 CPS
/* End CMDNUKAT.CPP **************************************************/
