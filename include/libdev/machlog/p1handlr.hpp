/*
 * P 1 H A N D L R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLog1stPersonHandler

    ABC for an object representing an entity currently under 1st person control.
*/

#ifndef _MACHLOG_P1HANDLR_HPP
#define _MACHLOG_P1HANDLR_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "machphys/machphys.hpp"

class MexTransform3d;
class MexPoint3d;
class MexRadians;
class W4dEntity;
class MachPhys1stPersonDriver;
class MachPhysFirstPersonStateVector;
class MachActor;
class MachLog1stPersonHandlerData;
class MachLogWeapon;
class MachLogCamera;

class MachLog1stPersonHandler
// Canonical form revoked
{
public:
    //Indicates whether handler is controlling a remote or local first person machine
    enum NetworkType{ LOCAL, REMOTE };

    virtual ~MachLog1stPersonHandler();

    //The actor being controlled
    MachActor& actor();

    //Returns the entity to become the camera parent, and the local transform to give it
    //in pOffsetTransform
    virtual W4dEntity& cameraAttachment( MexTransform3d* pOffsetTransform ) const = 0;

    //To be called immediately after camera has been attached to its parent entity
    //The camera is supplied
    void initialiseCamera( MachLogCamera* pCamera );
    //PRE( pCamera != NULL )

    //Movement set up. If not called every frame assumption is motion stopped
    void moveForwards();
    void moveBackwards();
    void turnLeft();
    void turnRight();

    //Set/get the fast/slow turn rate option
    void turnAtFastRate( bool fast );
    bool turnAtFastRate() const;

    //Control for looking up and down
    void lookDown( MexRadians byAngle ); //up is negative angle
    void lookAhead(); //Centre up/down angle
    MexRadians lookUpDownAngle() const; //Current angle

    //Control for looking left/right
    bool canTurnHead() const;
    void turnHeadBy( MexRadians angle );
    void turnHeadTo( MexRadians angle );
    MexRadians currentHeadAngle() const;

    //True if any enabled weapon is in angle range of the current target point
    bool targetAnglesValid() const;

    //To be called every frame after the setup calls.
    //Clears all setups after processing.
    void update();

    //Number of weapons mounted on the actor
    uint nWeapons() const;

    //The index'th weapon
    MachLogWeapon& weapon( uint index );
    //PRE( index < nWeapons() );

    //Sets the status of the index'th weapon
    void enableWeapon( uint index, bool isEnabled );
    //PRE( index < nWeapons() );

    //The statu sof the index'th weapon
    bool isWeaponEnabled( uint index ) const;
    //PRE( index < nWeapons() );

    //Maximum range of all selected weapons
    MATHEX_SCALAR maxWeaponRange() const;
    
    //True iff the index'th weapon cannot fire at a point, and must have an actor to aim at
    bool weaponCanOnlyFireAtActor( uint index ) const;
    //PRE( index < nWeapons() );

    //returns info about what is in the line of sight, within maximum enabled weapon range.
    //Result indicates whether hits terrain, actor or nothing in range.
    //pTargetPoint returns the location of line-of-sight impact or end-of-range location.
    //pTargetActor returns the actor that would be hit.
    MachPhys::StrikeType aimData( MexPoint3d* pTargetPoint, MachActor** ppTargetActor ) const;

    //Fire each enabled weapon at targetPoint, unless requires an actor
    void fire( const MexPoint3d& targetPoint );

    //Fire each enabled weapon which requires an actor at pTargetActor
    void fire( MachActor* pTargetActor );

    //true iff handling an entity under 1st person control on a remote node
    bool remotelyControlled() const;

    //Update the state of the controlled entity from state
    void updateState( const MachPhysFirstPersonStateVector& state );

    //Fill out pState with current data for the controlled entity
    void computeState( MachPhysFirstPersonStateVector* pState );

    //True if can return the MachLogCamera for first person
    bool hasCamera() const;

    //The first person camera
    MachLogCamera& camera() const;
    //PRE hasCamera()

    void CLASS_INVARIANT;

protected:
    //pActor is the entity being controlled.
    //The base class physical driver is pPhysDriver.
    //Remote/local status indicated by networkType.
    MachLog1stPersonHandler( MachActor* pActor, MachPhys1stPersonDriver* pPhysDriver, NetworkType networkType );

    //Setup queries
    bool isToMoveForwards() const;
    bool isToMoveBackwards() const;
    bool isToTurnLeft() const;
    bool isToTurnRight() const;

    //Establish remote handlers for this entity on remote nodes
    //Subclasses most call at end of ctor.
    void setupRemoteHandlers();

    //Clear remote node handlers for this entity
    //Subclasses most call in dtor.
    void clearRemoteHandlers();

private:
    friend ostream& operator <<( ostream& o, const MachLog1stPersonHandler& t );

    //Clear all the setup flags
    void clearSetupFlags();

    //True if any of the moving/turning flags differ in state from the physical driver
    bool setupFlagsChanged();

    //Recompute the max weapon range
    void updateMaxWeaponRange();

    //Implements the update of motion etc.
    //Setups are cleared after this callback.
    virtual void doUpdate() = 0;

    //Callback to take action on maximum weapon raneg change.
    //this occurs as different weapons are en/disabled
    virtual void doUpdateMaxWeaponRange( MATHEX_SCALAR range ) = 0;

    //Update the flag indicating can hit target point
    void updateTargetAnglesValidFlag();
    
    //Transmit the current state if appropriate (network game, local)
    void updateAnyNetworkState();

    //Send the current state of this entity round the network
    void xmitStateVector();

    //revoked
    MachLog1stPersonHandler( const MachLog1stPersonHandler& );
    MachLog1stPersonHandler& operator =( const MachLog1stPersonHandler& );

    //data members
    MachLog1stPersonHandlerData* pData_; //data implementation object
};


#endif

/* End P1HANDLR.HPP *************************************************/
