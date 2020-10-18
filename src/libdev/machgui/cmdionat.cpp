/*
 * C M D I O N A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdionat.hpp"
#include "machgui/ingame.hpp"
#include "device/butevent.hpp"
#include "machphys/machphys.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/actor.hpp"
#include "machlog/attack.hpp"
#include "machlog/missilem.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/pod.hpp"
#include "machlog/weapon.hpp"
#include "mathex/transf3d.hpp"
#include "ctl/pvector.hpp"

MachGuiIonAttackCommand::MachGuiIonAttackCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    action_( DO_NOTHING ),			// this default is for the case where no ion cannons are charged
    pDirectObject_( NULL ),
    hadFinalPick_( not atLeastOneActorHasChargedIonCannon() )
{
    TEST_INVARIANT;
}

MachGuiIonAttackCommand::~MachGuiIonAttackCommand()
{
    TEST_INVARIANT;

}

void MachGuiIonAttackCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiIonAttackCommand& t )
{

    o << "MachGuiIonAttackCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiIonAttackCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiIonAttackCommand::pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
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
void MachGuiIonAttackCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    hadFinalPick_ = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed ) !=
                        MachGui::INVALID_CURSOR;
}

//virtual
bool MachGuiIonAttackCommand::canActorEverExecute( const MachActor& actor ) const
{
	//this function has multiple exit points

	if( actor.objectType() == MachLog::POD and actor.asPod().hasIonCannon() )// ionCannonReady() )
		return true;
	else
		return false;
}

//virtual
bool MachGuiIonAttackCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
bool MachGuiIonAttackCommand::doApply( MachActor* pActor, string* pReason )
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

bool MachGuiIonAttackCommand::applyAttackLocation( MachActor* pActor, string* )
{
	ASSERT_INFO( *pActor );
	ASSERT( pActor->objectType() == MachLog::POD, "A non-pod actor has somehow been allowed to initiate an ion-attack op." );

	MexPoint2d validPoint;
	bool valid = pActor->asPod().ionCannonReady()
				 and findClosestPointValidOnTerrain(location_,
												pActor->globalTransform().position(),
				 							    IGNORE_SELECTED_ACTOR_OBSTACLES,
				 					    		0.1, // Clearance
												5,	 // Step distance
				 					    		&validPoint );

	if ( valid )
	{
		 //Construct appropriate type of operation
	    MachLogOperation* pOp = _NEW( MachLogPodAttackOperation( &pActor->asPod(), validPoint ) );

	    //Give it to the actor
	    pActor->newOperation( pOp );
	}

    return true;
}

bool MachGuiIonAttackCommand::applyAttackObject( MachActor* pActor, string* )
{
	ASSERT_INFO( *pActor );
	ASSERT( pActor->objectType() == MachLog::POD, "A non-pod actor has somehow been allowed to initiate an ion-attack op." );

    bool canDo = pActor->asPod().ionCannonReady();

    if( canDo )
    {
		ASSERT_INFO( *pActor );
		ASSERT( pActor->objectType() == MachLog::POD, "A non-pod actor has somehow been allowed to initiate an ion-attack op." );

        //Construct appropriate type of operation
        MachLogOperation* pOp = _NEW( MachLogPodAttackOperation( &pActor->asPod(), pDirectObject_ ) );

        //Give it to the actor
        pActor->newOperation( pOp );
    }

    return canDo;
}

//virtual
MachGui::Cursor2dType MachGuiIonAttackCommand::cursorOnTerrain( const MexPoint3d& location, bool, bool, bool )
{
 	MachGui::Cursor2dType cursorType = MachGui::MENU_CURSOR;

	if( atLeastOneActorHasChargedIonCannon() )
    {
	    cursorType = MachGui::INVALID_CURSOR;

	    if( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_ALL_ACTOR_OBSTACLES ) )
	    {
	    	action_ = ATTACK_LOCATION;
	        cursorType = MachGui::ION_ATTACK_CURSOR;
	        location_ = location;
	    }
	}

    return cursorType;
}

//virtual
MachGui::Cursor2dType MachGuiIonAttackCommand::cursorOnActor( MachActor* pActor, bool, bool, bool )
{
	MachGui::Cursor2dType cursorType = MachGui::MENU_CURSOR;

    if( atLeastOneActorHasChargedIonCannon() )
    {
	    //Set the attack object action
        action_ = ATTACK_OBJECT;
        cursorType = MachGui::ION_ATTACK_CURSOR;
        pDirectObject_ = pActor;
	}

    return cursorType;
}

//virtual
void MachGuiIonAttackCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiIonAttackCommand::clone() const
{
    return _NEW( MachGuiIonAttackCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiIonAttackCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/ionattk.bmp", "gui/commands/ionattk.bmp" );
    return names;
}

//virtual
uint MachGuiIonAttackCommand::cursorPromptStringId() const
{
    return IDS_ION_ATTACK_COMMAND;
}

//virtual
uint MachGuiIonAttackCommand::commandPromptStringid() const
{
    return IDS_ION_ATTACK_START;
}

//virtual
bool MachGuiIonAttackCommand::processButtonEvent( const DevButtonEvent& be )
{
	if ( isVisible() and be.scanCode() == DevKey::KEY_I and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		return true;
	}

	return false;
}

//static
void MachGuiIonAttackCommand::update( const Actors& actors )
{
	// Work out how recharged the ion cannons are
	int nHighestPercentageRecharged = 0;

	for ( Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		if ( (*iter)->objectType() == MachLog::POD )
		{
			if ( (*iter)->asPod().hasIonCannon() )
			{
				int percentageRecharged = (*iter)->asPod().weapons().front()->percentageRecharge();

				if ( percentageRecharged > nHighestPercentageRecharged )
				{
					nHighestPercentageRecharged = percentageRecharged;
				}
			}
		}
	}

	highestPercentageRecharged() = 	nHighestPercentageRecharged;
}

//static
int& MachGuiIonAttackCommand::highestPercentageRecharged()
{
	static int nHighestPercentageRecharged = 0;

	return nHighestPercentageRecharged;
}

bool MachGuiIonAttackCommand::atLeastOneActorHasChargedIonCannon() const
{
	bool noneHasChargedCannon = true;

	for( MachInGameScreen::Actors::const_iterator iter = inGameScreen().selectedActors().begin();
		 iter != inGameScreen().selectedActors().end() and noneHasChargedCannon;
		 ++iter )
	{
		MachActor& actor = (**iter);

		if( actor.objectType() == MachLog::POD
			and actor.asPod().ionCannonReady() )
		{
			noneHasChargedCannon = false;
		}
	}

	return not( noneHasChargedCannon );
}

/* End CMDIONAT.CPP **************************************************/
