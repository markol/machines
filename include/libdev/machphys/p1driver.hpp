/*
 * P 1 D R I V E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhys1stPersonDriver

    ABC Handler for an entity controlled in 1st person.
    Needs to be updated every frame, due to the reactive nature of 1st person control.
*/

#ifndef _MACHPHYS_P1DRIVER_HPP
#define _MACHPHYS_P1DRIVER_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
//#include "mathex/transf3d.hpp"
#include "mathex/ctrans3d.hpp"
//#include "mathex/radians.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

class MexRadians;
class MexTransform3d;
class W4dEntity;
class W4dEntityFilter;
class MachPhysMachine;
class MachPhysCanAttack;
class MachPhysPlanetSurface;
class MachPhys1stPersonDriverImpl;

//Memberwise canonical state info for use in network transmission
struct MachPhysFirstPersonStateVector
{
    MexCompressedTransform3d transform_; //Current position
    PhysCompressedAbsoluteTime lastUpdateTime_; //Time of last position update
    float lastSpeed_; //Last forward/backward speed (signed)
    float headCurrentAngle_; //Angle of upper body at last update time
    float headTargetAngle_; //Angle upper body currently trying to move to
    float cameraCurrentAngle_; //Angle of camera on faceplate at last update time
    float cameraTargetAngle_; //Angle camera trying to move to
    bool movingForwards_; //Flag indicating whether attempting to move forwards/backwards
    bool movingBackwards_;
    bool turningLeft_; //Turning flags
    bool turningRight_;
    bool turnAtFastRate_;
};

class MachPhys1stPersonDriver
// Canonical form revoked
{
public:
    //ctor specifies machine being driven (including view of as can attack - maybe NULL).
    //If remoteNode is true, this is being used to echo 1st person from another node.
    MachPhys1stPersonDriver( W4dEntity* pEntity, MachPhysCanAttack* pCanAttack,
                             bool remoteNode );

    virtual ~MachPhys1stPersonDriver();

    //To be called immediately after camera has been attached to its parent entity
    void initialiseCamera();

    //performs per-frame updating
    void update();

    //The distance along camera line of sight which is at the nearer of max weapon range
    //or intersecting some entity. Updated every update() call.
    MATHEX_SCALAR hitDistance() const;

    //The global coords at point referred to by hitDistance().
    const MexPoint3d& hitPoint() const;

    //The entity which is along line-of-sight as described for aimPoint(), if it exists
    bool hasHitEntity() const;
    W4dEntity& hitEntity() const;
    //PRE( hasHitEntity() );

    //Sets the machine moving forwards
    void moveForwards();

    //Sets the machine moving forwards
    void moveBackwards();

    //Makes the machine decelerate to rest
    void stopMoving();

    //Makes it turn left
    void turnLeft();

    //Makes it turn right
    void turnRight();

    //Makes it stop turning
    void stopTurning();

    //Set/get the fast/slow turn rate option
    void turnAtFastRate( bool fast );
    bool turnAtFastRate() const;

    //State accessors
    bool isMovingForwards() const;
    bool isMovingBackwards() const;
    bool isMoving() const; //Either direction
    bool isTurningLeft() const;
    bool isTurningRight() const;
    bool isTurning() const; //Either direction
    bool isStationary() const; //neither moving nor turning

    //Control for looking up and down
    void lookDown( MexRadians byAngle ); //up is negative angle
    void lookAhead(); //Centre up/down angle
    MexRadians lookUpDownAngle() const; //Current angle

    //Adjust the camera position so it levels out, even if the controlled entity is on a slope.
    void adjustCameraRoll();

    //Return speed currently moving at
    virtual MATHEX_SCALAR currentSpeed() const = 0;

    ////////////////////////////////
    //Head turning controls
    virtual bool canTurnHead() const = 0;

    virtual PhysRelativeTime turnHeadBy( MexRadians angle ) = 0;
    //PRE( canTurnHead() );

    virtual PhysRelativeTime turnHeadTo( MexRadians angle ) = 0;
    //PRE( canTurnHead() );

    //Jump the head to specified angle as at time.
    virtual void snapHeadTo( MexRadians angle, const PhysAbsoluteTime& time ) = 0;
    //PRE( canTurnHead() );

    virtual MexRadians currentHeadAngle() const = 0;

    //The angle trying to move to
    virtual MexRadians targetHeadAngle() const = 0;

    ////////////////////////////////

    //set/get max weapon range
    void maxWeaponRange( MATHEX_SCALAR range );
    MATHEX_SCALAR maxWeaponRange() const;

    //True if the angles from weaponIndex'th weapon to global targetPoint are in limits for the weapon
    bool targetPointAnglesValid( uint weaponIndex, const MexPoint3d& targetPoint ) const;

    //Returns the entity to become the camera parent, and the local transform to give it
    //in pOffsetTransform
    virtual W4dEntity& cameraAttachment( MexTransform3d* pOffsetTransform ) const = 0;

    //Must be supplied before aimData() called
    void aimDataFilter( W4dEntityFilter* pFilter );

    //Update the state of the controlled entity from state
    void updateState( const MachPhysFirstPersonStateVector& state );

    //Fill out pState with current data for the controlled entity
    void computeState( MachPhysFirstPersonStateVector* pState );

    //Make the entity jump to the new position specified by transform, with
    //forward/backward signed speed at specified time.
    virtual void snapTo( const MexTransform3d& transform, MATHEX_SCALAR speed,
                         const PhysAbsoluteTime& time  ) = 0;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhys1stPersonDriver& t );

    //Initialise the weapon tracking
    void setupTrackEntity();

    //Move the tracking target entity to correct aim point derived from last aimData call
    void setTrackEntityPosition();

    //Update the hit data
    void aimData();

    //revoked
    MachPhys1stPersonDriver( const MachPhys1stPersonDriver& );
    MachPhys1stPersonDriver& operator =( const MachPhys1stPersonDriver& );

    //data members
    W4dEntity* pEntity_; //the entity being controlled
    MachPhysCanAttack *pCanAttack_; //the attacker view of the machine
    PhysAbsoluteTime lastUpdateTime_; //base time for current motion
    MATHEX_SCALAR maxWeaponRange_; //Max weapon range of enabled weapons - set by client
    W4dEntity* pCameraEntity_; //Entity at camera position: is the camera on player node, dummy on remote nodes
    W4dEntity* pTrackEntity_; //Entity attached to head for weapons to track
    MexPoint3d hitPoint_; //the point on line-of-sight cached from aimData call
    MATHEX_SCALAR hitDistance_; //distance along line-of-sight to hitPoint
    W4dEntity* pHitEntity_; //the entity that would be fired at on firing cached from aimData call
    bool movingForwards_; //True if driving forwards
    bool movingBackwards_; //True if reversing
    bool turningLeft_; //True if turning to left
    bool turningRight_; //True if turning to left
    bool remoteNode_; //True if echoing 1st person from another player
    MachPhys1stPersonDriverImpl* pImpl_; //More data stored in here
};

ostream& operator <<( ostream& o, const MachPhysFirstPersonStateVector& t );


#endif

/* End P1DRIVER.HPP *************************************************/
