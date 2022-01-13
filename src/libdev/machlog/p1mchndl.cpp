/*
 * P 1 M C H N D L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/p1mchndl.hpp"

#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/utility.hpp"
#include "phys/motchunk.hpp"
#include "phys/cspace2.hpp"
#include "world4d/link.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/domain.hpp"
#include "sim/manager.hpp"
#include "machphys/machine.hpp"
#include "machphys/p1mcdriv.hpp"
#include "machphys/station.hpp"

#include "machlog/machine.hpp"
#include "machlog/planet.hpp"
#include "machlog/canattac.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/strategy.hpp"
#include "machlog/presspad.hpp"
#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"
#include "machlog/camera.hpp"

#include "machlog/MachLog1stPersonActiveSquad.hpp"

// forward declarations
class MachLogSquadron;

class MachLog1stPersonMachineHandlerData
{
public:
    //data members
    MachLogMachine* pMachine_; //the machine being controlled
	MachLogSquadron* pOriginalSquadron_;
    bool isFirstUpdate_; //Used to detect first update call

    // First Person Command
    MachLog1stPersonActiveSquadron activeSquadron_;
};

MachLog1stPersonMachineHandler::MachLog1stPersonMachineHandler
(
    MachLogMachine* pMachine, MachLog1stPersonHandler::NetworkType networkType
)
:   MachLog1stPersonHandler( pMachine, pNewPhysDriver( pMachine, networkType ), networkType )
{
	NETWORK_STREAM("MachLog1stPersonMachineHandler::MachLog1stPersonMachineHandler\n" );
	NETWORK_INDENT( 2 );
    pData_ = _NEW( MachLog1stPersonMachineHandlerData );
    pData_->isFirstUpdate_ = true;

    pData_->pMachine_ = pMachine;

	// if this machine is the member of a squadron, needs to be removed from that squadron for the
	// duration of its time in first person, and restored when the first person control has ended.
	pData_->pOriginalSquadron_ = pMachine->squadron();

	if( pData_->pOriginalSquadron_ )
	{
		pData_->pMachine_->squadron( NULL );

        // Provide the machine's squadron as the active one for FP Command
        pData_->activeSquadron_ = MachLog1stPersonActiveSquadron{ pData_->pOriginalSquadron_ };
	}

    //Cancel current operations
    stopStrategy();

    //Stop dead
    pMachine->motionSeq().stop();

    //Flag actor in 1st person
    pMachine->isIn1stPersonView( true );
    pMachine->motionSeq().is1stPersonControlled( true );
	pMachine->pFirstPersonMachineHandler( this );

    //Open any handlers on remote nodes in a network game
    setupRemoteHandlers();
	NETWORK_STREAM("machine motion sequqncer now is:\n" << pMachine->motionSeq() << std::endl );

	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachLog1stPersonMachineHandler::MachLog1stPersonMachineHandler DONE\n" );
    TEST_INVARIANT;
}

MachLog1stPersonMachineHandler::~MachLog1stPersonMachineHandler()
{
	NETWORK_STREAM("MachLog1stPersonMachineHandler::~MachLog1stPersonMachineHandler\n" );
	NETWORK_INDENT( 2 );
    TEST_INVARIANT;

    //Closed own any heandlers on remote nodes in a network game
    clearRemoteHandlers();

    //Clear the locally generated resting obstacle, and add a network wide consistent one
    MachLogMachine& machine = *pData_->pMachine_;
    machine.motionSeq().clearRestingObstacleWithoutEcho();
    machine.motionSeq().addRestingObstacle();

    //Flag actor not in 1st person
    machine.isIn1stPersonView( false );
    machine.motionSeq().is1stPersonControlled( false );
	machine.pFirstPersonMachineHandler( NULL );

	if( pData_->pOriginalSquadron_ )
	{
		pData_->pMachine_->squadron( pData_->pOriginalSquadron_ );
		pData_->pOriginalSquadron_ = NULL;
	}

    _DELETE( pDriver_ );
    _DELETE( pData_ );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachLog1stPersonMachineHandler::~MachLog1stPersonMachineHandler DONE\n" );
}

void MachLog1stPersonMachineHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( pData_ != NULL );
}

ostream& operator <<( ostream& o, const MachLog1stPersonMachineHandler& t )
{

    o << "MachLog1stPersonMachineHandler " << (void*)&t << " start" << std::endl;
    o << "MachLog1stPersonMachineHandler " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
W4dEntity& MachLog1stPersonMachineHandler::cameraAttachment( MexTransform3d* pOffsetTransform ) const
{
    return pDriver_->cameraAttachment( pOffsetTransform );
}

//virtual
void MachLog1stPersonMachineHandler::doUpdate()
{
    //On first update, do initial processing
    if( pData_->isFirstUpdate_ )
        firstUpdateProcessing();

    //Update the driver using current settings
    MachPhys1stPersonMachineDriver& driver = *pDriver_;

    if( not remotelyControlled() )
    {
        if( isToMoveForwards() )
            driver.moveForwards();
        else if( isToMoveBackwards() )
            driver.moveBackwards();
        else
            driver.stopMoving();

        if( isToTurnLeft() )
            driver.turnLeft();
        else if( isToTurnRight() )
            driver.turnRight();
        else
            driver.stopTurning();
    }

    MachLogMachine& machine = *pData_->pMachine_;
    MachLogMachineMotionSequencer& motionSequencer = machine.motionSeq();

	NETWORK_STREAM("MachLog1stPersonMachineHandler::doUpdate\n" );
	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );

    //Compute the new position if any for the machine, storing last update time.
    //It is possible for a network game resync to jump time back slightly. Hence may need to reduce startTime.
    PhysAbsoluteTime startTime = driver.lastUpdateTime();
    PhysAbsoluteTime now = W4dManager::instance().time();
    if( now < startTime )
        startTime = now;

    //Compute the new position at the current time
    MexTransform3d nextPosition;
    bool moveMachine = driver.nextPosition( &nextPosition );

    //Construct a clearance circle at the new position
    MexCircle2d nextPositionCircle( MexPoint2d( nextPosition.position() ), motionSequencer.highClearance() );

    //Check for being over a building entrance pressure pad
    MachLogConstruction* pEnteringConstruction = NULL;
    size_t entranceIndex;
    bool onEntrancePad =
        MachLogPlanet::instance().pressurePads().onPad( nextPositionCircle, &pEnteringConstruction, &entranceIndex );

    //determine if currently inside a building
    bool wasInside = machine.insideBuilding();
	NETWORK_STREAM( "(" << machine.id() << ") MachLog1stPersonMachineHandler::doUpdate wasInside " << wasInside << " onEntrancePad " << onEntrancePad << std::endl );
    NETWORK_INDENT( 2 );

    if( onEntrancePad )
    {
        //See if we, or anyone else has the entrance locked, checking first this machine is allowed
        //into the construction
        bool haveLockedEntrance = false;
        if( machine.canEnterConstruction( *pEnteringConstruction ) )
        {
            MachLogEntrance* pEntrance = &pEnteringConstruction->entrance( entranceIndex );
            if( pEntrance->locked() )
            {
                haveLockedEntrance = machine.hasLockedEntrance()  and  &machine.lockedEntrance() == pEntrance;
            }
            else
            {
                //Lock the entrance
                pEntrance->lock();
                machine.entranceLocked( pEntrance );
                haveLockedEntrance = true;
            }
        }

        //Can proceed if we have got the entrance locked
        if( haveLockedEntrance )
        {
            //Determine which side of the threshold we are on. This is determined by the camera position.
			bool isOutside = false;
			//is this locally controlled - if so determine in/out wrt to camera.
			if( not remotelyControlled() )
			{
	            const W4dCamera& camera = *W4dManager::instance().sceneManager()->currentCamera();
	            MexPoint3d cameraLocation = camera.globalTransform().position();
	            machine.physMachine().globalTransform().transformInverse( &cameraLocation );
	            nextPosition.transform( &cameraLocation );
	            isOutside = pEnteringConstruction->onOutsideSideOfThreshold( entranceIndex, MexPoint2d( cameraLocation) );
			}
			else
				isOutside = pEnteringConstruction->onOutsideSideOfThreshold( entranceIndex, MexPoint2d( machine.position() ) );

            //Ensure the machine knows it's on the pad
            machine.inOrOnPadBuilding( pEnteringConstruction );

            //Ensure machine correctly located inside or outside the construction
            if( wasInside and isOutside )
            {
                //Exit the building
				NETWORK_STREAM(" wasInside and isOutside so calling inside with NULL\n" );
                machine.insideWhichBuilding( NULL );

                //Tell the camera if local
                if( not remotelyControlled() )
                    camera().leaveConstruction();
            }
            else if( not wasInside  and  not isOutside )
            {
                //Enter the building
//				MachLogNetwork& network = MachLogNetwork::instance();
				NETWORK_STREAM(" not wasInside and not isOutside so calling inside with : " << (void*)pEnteringConstruction << std::endl );
				if( remotelyControlled() )
				{
					W4dDomain* pInteriorDomain = pEnteringConstruction->interiorDomain().getAsDomain();
					machine.motionSeq().useSpaceDomain( pInteriorDomain );
					machine.physObject().attachTo( pInteriorDomain );
				}
                machine.insideWhichBuilding( pEnteringConstruction );
				//send useSpaceDomain after the go inside building stuff
				//as the go inside building will set up the correct config space for the machine.
//				if( network.isNetworkGame() and network.remoteStatus( machine.race() ) == MachLogNetwork::LOCAL_PROCESS )
//					network.messageBroker().sendUseSpaceDomainMessage( machine.id(), pEnteringConstruction->id() );

                //Tell the camera if local
                if( not remotelyControlled() )
                    camera().enterConstruction();
            }

            //Ensure the motion sequencer ignores the entrance polygons if we have the entrance locked.
            //Also the construction polygon if outside.
            machine.ignoreEntrancePolygon( true );
            if( isOutside )
                machine.ignoreConstructionPolygon( true );
        }
        else
            moveMachine = false; //Can't move onto the pad
    }
	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );

    //Check to see if this new position is contained in the config space, other than
    //ignored polygons. Don't do this while on entrance pad
    if( moveMachine and not onEntrancePad )
    {
        if( nextPositionOk( &nextPositionCircle ) )
        {
            //Might have been changed by the function
            MexVec3 xBasis = nextPosition.xBasis();
            driver.surfacePosition( nextPositionCircle.centre(), xBasis, &nextPosition );
        }
        else
        {
            //Can't move to the new position
            moveMachine = false;
            driver.stopDead();
        }
    }

	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );
    //If controlling the machine on this node, stop any other machines about to collide with it
    bool stoppedAnyOtherMachines = not remotelyControlled()  and
                                   motionSequencer.preventImpendingCollisions( nextPosition, startTime );

    //Deal with machine trying to move
    if( moveMachine )
    {
        if( stoppedAnyOtherMachines )
        {
            //Stop this machine as well in case its own motion would drive into the other machines
            driver.stopDead();
            moveMachine = false;
        }
        else
        {
            //ok to move to next position
            pData_->pMachine_->physMachine().globalTransform( nextPosition );

            //Update the resting obstacle
            motionSequencer.addRestingObstacleWithoutEcho();
        }
    }

	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );
    //If we have moved off an entrance pad, we can unlock the entrance etc
    bool outside = not machine.insideBuilding();
    if( moveMachine  and  not onEntrancePad  and  machine.hasLockedEntrance() )
    {

        //Close the door
        MachLogConstruction& construction = machine.inOrOnPadWhichBuilding();
        if( construction.isEntranceOpen( 0 ) )
	    	construction.isEntranceOpen( 0, false );

        //Can't ignore the obstacles any more
        machine.ignoreEntrancePolygon( false );
        if( outside )
            machine.ignoreConstructionPolygon( false );

        //Unlock the entrance
        MachLogEntrance& oldEntrance = machine.lockedEntrance();
        oldEntrance.unlock();
        machine.entranceLocked( NULL );

        //Don't need to observe building any more if left completely
        if( outside )
            machine.inOrOnPadBuilding( NULL );
    }

	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );
    //If on an entrance pad, ensure the door opens
    if( moveMachine  and  onEntrancePad )
    {
        if( not pEnteringConstruction->isEntranceOpen( entranceIndex ) )
            pEnteringConstruction->isEntranceOpen( entranceIndex, true );
    }

	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );
    //If we have moved, and have a station locked, unlock it
    if( moveMachine  and  machine.hasStationLocked() )
    {
        machine.stationLocked().lock( false );
        machine.stationLocked( NULL );
    }

	if( not outside )
		machine.physObject().attachTo( machine.insideWhichBuilding().interiorDomain().getAsDomain() );

    //Adjust the camera roll angles
    driver.adjustCameraRoll();

    //Do other driver update processing
    driver.update();
    driver.updateLocomotionAnimations();
	NETWORK_STREAM("(" << machine.id() << ") physMachine.parent is " << (void*)machine.physObject().pParent() << std::endl );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachLog1stPersonMachineHandler::doUpdate DONE\n" );
}

//virtual
void MachLog1stPersonMachineHandler::doUpdateMaxWeaponRange( MATHEX_SCALAR range )
{
    pDriver_->maxWeaponRange( range );
}

MachPhys1stPersonDriver* MachLog1stPersonMachineHandler::pNewPhysDriver
(
    MachLogMachine* pMachine, MachLog1stPersonHandler::NetworkType networkType
)
{
    //If the machine can attack, get the physical view of it
    MachPhysCanAttack *pPhysCanAttack =
        ( pMachine->objectIsCanAttack() ? &pMachine->asCanAttack().physCanAttack() : NULL );

    pDriver_ =
        _NEW( MachPhys1stPersonMachineDriver( &pMachine->physMachine(), pPhysCanAttack,
                                              *MachLogPlanet::instance().surface(), networkType == REMOTE ) );

    return pDriver_;
}

void MachLog1stPersonMachineHandler::stopStrategy()
{
    // Stop what machine is currently doing
    pData_->pMachine_->strategy().beInterrupted();
}

MATHEX_SCALAR MachLog1stPersonMachineHandler::currentSpeed() const
{
	return pDriver_->currentSpeed();
}

bool MachLog1stPersonMachineHandler::nextPositionOk( MexCircle2d* pCircle )
{
    //get the machine, its motion sequencer, and config space
    MachLogMachine& machine = *pData_->pMachine_;
    MachLogMachineMotionSequencer& motionSequencer = machine.motionSeq();
    const PhysConfigSpace2d& configSpace = motionSequencer.currentConfigSpace();

    //First check to see if the next position is inside the config space
    MexAlignedBox2d testBox( pCircle->centre(), pCircle->radius() );
    bool inSpace = configSpace.boundary().contains( testBox );

    bool ok = inSpace;
    if( inSpace )
    {
        //Now check to see if the circle collides with any polygons
        PhysConfigSpace2d::PolygonId polygonId;
        ok = configSpace.contains( *pCircle, motionSequencer.obstacleFlags(), &polygonId );

        if( not ok )
        {
            //Get the polygon which is causing the obstruction
            const MexPolygon2d& polygon = configSpace.polygon( polygonId );

            //Get the machine's current location, and hence compute a direction vector for
            //the motion and distance to move.
            MexPoint2d startPoint = motionSequencer.currentLocation();
            MexVec2 motionDirection( startPoint, pCircle->centre() );
            MATHEX_SCALAR distance = motionDirection.modulus();
            if( distance > MexEpsilon::instance() )
            {
                motionDirection /= distance;

                //Try sliding along the near edges of the polygon
                ctl_vector< MexPoint2d > slidePoints;
                slidePoints.reserve( 4 );
                MexUtility::slideAlongPolygon( polygon, startPoint, motionDirection, distance, &slidePoints );

                //Try each of the slide points
                uint nPoints = slidePoints.size();
                for( uint i = 0; i < nPoints; ++i )
                {
                    //Create a new circle at this point
                    MexCircle2d slideCircle( slidePoints[i], pCircle->radius() );

                    //See if it is ok to go there
                    ok = configSpace.contains( slideCircle, motionSequencer.obstacleFlags(), &polygonId );
                    if( ok )
                    {
                        *pCircle = slideCircle;
                        break;
                    }
                }
            }
        }
    }

    return ok;
}

void MachLog1stPersonMachineHandler::firstUpdateProcessing()
{
    //Don't want to do this again
    pData_->isFirstUpdate_ = false;

    //Get the machine, and if currently inside a building, ensure the camera knows
    if( not remotelyControlled() )
    {
        MachLogMachine& machine = *pData_->pMachine_;
        if( machine.insideBuilding() )
            camera().enterConstruction();
    }
}

//virtual
const MachLog1stPersonActiveSquadron& MachLog1stPersonMachineHandler::actuallyGetActiveSquadron() const
{
    return pData_->activeSquadron_;
}
/* End P1MCHNDL.CPP *************************************************/
