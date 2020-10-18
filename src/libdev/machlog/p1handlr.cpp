/*
 * P 1 H A N D L R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/p1handlr.hpp"

#include "ctl/nbvector.hpp"
#include "utility/id.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"
#include "phys/phys.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyfilt.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/domain.hpp"
#include "sim/manager.hpp"
#include "machphys/p1driver.hpp"
#include "machlog/actor.hpp"
#include "machlog/canattac.hpp"
#include "machlog/weapon.hpp"
#include "machlog/races.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "network/netnet.hpp"

class MachLogAimDataFilter;
class MachLog1stPersonHandlerData
{
public:
    MachActor* pActor_; //the actor being controlled
    MachLogCanAttack* pCanAttack_; //the actor as a can attack if has weapons
    MachPhys1stPersonDriver* pPhysDriver_; //the driver at phys level
    ctl_nb_vector< bool > weaponEnabled_; //Flags indicating which weapons are enabled
    uint nWeapons_; //The number of weapons mounted on the actor
    MATHEX_SCALAR maxWeaponRange_; //the maximum range of all enabled weapons
    PhysAbsoluteTime lastXmitTime_; //time this entity's state last updated round network
    bool isToMoveForwards_; //motion setup flags
    bool isToMoveBackwards_;
    bool isToTurnLeft_;
    bool isToTurnRight_;
    bool targetAnglesValid_; //true if any enabled weapon can hit the target angle
    bool remoteNode_; //true if handling a 1st person entity controlled from a remote node
    bool xmitOnUpdate_; //true if the state vector should be networked on next update call
    bool hasHealWeapon_; //true if a heal weapon is present
    uint healWeaponIndex_; //the index of the heal weapon
    MachLogCamera* pCamera_; //The camera used in 1st person
    MachLogAimDataFilter* pAimDataFiler_; //Filter used on aim data check
};

//Class used to filter out entities we aren't interested in when looking for what's in line of sight
class MachLogAimDataFilter : public W4dEntityFilter
{
public:
    //Override to perform specific filtering.
    //True if entity should be processed. 
    //Returns option for continued tree processing in pOption.
    virtual bool check( const W4dEntity& entity, TreeOption* pOption );
};

MachLog1stPersonHandler::MachLog1stPersonHandler
(
    MachActor* pActor, MachPhys1stPersonDriver* pPhysDriver, NetworkType networkType
)
:   pData_( _NEW( MachLog1stPersonHandlerData ) )
{
    PRE( pActor != NULL );
    PRE( pPhysDriver != NULL );
    pData_->pActor_ = pActor;
    pData_->pPhysDriver_ = pPhysDriver;
    clearSetupFlags();
    pData_->weaponEnabled_.reserve( 4 );
    pData_->pCamera_ = NULL;
    pData_->hasHealWeapon_ = false;
    pData_->healWeaponIndex_ = 0;
    pData_->pAimDataFiler_ = _NEW( MachLogAimDataFilter );

    pPhysDriver->aimDataFilter( pData_->pAimDataFiler_ );

    //If the actor can attack, set things up
    if( pActor->objectIsCanAttack() )
    {
        pData_->pCanAttack_ = &pActor->asCanAttack();
        pData_->nWeapons_ = pData_->pCanAttack_->weapons().size();

        //Loop thru the weapons
        for( uint i = pData_->nWeapons_; i--; )
        {
            //Initially no weapon is enabled
            pData_->weaponEnabled_.push_back( false );

            //We want to ignore heal weapons, coz they give us a load of grief in 1st person
            MachPhys::WeaponType type = pData_->pCanAttack_->weapons()[ i ]->type();

            if( type == MachPhys::SUPERCHARGE_ADVANCED or type == MachPhys::SUPERCHARGE_SUPER )
            {
                ASSERT( not pData_->hasHealWeapon_, "Only one heal weapon allowed" );
                pData_->hasHealWeapon_ = true;
                pData_->healWeaponIndex_ = i;
            }
        }
    }
    else
    {
        pData_->pCanAttack_ = NULL;
        pData_->nWeapons_ = 0;
    }

    pData_->maxWeaponRange_ = 0.0;
    pData_->targetAnglesValid_ = false;
    pData_->remoteNode_ = networkType == REMOTE;
    pData_->xmitOnUpdate_ = not pData_->remoteNode_;
    pData_->lastXmitTime_ = 0.0;

    TEST_INVARIANT;
}

MachLog1stPersonHandler::~MachLog1stPersonHandler()
{
    TEST_INVARIANT;

    _DELETE( pData_->pAimDataFiler_ );
    _DELETE( pData_ );
}

void MachLog1stPersonHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( pData_ != NULL );
}

ostream& operator <<( ostream& o, const MachLog1stPersonHandler& t )
{

    o << "MachLog1stPersonHandler " << (void*)&t << " start" << std::endl;
    o << "MachLog1stPersonHandler " << (void*)&t << " end" << std::endl;

    return o;
}

MachActor& MachLog1stPersonHandler::actor()
{
    return *pData_->pActor_;
}

void MachLog1stPersonHandler::moveForwards()
{
    pData_->isToMoveForwards_ = true;
    pData_->isToMoveBackwards_ = false;
}

void MachLog1stPersonHandler::moveBackwards()
{
    pData_->isToMoveForwards_ = false;
    pData_->isToMoveBackwards_ = true;
}

void MachLog1stPersonHandler::turnLeft()
{
    pData_->isToTurnLeft_ = true;
    pData_->isToTurnRight_ = false;
}

void MachLog1stPersonHandler::turnRight()
{
    pData_->isToTurnLeft_ = false;
    pData_->isToTurnRight_ = true;
}

void MachLog1stPersonHandler::update()
{
    //If networking and any of the setup flags have changed, ensure we retransmit the state vector
    bool weShouldXmit = MachLogNetwork::instance().isNetworkGame() and  not pData_->remoteNode_;
    if( weShouldXmit  and  not pData_->xmitOnUpdate_ )
        pData_->xmitOnUpdate_ = setupFlagsChanged();

    //Concrete class to do its work
    doUpdate();

    //If required, xmit the state vector
    if( weShouldXmit  and  (pData_->xmitOnUpdate_  or  (Phys::time() - pData_->lastXmitTime_ > 0.5)) )
        xmitStateVector();

    if( not pData_->remoteNode_ )
    {
        //cancel all motion setups
        clearSetupFlags();

        //Update the validity flag
        if( pData_->pCanAttack_ )
            updateTargetAnglesValidFlag();
    }
}

bool MachLog1stPersonHandler::isToMoveForwards() const
{
    return pData_->isToMoveForwards_;
}

bool MachLog1stPersonHandler::isToMoveBackwards() const
{
    return pData_->isToMoveBackwards_;
}

bool MachLog1stPersonHandler::isToTurnLeft() const
{
    return pData_->isToTurnLeft_;
}

bool MachLog1stPersonHandler::isToTurnRight() const
{
    return pData_->isToTurnRight_;
}

void MachLog1stPersonHandler::clearSetupFlags()
{
    pData_->isToMoveForwards_ = false;
    pData_->isToMoveBackwards_ = false;
    pData_->isToTurnLeft_ = false;
    pData_->isToTurnRight_ = false;
}

uint MachLog1stPersonHandler::nWeapons() const
{
    uint n = pData_->nWeapons_;
    return (pData_->hasHealWeapon_ ? --n : n);
}

MachLogWeapon& MachLog1stPersonHandler::weapon( uint index )
{
    PRE( index < nWeapons() );

    //So we can ignore heal weapons
    if( pData_->hasHealWeapon_ and index >= pData_->healWeaponIndex_ )
        ++index;

    return *pData_->pCanAttack_->weapons()[ index ];
}

void MachLog1stPersonHandler::enableWeapon( uint index, bool isEnabled )
{
    PRE( index < nWeapons() );

    //So we can ignore heal weapons
    if( pData_->hasHealWeapon_ and index >= pData_->healWeaponIndex_ )
        ++index;

    pData_->weaponEnabled_[ index ] = isEnabled;

    updateMaxWeaponRange();
}

bool MachLog1stPersonHandler::isWeaponEnabled( uint index ) const
{
    PRE( index < nWeapons() );

    //So we can ignore heal weapons
    if( pData_->hasHealWeapon_ and index >= pData_->healWeaponIndex_ )
        ++index;

    return pData_->weaponEnabled_[ index ];
}

void MachLog1stPersonHandler::updateMaxWeaponRange()
{
    MATHEX_SCALAR maxRange = 0.0;
    for( uint i = nWeapons(); i--; )
    {
        if( pData_->remoteNode_  or  isWeaponEnabled( i ) )
        {
            MATHEX_SCALAR weaponRange = weapon( i ).range();
            if( weaponRange > maxRange )
                maxRange = weaponRange;
        }
    }

    pData_->maxWeaponRange_ = maxRange;

    //Callback so subclass can take action
    doUpdateMaxWeaponRange( maxRange );
}

MATHEX_SCALAR MachLog1stPersonHandler::maxWeaponRange() const
{
    return pData_->maxWeaponRange_;
}

MachPhys::StrikeType MachLog1stPersonHandler::aimData( MexPoint3d* pTargetPoint, MachActor** ppTargetActor ) const
{
    MachPhys::StrikeType result = MachPhys::IN_AIR;
    *ppTargetActor = NULL;

    //Use the physical driver's cached entity
    if( pData_->pPhysDriver_->hasHitEntity() )
    {
        W4dEntity& hitEntity = pData_->pPhysDriver_->hitEntity();

        UtlId entityId = hitEntity.id();

        ASSERT_INFO( entityId );
        ASSERT( entityId < 2001, "That entityId isn't in the permissible range." ); 

        if( entityId != 0  and  MachLogRaces::instance().actorExists( entityId ) )
        {
            *ppTargetActor = &MachLogRaces::instance().actor( entityId );
            result = MachPhys::ON_OBJECT;

			// boost alertness of actor being targetted if it's a canattack of a different race to me
			MachActor& targetActor = **ppTargetActor;
			if( targetActor.objectIsCanAttack() and targetActor.race() != pData_->pActor_->race() )
				targetActor.asCanAttack().setMinimumAlertness( 125 );

        }
        else
            result = MachPhys::ON_TERRAIN;
    }

    //Get the hit point coords
    *pTargetPoint = pData_->pPhysDriver_->hitPoint();

    return result;
}

void MachLog1stPersonHandler::fire( const MexPoint3d& targetPoint )
{
    //If a network game ensure state transmitted
    updateAnyNetworkState();

    //Fire each weapon that is enabled
    for( uint i = nWeapons(); i--; )
    {
        if( isWeaponEnabled( i ) and not weaponCanOnlyFireAtActor( i ) )
            weapon( i ).fire( targetPoint );
    }
}

void MachLog1stPersonHandler::fire( MachActor* pTargetActor )
{
    PRE( pTargetActor != NULL );

    //If a network game ensure state transmitted
    updateAnyNetworkState();

    //Fire each weapon that is enabled
    for( uint i = nWeapons(); i--; )
    {
        if( isWeaponEnabled( i ) and weaponCanOnlyFireAtActor( i ) )
        {
            //Check only firing at machines if a heal weapon
            MachLogWeapon& firingWeapon = weapon( i );
            MachPhys::WeaponType type = firingWeapon.type();
            bool canFire = true;
            if( type == MachPhys::SUPERCHARGE_ADVANCED or type == MachPhys::SUPERCHARGE_SUPER )
            {
                canFire = pTargetActor->objectIsMachine();
            }

            if( canFire )
                firingWeapon.fire( pTargetActor );
        }
    }
}

bool MachLog1stPersonHandler::weaponCanOnlyFireAtActor( uint index ) const
{
    PRE( index < nWeapons() );

    //get the weapon type. This decides it
    MachPhys::WeaponType type = _CONST_CAST( MachLog1stPersonHandler*, this)->weapon( index ).type();
    return type == MachPhys::SUPERCHARGE_ADVANCED or type == MachPhys::SUPERCHARGE_SUPER;
}

void MachLog1stPersonHandler::lookDown( MexRadians byAngle )
{
    pData_->pPhysDriver_->lookDown( byAngle );
}

void MachLog1stPersonHandler::lookAhead()
{
    pData_->pPhysDriver_->lookAhead();
}

MexRadians MachLog1stPersonHandler::lookUpDownAngle() const
{
    return pData_->pPhysDriver_->lookUpDownAngle();
}

bool MachLog1stPersonHandler::canTurnHead() const
{
    return pData_->pPhysDriver_->canTurnHead();
}

void MachLog1stPersonHandler::turnHeadBy( MexRadians angle )
{
    pData_->pPhysDriver_->turnHeadBy( angle );
}

void MachLog1stPersonHandler::turnHeadTo( MexRadians angle )
{
    pData_->pPhysDriver_->turnHeadTo( angle );
}

MexRadians MachLog1stPersonHandler::currentHeadAngle() const
{
    return pData_->pPhysDriver_->currentHeadAngle();
}

void MachLog1stPersonHandler::updateTargetAnglesValidFlag()
{
    pData_->targetAnglesValid_ = false;

    //get the target point
    const MachPhys1stPersonDriver& physDriver = *pData_->pPhysDriver_;
    const MexPoint3d& targetPoint = physDriver.hitPoint();

    //Iterate through the enabled weapons
    for( uint i = nWeapons(); i--; )
    {
        if( isWeaponEnabled( i ) and physDriver.targetPointAnglesValid( i, targetPoint ) )
        {
            pData_->targetAnglesValid_ = true;
            break;
        }
    }
}

bool MachLog1stPersonHandler::targetAnglesValid() const
{
    return pData_->targetAnglesValid_;
}

bool MachLog1stPersonHandler::remotelyControlled() const
{
    return pData_->remoteNode_;
}

void MachLog1stPersonHandler::initialiseCamera( MachLogCamera* pCamera )
{
    //Store the camera
    pData_->pCamera_ = pCamera;

    //We need the weapon raneg to correctly position the track entity.
    //We don't bother updating the max weapon range across network so do it once here.
    if( pData_->remoteNode_ )
        updateMaxWeaponRange();

    pData_->pPhysDriver_->initialiseCamera();
}

void MachLog1stPersonHandler::turnAtFastRate( bool fast )
{
    pData_->pPhysDriver_->turnAtFastRate( fast );
}

bool MachLog1stPersonHandler::turnAtFastRate() const
{
    return pData_->pPhysDriver_->turnAtFastRate();
}

bool MachLog1stPersonHandler::setupFlagsChanged()
{
    bool changed = false;

    //Check moving
    MachPhys1stPersonDriver& physDriver = *pData_->pPhysDriver_;

    if( isToMoveForwards() )
        changed = not physDriver.isMovingForwards();
    else if( isToMoveBackwards() )
        changed = not physDriver.isMovingForwards();
    else
        changed = physDriver.isMoving();

    //Check turning
    if( not changed )
    {
        if( isToTurnLeft() )
            changed = not physDriver.isTurningLeft();
        else if( isToTurnRight() )
            changed = not physDriver.isTurningRight();
        else
            changed = physDriver.isTurning();

        if( not changed  and  physDriver.isTurning() )
            changed = physDriver.turnAtFastRate() != turnAtFastRate();
    }

    return changed;
}

void MachLog1stPersonHandler::updateAnyNetworkState()
{
    if( MachLogNetwork::instance().isNetworkGame()  and  not pData_->remoteNode_  and
        SimManager::instance().currentTime() != pData_->lastXmitTime_ )
    {
        xmitStateVector();
    }
}

void MachLog1stPersonHandler::setupRemoteHandlers()
{
    //If this is a local handler, and we are playing a network game, we need to set up remote handlers
    if( not pData_->remoteNode_  and  MachLogNetwork::instance().isNetworkGame() )
    {
        MachActor& actor = *pData_->pActor_;
        MachLogNetwork::instance().messageBroker().sendFirstPersonEndMessage( actor.id(), actor.race(), true );
    }
}

void MachLog1stPersonHandler::clearRemoteHandlers()
{
    //If this is a local handler, and we are playing a network game, we need to clear remote handlers
    if( not pData_->remoteNode_  and  MachLogNetwork::instance().isNetworkGame() )
    {
        MachActor& actor = *pData_->pActor_;
        MachLogNetwork::instance().messageBroker().sendFirstPersonEndMessage( actor.id(), actor.race(), false );
    }
}

void MachLog1stPersonHandler::xmitStateVector()
{
    PRE( MachLogNetwork::instance().isNetworkGame() );
    PRE( not pData_->remoteNode_ );

	PhysAbsoluteTime now = SimManager::instance().currentTime();
	PhysRelativeTime timeGap = now - pData_->lastXmitTime_;

	//if the last time we sent was less than 0.125 (125ms) then don't bother sending.
	//check for greater than zero as well incase time got reset backwards.
	float timeGapThreshold = 1 / NetNetwork::instance().maxSentMessagesPerSecond();
	if( timeGapThreshold > 0.125 )
		timeGapThreshold = 0.125;
	if( timeGap > 0 and timeGap < timeGapThreshold )
		return;
    //Clear flag indicating retransmit on next update call
    pData_->xmitOnUpdate_ = false;
    pData_->lastXmitTime_ = now;

    //Get the current state
    MachPhysFirstPersonStateVector stateVector;
    computeState( &stateVector );

    //Send the update message
    MachActor& actor = *pData_->pActor_;
	if( not NetNetwork::instance().imStuffed() )
	    MachLogNetwork::instance().messageBroker().sendFirstPersonUpdateMessage( actor.id(), actor.race(), stateVector );
}

void MachLog1stPersonHandler::updateState( const MachPhysFirstPersonStateVector& state )
{
    pData_->pPhysDriver_->updateState( state );
}

void MachLog1stPersonHandler::computeState( MachPhysFirstPersonStateVector* pState )
{
    pData_->pPhysDriver_->computeState( pState );
}

bool MachLog1stPersonHandler::hasCamera() const
{
    return pData_->pCamera_ != NULL;
}

MachLogCamera& MachLog1stPersonHandler::camera() const
{
    PRE( hasCamera() );
    return *(pData_->pCamera_);
}

//virtual
bool MachLogAimDataFilter::check( const W4dEntity& entity, TreeOption* pOption )
{
    //Always carry on
    *pOption = PROCESS_SUBTREE;

    //Check by default
    bool doCheck = entity.solid() == W4dEntity::SOLID;
    if( doCheck )
    {
        //See if this one has an id. Projectiles have an id, but aren't in the actor map.
        //We believe these are the only such objects, so we want to exclude them.
        UtlId entityId = entity.id();
        if( entityId != 0 )
        {
            if( MachLogRaces::instance().actorExists( entityId ) )
            {
                //Get the actor
                MachActor& actor = MachLogRaces::instance().actor( entityId );

                //Don't check debris, or ore holos
                if( actor.objectIsDebris() or actor.objectIsOreHolograph() )
                    doCheck = false;
            }
            else
                doCheck = false; //Projectiles
        }
    }

    return doCheck;
}

/* End P1HANDLR.CPP *************************************************/
