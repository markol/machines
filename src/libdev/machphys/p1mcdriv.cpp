/*
 * P 1 M C D R I V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/p1mcdriv.hpp"

#include "mathex/vec3.hpp"
#include "mathex/point2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/abox3d.hpp"

#include "world4d/link.hpp"
#include "world4d/domain.hpp"

#include "sim/manager.hpp"
#include "stdlib/string.hpp"
#include "ctl/list.hpp"

#include "machphys/machine.hpp"
#include "machphys/machdata.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/genedata.hpp"
#include "machphys/locomoti.hpp"

MachPhys1stPersonMachineDriver::MachPhys1stPersonMachineDriver
(
    MachPhysMachine* pMachine, MachPhysCanAttack* pCanAttack,
    const MachPhysPlanetSurface& planetSurface, bool remoteNode
)
:   MachPhys1stPersonDriver( pMachine, pCanAttack, pMachine->machineData().scannerRange(), remoteNode ),
    pMachine_( pMachine ),
    pPlanetSurface_( &planetSurface ),
    lastSpeed_( 0.0 ),
    lastUpdateTime_( SimManager::instance().currentTime() )
{
    if( remoteNode )
	{
        pMachine_->locomotionMethod().isRemotelyControlledFirstPerson( true );
		pMachine_->setMachineControlType(MachPhysMachine::FIRST_PERSON_REMOTE);
	}
	else
	{
		pMachine_->setMachineControlType(MachPhysMachine::FIRST_PERSON_LOCAL);
	}

    pMachine_->locomotionMethod().firstPersonMotionAnimations( MachPhysLocomotionMethod::ENTERING );

    TEST_INVARIANT;
}

MachPhys1stPersonMachineDriver::~MachPhys1stPersonMachineDriver()
{
    TEST_INVARIANT;
    pMachine_->locomotionMethod().firstPersonMotionAnimations( MachPhysLocomotionMethod::STOPPED );
    pMachine_->locomotionMethod().firstPersonMotionAnimations( MachPhysLocomotionMethod::LEAVING );
    pMachine_->locomotionMethod().isRemotelyControlledFirstPerson( false );

	pMachine_->setMachineControlType(MachPhysMachine::OTHER);
}

void MachPhys1stPersonMachineDriver::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhys1stPersonMachineDriver& t )
{

    o << "MachPhys1stPersonMachineDriver " << (void*)&t << " start" << std::endl;
    o << "MachPhys1stPersonMachineDriver " << (void*)&t << " end" << std::endl;

    return o;
}

bool MachPhys1stPersonMachineDriver::nextPosition( MexTransform3d* pNextPosition )
{
    //Store current time and interval since last update
    bool foundNewPosition = false;
    PhysAbsoluteTime now = SimManager::instance().currentTime();
    PhysRelativeTime interval = now - lastUpdateTime_;
    if( interval > 0.0 )
    {
        //Check for need to compute motion
        if( isMovingForwards() or isMovingBackwards() or isTurningLeft() or isTurningRight() or lastSpeed_ != 0.0 )
        {
            //Get the machine's current position. Set a gradient modifier if on a slope.
            const MachPhysMachineData& machineData = pMachine_->machineData();
            const MexTransform3d& oldMachineTransform = pMachine_->globalTransform();

            //We need the x axis direction after a rotation if turning
            MexVec3 xBasis;

            if( isTurningLeft() or isTurningRight() )
            {
                //Get the limit for turning rate, and cap the machine's own rate.
                //See whether fast or slow limit set.
                const MachPhysGeneralData& generalData = MachPhysData::instance().generalData();
                MexRadians maxTurnRate = (turnAtFastRate() ? generalData.firstPersonMaxFastTurnRate()
                                                           : generalData.firstPersonMaxSlowTurnRate() );

                MexRadians turnRate = machineData.rotationSpeed();
                if( turnRate > maxTurnRate )
                    turnRate = maxTurnRate;

                //Compute the required angle of rotation about the z axis
                MexRadians angle = turnRate * interval;
                if( isTurningLeft() )
                    angle = -angle;

                //generate the global machine transform after the rotation
                MexTransform3d rotationTransform( MexEulerAngles( angle, 0.0, 0.0 ) );
                MexTransform3d rotatedGlobalTransform;
                oldMachineTransform.transform( rotationTransform, &rotatedGlobalTransform );

                xBasis = rotatedGlobalTransform.xBasis();
            }
            else
                xBasis = oldMachineTransform.xBasis();

            //Set a gradient modifier if on a slope.
            MATHEX_SCALAR xBasisZ = xBasis.z();
            MATHEX_SCALAR gradientModifer = 1.0;
            if( xBasisZ > 0.1 )
                gradientModifer = ( isMovingForwards() ? 0.8 : 1.2 );
            else if( xBasisZ < -0.1 )
                gradientModifer = ( isMovingForwards() ? 1.2 : 0.8 );

            //Compute the machine's new capped speed based on acceleration and desired direction
            MATHEX_SCALAR acceleration = machineData.acceleration();
            MATHEX_SCALAR topSpeed = machineData.speed() * gradientModifer;
            if( isMovingBackwards() )
            {
                acceleration *= -1.0;
                topSpeed *= -0.25; //Only go slowly in reverse
            }
            else if( not isMovingForwards() )
            {
                //Come to rest
                if( lastSpeed_ > 0.0 )
                    acceleration *= -1.0;
                topSpeed = 0.0;
            }

            //Hence compute the distance it will have moved in the interval
            MATHEX_SCALAR endSpeed;
            MATHEX_SCALAR distance =
                PhysRampAcceleration::distanceAtTime( lastSpeed_, acceleration, topSpeed, interval, &endSpeed );

            //Get the new location by extrapolating this distance along the x basis vector.
            MexPoint3d newLocation = oldMachineTransform.position();
            MexVec3 offset( xBasis );
            offset *= distance;
            newLocation += offset;

            //Use the locomotion method to get the new position for the machine
            surfacePosition( MexPoint2d( newLocation ), xBasis, pNextPosition );
            foundNewPosition = true;

            //Update stored speed. Clamp to zero if close.
            if( Mathex::abs( endSpeed ) < MexEpsilon::instance() )
                endSpeed = 0.0;

            lastSpeed_ = endSpeed;
        }

        //Update stored time
        lastUpdateTime_ = now;
    }

    if( not foundNewPosition )
        *pNextPosition = pMachine_->globalTransform();

    return foundNewPosition;
}

void MachPhys1stPersonMachineDriver::stopDead()
{
    stopMoving();
    lastSpeed_ = 0.0;
}

W4dEntity& MachPhys1stPersonMachineDriver::cameraAttachment( MexTransform3d* pOffsetTransform ) const
{
    //This needs to be at the centre front of the faceplate
    W4dEntity& attachLink = pMachine_->facePlate();

    const MexAlignedBox3d& boundary = attachLink.boundingVolume();
    MexPoint3d offset = boundary.centroid();
    offset.x( boundary.maxCorner().x() );

    *pOffsetTransform = MexTransform3d( offset );
    return attachLink;
}

//virtual
bool MachPhys1stPersonMachineDriver::canTurnHead() const
{
    return pMachine_->canTurnUpperBody();
}

//virtual
PhysRelativeTime MachPhys1stPersonMachineDriver::turnHeadBy( MexRadians angle )
{
    PRE( canTurnHead() );
    return pMachine_->upperBodyTurnBy( angle );
}

//virtual
PhysRelativeTime MachPhys1stPersonMachineDriver::turnHeadTo( MexRadians angle )
{
    PRE( canTurnHead() );
    return pMachine_->upperBodyTurnTo( angle );
}

//virtual
MexRadians MachPhys1stPersonMachineDriver::currentHeadAngle() const
{
    return (canTurnHead() ? pMachine_->upperBodyCurrentAngle() : 0.0);
}

const PhysAbsoluteTime& MachPhys1stPersonMachineDriver::lastUpdateTime() const
{
    return lastUpdateTime_;
}

//virtual
MATHEX_SCALAR MachPhys1stPersonMachineDriver::currentSpeed() const
{
    return lastSpeed_;
}

//virtual
void MachPhys1stPersonMachineDriver::snapHeadTo( MexRadians angle, const PhysAbsoluteTime& time )
{
    PRE( canTurnHead() );

    pMachine_->upperBodySnapTo( angle, time );
}

//virtual
MexRadians MachPhys1stPersonMachineDriver::targetHeadAngle() const
{
    return (canTurnHead() ? pMachine_->upperBodyTargetAngle() : 0.0);
}

//virtual
void MachPhys1stPersonMachineDriver::snapTo
(
    const MexTransform3d& transform, MATHEX_SCALAR speed, const PhysAbsoluteTime& time
)
{
    //Store the speed and time
    lastSpeed_ = speed;
    lastUpdateTime_ = time;

    //Move the machine to its new position and domain
    pMachine_->globalTransform( transform );
    W4dDomain* pNewDomain = pPlanetSurface_->domainAt( transform.position() );
    W4dDomain* pOldDomain = pMachine_->containingDomain();
    if( pNewDomain != pOldDomain )
        pMachine_->attachTo( pNewDomain );
}

void MachPhys1stPersonMachineDriver::updateLocomotionAnimations()
{
    MachPhysLocomotionMethod::FirstPersonMotionState state;

    if( isMovingForwards() )
        state = MachPhysLocomotionMethod::MOVING_FORWARDS;
    else if( isMovingBackwards() )
        state = MachPhysLocomotionMethod::MOVING_BACKWARDS;
    else if( isTurningLeft() )
        state = MachPhysLocomotionMethod::TURNING_LEFT;
    else if( isTurningRight() )
        state = MachPhysLocomotionMethod::TURNING_RIGHT;
    else
        state = MachPhysLocomotionMethod::STOPPED;

    pMachine_->locomotionMethod().firstPersonMotionAnimations( state );
}

void MachPhys1stPersonMachineDriver::surfacePosition
(
    const MexPoint2d& location, const MexVec3& xBasis, MexTransform3d* pTransform
)
{
    //Use the locomotion method dependent function to get the surface position
    MexVec3 terrainUnitNormal;
    MexPoint3d newLocation;
    pMachine_->locomotionMethod().machineSurfacePosition( *pPlanetSurface_, location, xBasis,
                                                          &newLocation, &terrainUnitNormal );

    //Construct the new machine transform and return it
    *pTransform = MexTransform3d( MexTransform3d::Z_ZX, terrainUnitNormal, xBasis, newLocation );
}
/* End P1MCDRIV.CPP *************************************************/
