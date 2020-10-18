/*
 * M A C H I N E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/machine.hpp"
#include "machphys/internal/machinei.hpp"

#include "stdlib/string.hpp"
#include "render/colour.hpp"            //for fading

#include "world4d/fdedapln.hpp"         //for fading
#include "world4d/entyiter.hpp"         //for fading
#include "world4d/soundman.hpp"
#include "world4d/mvecccha.hpp"
#include "world4d/link.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/meshplan.hpp"
#include "world4d/colpulse.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/compplan.hpp"

#include "ctl/vector.hpp"

#include "phys/linetrav.hpp"
#include "phys/linemoti.hpp"
#include "phys/eulertx.hpp"
#include "phys/lsclplan.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/epsilon.hpp"

#include "sim/manager.hpp"

#include "machphys/machdata.hpp"
#include "machphys/hover.hpp"
#include "machphys/wheels.hpp"
#include "machphys/spider.hpp"
#include "machphys/tracks.hpp"
#include "machphys/glider.hpp"
#include "machphys/machexp.hpp"
#include "machphys/fireball.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/levels.hpp"
#include "machphys/random.hpp"
#include "machphys/racechng.hpp"
#include "machphys/machburn.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/turntrak.hpp"
#include "machphys/mcdmgdat.hpp"
#include "machphys/genedata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/facepl.hpp"
#include "machphys/locomoti.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/administ.hpp"
#include "machphys/constron.hpp"
#include "machphys/attack.hpp"
#include "machphys/weapon.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachPhysMachine );

MachPhysMachine::MachPhysMachine(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    const SysPathName& compositeFileName,
      const MachPhysMachineData& data )

: W4dComposite( pParent, localTransform, SOLID ),
    pImpl_( _NEW( MachPhysMachineImpl ) )
{
    CB_DEPIMPL( W4dLink*, pFaceplate_ );
    CB_DEPIMPL( W4dLink*, pUpperBodyTurnLink_ );
    CB_DEPIMPL( size_t, bodyLevel_ );
    CB_DEPIMPL( size_t, brainLevel_ );

	bodyLevel_ = data.hwLevel();
	brainLevel_ = data.swLevel();

    ASSERT_FILE_EXISTS( compositeFileName.pathname().c_str() );

    readCompositeFile( compositeFileName );

    //See if a faceplate link is defined
    bool linkFound = findLink( "faceplate", &pFaceplate_ ) or
    				 findLink( "fplat", &pFaceplate_ );

    if( not linkFound )
        pFaceplate_ = NULL;

    //Set up any locomotion method
    createLocomotion( data );

    //Set up shaking links
    initialiseShakeLinks();

    //Set up the turning link if defined
    const string& turnLinkName = data.upperBodyTurnLinkName();
    if( turnLinkName.length() != 0  and  turnLinkName != "NONE" )
    {
        linkFound = findLink( turnLinkName, &pUpperBodyTurnLink_ );
        ASSERT_INFO( turnLinkName );
        ASSERT( linkFound, "Turn link not found" );
    }

    defaultExplosionData();

    TEST_INVARIANT;

}

MachPhysMachine::MachPhysMachine(
    const MachPhysMachine& copyMe,
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race,
    const MachPhysMachineData& data )

: W4dComposite( copyMe, pParent, localTransform ),
    pImpl_( _NEW( MachPhysMachineImpl( *copyMe.pImpl_, data, race, bodyLevel, brainLevel ) ) )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );
    CB_DEPIMPL( W4dLink*, pFaceplate_ );
    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );
    CB_DEPIMPL( W4dComposite::W4dLinks, shakeLinks_ );
    CB_DEPIMPL( MachPhys::Race, race_ );
    CB_DEPIMPL( W4dLink*, pUpperBodyTurnLink_ );
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    if( copyMe.pImpl_->pLocomotionMethod_ )
        pLocomotionMethod_ = copyMe.pImpl_->pLocomotionMethod_->clone( this, links() );

    //Set up the locomotion sound
    addLocomotionSound( data );

    //  Store the faceplate link pointer
    if( copyMe.pImpl_->pFaceplate_ )
        pFaceplate_ = links()[ copyMe.pImpl_->pFaceplate_->id() ];


    if( race != MachPhys::RED )
        convertMaterials( data, race_ );

    //Convert the shake link pointers
    size_t nShakeLinks = shakeLinks_.size();
    for( size_t i = 0; i != nShakeLinks; ++i )
        shakeLinks_[i] = links()[ shakeLinks_[i]->id() ];

    //Initialise the upper body turn pointer, and if we have one, set up a controller for doing the work.
    if( copyMe.pImpl_->pUpperBodyTurnLink_ != NULL )
    {
        pUpperBodyTurnLink_ = links()[ copyMe.pImpl_->pUpperBodyTurnLink_->id() ];
        pTurnerTracker_ =
            _NEW( MachPhysTurnerTracker( pUpperBodyTurnLink_, W4d::Z_AXIS, W4d::X_AXIS, data.upperBodyTurnRate() ));
    }

    TEST_INVARIANT;
}

MachPhysMachine::~MachPhysMachine()
{
    TEST_INVARIANT;

    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );
    CB_DEPIMPL( MachPhysMachineExplosion*, pExplosion_ );
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    //Cancel its sound, if any
SOUND_STREAM("Stopping in destructor of machine ... ");
    W4dSoundManager::instance().stop( this );
SOUND_STREAM(" done!" << std::endl);

    if( isBurning()  and  ( not isBurningFinished() ) )
    {
        damageData().finishBurning();
    }


    _DELETE( pLocomotionMethod_ );
    _DELETE( pExplosion_ );
    _DELETE( pDamageData_ );
    _DELETE( pTurnerTracker_ );

    _DELETE( pImpl_ );
}

// virtual
std::auto_ptr< MachPhysMachineMoveInfo > MachPhysMachine::moveInfo
(
    ProfilePtr profilePtr,
    const PhysAbsoluteTime& startTime, MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR capSpeed, MachPhys::FinalState finalState
)
{
/*
    //Check for finished burning
    if( pBurning_  and  pBurning_->isBurningFinished() )
    {
        _DELETE( pBurning_ );
        pBurning_ = NULL;
    }
*/
    //Check for finished burning
    if( isBurning()  and  isBurningFinished() )
    {
        damageData().finishBurning();
    }

    return doMoveInfo(
        profilePtr,
        startTime, startSpeed,
        capSpeed, finalState );
}

//virtual
void MachPhysMachine::move( const MachPhysMachineMoveInfo& info )
{
    doMove( info );
}

// virtual
PhysRelativeTime MachPhysMachine::beDestroyed( void )
{
    //Cancel its sound
    W4dSoundManager::instance().stop( this );
    return doExplode();
}

std::auto_ptr< MachPhysMachineMoveInfo > MachPhysMachine::doMoveInfo
(
    ProfilePtr profilePtr,
    const PhysAbsoluteTime& startTime, MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR capSpeed, MachPhys::FinalState finalState
) const
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );
    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

    CtlCountedPtr< MachPhysLocomotionMethod::RampAccelerations >
        rampAccelerationsPtr( _NEW( MachPhysLocomotionMethod::RampAccelerations ) );
    rampAccelerationsPtr->reserve( 8 );

    pLocomotionMethod_->calculateAccelerations(
      profilePtr, startSpeed, capSpeed,
      finalState, rampAccelerationsPtr );

    MachPhysMachineMoveInfo* pInfo =
        _NEW( MachPhysMachineMoveInfo( profilePtr,
                                       rampAccelerationsPtr,
                                       startTime ) );

    return std::auto_ptr< MachPhysMachineMoveInfo >( pInfo );
}

void MachPhysMachine::addTurnTransformIfNecessary(
    TransformsPtr transformsPtr ) const
{
    PRE( transformsPtr->size() >= 2 );

    //  See if we need to insert an extra transform for the turn at the
    //  beginning of the move

    MexVec3 originalDirection( transformsPtr->front().xBasis() );
    MexVec3 movementDirection( transformsPtr->front().position(), transformsPtr->back().position() );

    //  This might be a pure rotation - check for this
    if( movementDirection.modulus() > MexEpsilon::instance() )
    {
        //  Not a pure rotation - see if an extra turn transform is required

        MexVec2 originalDirection2d( originalDirection.x(), originalDirection.y() );
        MexVec2 movementDirection2d( movementDirection.x(), movementDirection.y() );

        MexRadians turnAngle = originalDirection2d.angleBetween( movementDirection2d );

        if( fabs( turnAngle.asScalar() ) > MexEpsilon::instance() )
        {
            MexTransform3d  turnTransform( transformsPtr->front() );
            turnTransform.transform( MexEulerAngles( turnAngle, 0.0, 0.0 ) );

            transformsPtr->insert( transformsPtr->begin() + 1, turnTransform );
        }
    }
}

//  Smooth off the profile by providing nice transitions between different gradiants

void MachPhysMachine::smoothProfile(
    const MachPhysPlanetSurface::Profile& inputPositions,
    const MachPhysPlanetSurface::Profile& postMovePositions,
    MATHEX_SCALAR machineLength,
    CtlCountedPtr< ctl_vector< MexTransform3d > > outputTransformsPtr ) const
{
    MachPhysPlanetSurface::Profile::const_iterator lastPositionIterator = inputPositions.begin();
    MexVec3 lastToThisVector;

    MachPhysPlanetSurface::Profile::const_iterator positionIterator = lastPositionIterator;

    if( lastPositionIterator != inputPositions.end() )
    {
        //  Get the first point on as a special case
        outputTransformsPtr->push_back( *lastPositionIterator );

        ++positionIterator;

        if( positionIterator != inputPositions.end() )
        {
            lastToThisVector = MexVec3( (*lastPositionIterator).position(), (*positionIterator).position() );
        }
    }

    for( ; positionIterator != inputPositions.end(); ++positionIterator )
    {
        MachPhysPlanetSurface::Profile::const_iterator nextPositionIterator = positionIterator;
        ++nextPositionIterator;

        if( nextPositionIterator != inputPositions.end() )
        {
            //  This is a point in the middle of the profile

            ctl_vector< MexTransform3d >    transforms;
            transforms.reserve( 16 );

            smoothTransition( *lastPositionIterator, *positionIterator, *nextPositionIterator,
              machineLength, &transforms );

            outputTransformsPtr->insert( outputTransformsPtr->end(), transforms.begin(), transforms.end() );
        }
        else
        {
            //  This is the last point. Use a point beyond the current path to calculate the
            //  position required for the last point.

            ctl_vector< MexTransform3d >    transforms;
            transforms.reserve( 16 );

            smoothTransition( *lastPositionIterator, *positionIterator, postMovePositions[ 1 ],
              machineLength, &transforms );

            //  Add transforms until we reach one that has its XY coordinates the same
            //  as our end point

            bool    finished = false;

            for( size_t i = 0; i < transforms.size() and not finished; ++i )
            {
                outputTransformsPtr->push_back( transforms[ i ] );

                if( MexPoint2d( transforms[ i ].position() ) == MexPoint2d( (*positionIterator).position() ) )
                    finished = true;
            }
        }

        lastPositionIterator = positionIterator;
    }
}

void MachPhysMachine::smoothTransition(
    const MexTransform3d& lastTransform,
    const MexTransform3d& thisTransform,
    const MexTransform3d& nextTransform,
    MATHEX_SCALAR machineLength,
    ctl_vector< MexTransform3d >* pTransforms ) const
{
    MexVec3 lastToThisVector( lastTransform.position(), thisTransform.position() );
    MexVec3 thisToNextVector( thisTransform.position(), nextTransform.position() );

    MATHEX_SCALAR   entryAngle = angleWithHorizontal( lastToThisVector );
    MATHEX_SCALAR   exitAngle = angleWithHorizontal( thisToNextVector );

    PATH_PROFILE_STREAM( "lastTransform\n" << lastTransform );
    PATH_PROFILE_STREAM( "thisTransform\n" << thisTransform );
    PATH_PROFILE_STREAM( "nextTransform\n" << nextTransform );

    PATH_PROFILE_INSPECT( entryAngle );
    PATH_PROFILE_INSPECT( exitAngle );

    PATH_PROFILE_INSPECT( machineLength );

    if( entryAngle >= exitAngle - MexEpsilon::instance() )
    {
        pTransforms->push_back( thisTransform );
    }
    else if( lastToThisVector.modulus() < machineLength or
      thisToNextVector.modulus() < machineLength )
    {
        //  If the machine is longer than the distances between this point and
        //  the last or this point and the next it is difficult to do any
        //  meaningful readjustment of its position. For the time being we
        //  just cop out of this. This might have to be revisited.

        pTransforms->push_back( thisTransform );
    }
    else
    {
        //  Moving up a slope

        MATHEX_SCALAR   breakDistance;
        MexPoint3d      lastPosition( lastTransform.position() );

        if( lastToThisVector.modulus() > machineLength )
        {
            breakDistance = lastToThisVector.modulus() - machineLength / 2;
            pTransforms->push_back( MexTransform3d( MexTransform3d::X_XZ, lastToThisVector, lastTransform.zBasis(), lastTransform.position() ) );
            moveTransform( pTransforms->back(), lastToThisVector, breakDistance, &pTransforms->back() );
            lastPosition = pTransforms->back().position();
            PATH_PROFILE_INSPECT( breakDistance );
        }

        //  Calculate the point above the slope transition point at which the
        //  machine will be sat exactly across the slope

        MATHEX_SCALAR   t1 = tan( entryAngle );
        MATHEX_SCALAR   t2 = tan( exitAngle );

        MATHEX_SCALAR m = machineLength / 2;
        MATHEX_SCALAR k = sqrt( ( 4 * m * m ) /
          ( 4 + t1 * t1 + t2 * t2 - 2 * t1 * t2 ) );

        MATHEX_SCALAR z = fabs( k * ( t1 + t2 ) / 2.0 );

        MexPoint3d position( thisTransform.position() );
        position += MexVec3( 0.0, 0.0, z );

        MexVec3 xVector( lastPosition, position );

        MexTransform3d  transitionTransform(
          MexTransform3d::X_XY, xVector, thisTransform.yBasis(), position );

        PATH_PROFILE_INSPECT( z );
        PATH_PROFILE_INSPECT( transitionTransform );

        pTransforms->push_back( transitionTransform );

        if( thisToNextVector.modulus() > machineLength )
        {
            breakDistance = machineLength / 2.0;

            PATH_PROFILE_INSPECT( breakDistance );
            pTransforms->push_back( MexTransform3d() );

            moveTransform( thisTransform, thisToNextVector, breakDistance, &pTransforms->back() );
        }
    }

    PATH_PROFILE_STREAM( "Smoothed transition:\n" << *pTransforms );
}

//  Move a transform by a given distance in a given direction

void MachPhysMachine::moveTransform(
    const MexTransform3d& transform,
    MexVec3 direction,
    MATHEX_SCALAR distance,
    MexTransform3d* pResult ) const
{
    direction.makeUnitVector();
    direction *= distance;

    MexPoint3d  point( transform.position() );
    point += direction;

    *pResult = transform;
    pResult->position( point );
}

MATHEX_SCALAR   MachPhysMachine::angleWithHorizontal( const MexVec3& vec ) const
{
    return atan2( vec.z(), sqrt( vec.x() * vec.x() + vec.y() * vec.y() ) );
}

void MachPhysMachine::doMove( const MachPhysMachineMoveInfo& info )
{
    PHYS_MOTION_STREAM( "Enter MachPhysMachine::doMove " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    PHYS_MOTION_STREAM( "Move info:\n" << info );

    //Compute the linear motion plan
    PhysMotionPlan::Transforms*  pTransforms = _NEW( PhysMotionPlan::Transforms( info.transforms() ) );
    PhysMotionPlan::TransformsPtr   transformsPtr( pTransforms );

    PhysMotionPlan::RampAccelerationsPtr   rampAccelerationsPtr(
      _NEW( PhysMotionPlan::RampAccelerations( info.moveProfiles() ) ) );

    PhysLinearTravelPlan* pPlan = _NEW( PhysLinearTravelPlan(
        transformsPtr,
        rampAccelerationsPtr ) );

    const MachPhysGeneralData& genData = MachPhysData::instance().generalData();
    pPlan->startRotationDistance( genData.startRotationDistance() );

    PhysMotionPlanPtr planPtr( pPlan );

    entityPlanForEdit().absoluteMotion( planPtr, info.startTime(), 0, MachPhys::MACHINE_LOCOMOTION );

    if( pLocomotionMethod_ )
    {
        MachPhysLocomotionMethod::TurnAngles  turnAngles( pPlan->nSegments(), 0.0 );

        if( info.needsToTurn() )
            turnAngles[ 0 ] = info.turnAngle();

        pLocomotionMethod_->moveAnimations( *pPlan, turnAngles, info );
    }

	if(machineData().locomotionType() != MachPhys::HOVER)
	{
		SoundId moveId;
		SoundId idleId;

		const MachPhysMachineData& data = machineData();

		getLocomotionSounds( &idleId, &moveId, data );

		//If the movement is a hover blast then play that sound
		//over the idle one
		if(moveId == SID_FLY_BLAST)
		{
//			W4dSoundManager::instance().play( this, moveId, info.startTime(), 1 );
		}
		else
		{
			W4dSoundManager::instance().stop( this );
			W4dSoundManager::instance().playForDuration( this, moveId, info.startTime(), info.totalTime() );
			W4dSoundManager::instance().play( this, idleId, (info.startTime() + info.totalTime()), 0 );
		}
	}

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysMachine::doMove " << (void*)this << std::endl );
}

bool MachPhysMachine::hasCanAttack() const
{
        MachPhys::MachineType thisMachineType = machineData().machineType();
        bool returnVal = false;
        if(thisMachineType == MachPhys::AGGRESSOR)
        {
                returnVal = true;
        }
        else if(thisMachineType == MachPhys::ADMINISTRATOR)
        {
                returnVal = true;
        }
        return returnVal;
}

MachPhysCanAttack* MachPhysMachine::canAttack() const
{
        PRE(hasCanAttack());
        MachPhys::MachineType thisMachineType = machineData().machineType();
        MachPhysCanAttack* pThisAttack = NULL;
        if(thisMachineType == MachPhys::AGGRESSOR)
        {
                //MachPhysAggressor* pThisAggressor = _STATIC_CAST(MachPhysAggressor*, this);
                MachPhysAggressor* pThisAggressor = _STATIC_CAST(MachPhysAggressor*, _CONST_CAST(MachPhysMachine*, this ));
                pThisAttack = _STATIC_CAST(MachPhysCanAttack*, pThisAggressor);
        }
        else if(thisMachineType == MachPhys::ADMINISTRATOR)
        {
                //MachPhysAdministrator* pThisAdministrator = _STATIC_CAST(MachPhysAdministrator*, this);
                MachPhysAdministrator* pThisAdministrator = _STATIC_CAST(MachPhysAdministrator*, _CONST_CAST(MachPhysMachine*, this ));

                pThisAttack = _STATIC_CAST(MachPhysCanAttack*, pThisAdministrator);
        }
        POST(pThisAttack);
        return pThisAttack;
}

void MachPhysMachine::getLocomotionSounds( SoundId* pIdle, SoundId* pMove, const MachPhysMachineData& data ) const
{
	SoundId moveId;
	SoundId idleId;

	switch(data.machineType())
	{
        case MachPhys::AGGRESSOR:
		{
			switch(data.subType())
			{
				case MachPhys::GRUNT:
				{
					switch(data.hwLevel())
					{
						case 1:
				            moveId = SID_WHEEL_MOVEMENT_SMALL;
				            idleId = SID_WHEEL_IDLE_SMALL;
						break;
						case 2:
						case 3:
				            moveId = SID_TRACK_MOVEMENT_SMALL;
				            idleId = SID_TRACK_IDLE_SMALL;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				}
				break;
				case MachPhys::ASSASSIN:
		            moveId = SID_SPIDER_MOVEMENT_LARGE;
		            idleId = SID_SPIDER_IDLE_LARGE;
				break;
				case MachPhys::KNIGHT:
					switch(data.hwLevel())
					{
						case 3:
				            moveId = SID_TRACK_MOVEMENT_LARGE;
				            idleId = SID_TRACK_IDLE_LARGE;
						break;
						case 4:
						case 5:
				            moveId = SID_HOVER_MOVEMENT_LARGE;
				            idleId = SID_HOVER_IDLE_LARGE;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				break;
				case MachPhys::BALLISTA:
					switch(data.hwLevel())
					{
						case 3:
				            moveId = SID_WHEEL_MOVEMENT_LARGE;
				            idleId = SID_WHEEL_IDLE_LARGE;
						break;
						case 4:
				            moveId = SID_TRACK_MOVEMENT_LARGE;
				            idleId = SID_TRACK_IDLE_LARGE;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				break;
				case MachPhys::NINJA:
	            	moveId = SID_NINJA_MOVEMENT;
    	        	idleId = SID_NINJA_IDLE;
				break;
				default:
					ASSERT(false, "Unknown subtype");
				break;
			}
		}
		break;
        case MachPhys::CONSTRUCTOR:
		{
			switch(data.subType())
			{
				case MachPhys::DOZER:
		            moveId = SID_TRACK_MOVEMENT_SMALL;
		            idleId = SID_TRACK_IDLE_SMALL;
				break;
				case MachPhys::BUILDER:
		            moveId = SID_TRACK_MOVEMENT_SMALL;
		            idleId = SID_TRACK_IDLE_SMALL;
				break;
				case MachPhys::BEHEMOTH:
					switch(data.hwLevel())
					{
						case 4:
				            moveId = SID_WHEEL_MOVEMENT_LARGE;
				            idleId = SID_WHEEL_IDLE_LARGE;
						break;
						case 5:
				            moveId = SID_TRACK_MOVEMENT_LARGE;
				            idleId = SID_TRACK_IDLE_LARGE;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				break;
				default:
					ASSERT(false, "Unknown subtype");
				break;
			}
		}
        break;
        case MachPhys::ADMINISTRATOR:
		{
			switch(data.subType())
			{
				case MachPhys::BOSS:
					switch(data.hwLevel())
					{
						case 1:
				            moveId = SID_FLY_BLAST;
				            idleId = SID_WASP_FLY;
						break;
						case 2:
				            moveId = SID_FLY_BLAST;
				            idleId = SID_BEE_FLY;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				break;
				case MachPhys::OVERSEER:
					switch(data.hwLevel())
					{
						case 2:
				            moveId = SID_TRACK_MOVEMENT_LARGE;
				            idleId = SID_TRACK_IDLE_LARGE;
						break;
						case 3:
				            moveId = SID_SPIDER_MOVEMENT_VERY_LARGE;
				            idleId = SID_SPIDER_IDLE_VERY_LARGE;
						break;
						default:
							ASSERT(false, "Unknown hwlevel");
						break;
					}
				break;
				case MachPhys::COMMANDER:
		            moveId = SID_HOVER_MOVEMENT_LARGE;
		            idleId = SID_HOVER_IDLE_LARGE;
				break;
				default:
					ASSERT(false, "Unknown subtype");
				break;
			}
		}
		break;
        case MachPhys::TECHNICIAN:
		{
			switch(data.subType())
			{
				case MachPhys::LAB_TECH:
		            moveId = SID_WHEEL_MOVEMENT_SMALL;
		            idleId = SID_WHEEL_IDLE_SMALL;
				break;
				case MachPhys::TECH_BOY:
		            moveId = SID_TRACK_MOVEMENT_SMALL;
		            idleId = SID_TRACK_IDLE_SMALL;
				break;
				case MachPhys::BRAIN_BOX:
		            moveId = SID_HOVER_MOVEMENT_SMALL;
		            idleId = SID_HOVER_IDLE_SMALL;
				break;
				default:
					ASSERT(false, "Unknown subtype");
				break;
			}
		}
		break;
        case MachPhys::RESOURCE_CARRIER:
		{
			switch( data.hwLevel() )
			{
				case 1:
					moveId = SID_WHEEL_MOVEMENT_SMALL;
					idleId = SID_WHEEL_IDLE_SMALL;
					break;
				case 2 :
		            moveId = SID_TRACK_MOVEMENT_SMALL;
		            idleId = SID_TRACK_IDLE_SMALL;
				break;
				case 3 :
					moveId = SID_SPIDER_MOVEMENT_VERY_LARGE;
					idleId = SID_SPIDER_IDLE_VERY_LARGE;
				break;
				case 4 :
					moveId = SID_HOVER_MOVEMENT_LARGE;
		            idleId = SID_HOVER_IDLE_LARGE;
				break;
				case 5 :
		            moveId = SID_TRACK_MOVEMENT_LARGE;
		            idleId = SID_TRACK_IDLE_LARGE;
				break;
				default:
					ASSERT(false, "unknown hardware level");
				break;
			}
		}
        break;
        case MachPhys::APC:
		{
			switch(data.hwLevel())
			{
				case 2:
		            moveId = SID_TRACK_MOVEMENT_LARGE;
		            idleId = SID_TRACK_IDLE_LARGE;
				break;
				case 4:
		            moveId = SID_FLY_BLAST;
		            idleId = SID_APC_FLY_LARGE;
				break;
				case 5:
		            moveId = SID_FLY_BLAST;
		            idleId = SID_APC_FLY_SMALL;
				break;
				default:
					ASSERT(false, "unknown hardware level");
				break;
			}
		}
        break;
        case MachPhys::GEO_LOCATOR:
		{
			switch(data.hwLevel())
			{
				case 1:
		            moveId = SID_SPIDER_MOVEMENT_SMALL;
		            idleId = SID_SPIDER_IDLE_SMALL;
				break;
				case 2:
		            moveId = SID_WHEEL_MOVEMENT_SMALL;
		            idleId = SID_WHEEL_IDLE_SMALL;
				break;
				case 3:
					moveId = SID_FLY_BLAST;
		            idleId = SID_GEO_FLY;
				break;
				default:
					ASSERT(false, "unknown hardware level");
				break;
			}
		}
        break;
        case MachPhys::SPY_LOCATOR:
		{
            moveId = SID_HOVER_MOVEMENT_SMALL;
            idleId = SID_HOVER_IDLE_SMALL;
		}
        break;
		default:
			ASSERT(false, "Unknown machine type");
		break;
	}

	*pIdle = idleId;
	*pMove = moveId;
}

size_t MachPhysMachine::bodyLevel() const
{
    CB_DEPIMPL( size_t,	bodyLevel_ );

	return bodyLevel_;
}

size_t MachPhysMachine::brainLevel() const
{
    CB_DEPIMPL( size_t,	brainLevel_ );

	return brainLevel_;
}

PhysRelativeTime MachPhysMachine::doExplode( void )
{
    W4dComposite::clearAllPlans();
    PhysRelativeTime time = explodeLinks();

    return time;
}

PhysRelativeTime MachPhysMachine::explodeLinks( void )
{
    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );
    CB_DEPIMPL( MachPhysMachineExplosion*, pExplosion_ );

    pExplosion_ = _NEW( MachPhysMachineExplosion( this, explosionData(), 2.0 ) );

    return 2.0;
}

void MachPhysMachine::fireballEffect( PhysAbsoluteTime time )
{
    static  W4dMeshPlanPtr fireballPlanPtr;
    static  bool first = true;

    if( first )
    {
        first = false;

        //  Create a fireball effect that lasts for the given time

        const SysPathName fireballModels[] =
        {
            "models/weapons/flash/flash.lod",
            "models/weapons/flash/flash.lod",
        };

        const size_t    nModels = elementsof( fireballModels );
        const size_t    nSegments = ( time / 0.25 ) + 1;

        W4dMeshPlan* pFireballPlan = _NEW( W4dMeshPlan );
        PhysRelativeTime    modelTime = 0.0;

        for( size_t i = 0; i < nSegments; ++i )
        {
            modelTime += time / _STATIC_CAST( double, nSegments );

            size_t modelIndex = rand() % nModels;

            MexEulerAngles  rotation(
//              (float)rand() / 32767.0,
//              (float)rand() / 32767.0,
//              (float)rand() / 32767.0 );
                (float)rand() / (float)RAND_MAX,
                (float)rand() / (float)RAND_MAX,
                (float)rand() / (float)RAND_MAX );

            MexPoint3d  position( 0.0, 0.0, 1.5 );

            W4dTransform3d  transform( rotation, position );

            pFireballPlan->add( fireballModels[ modelIndex ], modelTime, transform );
        }

        fireballPlanPtr = W4dMeshPlanPtr( pFireballPlan );
    }

    entityPlanForEdit().meshPlan( fireballPlanPtr, SimManager::instance().currentTime(), 0 );
}

MATHEX_SCALAR MachPhysMachine::maxTranslationSpeed( void ) const
{
    CB_DEPIMPL( MATHEX_SCALAR, maxTranslationSpeed_ );

    return maxTranslationSpeed_;
}

void    MachPhysMachine::maxTranslationSpeed( MATHEX_SCALAR newSpeed )
{
    CB_DEPIMPL( MATHEX_SCALAR, maxTranslationSpeed_ );
    maxTranslationSpeed_ = newSpeed;
}

MexRadians MachPhysMachine::maxRotationSpeed( void ) const
{
    CB_DEPIMPL( MexRadians, maxRotationSpeed_ );
    return maxRotationSpeed_;
}

void MachPhysMachine::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MATHEX_SCALAR MachPhysMachine::maxTranslationAcceleration( void ) const
{
    CB_DEPIMPL( MATHEX_SCALAR, maxTranslationAcceleration_ );
    return maxTranslationAcceleration_;
}

MexRadians MachPhysMachine::maxRotationAcceleration( void ) const
{
    CB_DEPIMPL( MexRadians, maxRotationAcceleration_ );
    return maxRotationAcceleration_;
}

void MachPhysMachine::createHoverBootLocomotion( MATHEX_SCALAR bobHeight )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    W4dLink*    pLHoverBoot;
    W4dLink*    pRHoverBoot;
    W4dLink*    pBobbingLink;

    bool l_hvLinkFound = findLink( "l_hv", &pLHoverBoot );
    //ASSERT_INFO( "l_hv" );
    //ASSERT( linkFound, "" );

    bool r_hvLinkFound = findLink( "r_hv", &pRHoverBoot );
    //ASSERT_INFO( "r_hv" );
    //ASSERT( linkFound, "" );

    bool hipLinkFound = findLink( "hip", &pBobbingLink );
    //ASSERT_INFO( "hip" );
    //ASSERT( linkFound, "" );

    if( !l_hvLinkFound )
    	pLHoverBoot = NULL;

    if( !r_hvLinkFound )
    	pRHoverBoot = NULL;

    if( !hipLinkFound )
    	pBobbingLink = NULL;

    pLocomotionMethod_ = _NEW( MachPhysHoverBoots( this, pLHoverBoot, pRHoverBoot, pBobbingLink, bobHeight ) );

}

void MachPhysMachine::createWheelLocomotion( MATHEX_SCALAR wheelRadius )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    MachPhysWheels::W4dLinks    lWheels;
    MachPhysWheels::W4dLinks    rWheels;
    W4dLink*    pLink;

    bool linkFound;
    //new links added by sja 26/11
    linkFound = findLink( "lw1", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "rw1", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );

    linkFound = findLink( "lw2", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "rw2", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );

    linkFound = findLink( "cod1d", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "cod1c", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "cod1b", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "cod1a", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );

    linkFound = findLink( "cod2d", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "cod2c", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "cod2b", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "cod2a", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );

    linkFound = findLink( "tel1d", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "tel1c", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "tel1b", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "tel1a", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );

    linkFound = findLink( "tel2d", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "tel2c", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "tel2b", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "tel2a", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );

    linkFound = findLink( "log2d", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "log2c", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "log2b", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "log2a", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );

    linkFound = findLink( "trr2k", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "trr2f", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "trr2g", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "trr2j", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "trr2i", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "trr2h", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );


    linkFound = findLink( "lw_1", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "rw_1", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "lw_2", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "rw_2", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );

    linkFound = findLink( "l_w1", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "r_w1", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "l_w2", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "r_w2", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );
    linkFound = findLink( "l_w3", &pLink );
    if( linkFound )
        lWheels.push_back( pLink );
    linkFound = findLink( "r_w3", &pLink );
    if( linkFound )
        rWheels.push_back( pLink );

    pLocomotionMethod_ = _NEW( MachPhysWheels( this, wheelRadius, lWheels, rWheels ) );
}

void MachPhysMachine::createSpiderLocomotion()
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    //Get the animations where defined.
    W4dCompositePlanPtr restingPlanPtr;
    W4dCompositePlanPtr walkingPlanPtr;
    W4dCompositePlanPtr turningRightPlanPtr;
    W4dCompositePlanPtr turningLeftPlanPtr;
    W4dCompositePlanPtr startWalkingPlanPtr;
    W4dCompositePlanPtr stopWalkingPlanPtr;

    bool    planFound;
    planFound = findCompositePlan( "Resting", &restingPlanPtr );
    planFound = findCompositePlan( "Walking", &walkingPlanPtr );
    planFound = findCompositePlan( "TurningRight", &turningRightPlanPtr );
    planFound = findCompositePlan( "TurningLeft", &turningLeftPlanPtr );
    planFound = findCompositePlan( "Start", &startWalkingPlanPtr );
    planFound = findCompositePlan( "Stop", &stopWalkingPlanPtr );

	pLocomotionMethod_ = _NEW( MachPhysSpiderLegs( this, restingPlanPtr, walkingPlanPtr,
                                                   turningLeftPlanPtr, turningRightPlanPtr,
                                                   startWalkingPlanPtr, stopWalkingPlanPtr) );
}

void MachPhysMachine::createTracksLocomotion( MATHEX_SCALAR repeatsPerMeter )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

	W4dLink* lTrackLink;
	W4dLink* rTrackLink;

    //Find the left link pointer
    bool lFound = findLink("l_trx", &lTrackLink) or
                  findLink("tr_l", &lTrackLink) or
                  findLink("tra_l", &lTrackLink) or
                  findLink("trp2j", &lTrackLink) or
                  findLink("log1v", &lTrackLink) or
                  findLink("trrtl", &lTrackLink);

    //Find the left link pointer
    bool rFound = findLink("r_trx", &rTrackLink) or
                  findLink("tr_r", &rTrackLink) or
                  findLink("tra_r", &rTrackLink) or
                  findLink("trp2h", &rTrackLink) or
                  findLink("log1w", &rTrackLink) or
                  findLink("trrtr", &rTrackLink);

    ASSERT(lFound && rFound, "Couldn't find machine tracks.");

    pLocomotionMethod_ = _NEW(MachPhysTracks( this, lTrackLink, rTrackLink, repeatsPerMeter));
}

void MachPhysMachine::createGliderLocomotion( MATHEX_SCALAR height )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    pLocomotionMethod_ = _NEW( MachPhysGlider( this, height ) );
}

ostream& operator <<( ostream& o, const MachPhysMachine& t )
{

    o << "MachPhysMachine " << t.bodyLevel() << " " << t.brainLevel() << std::endl;
	o << " race " << t.race() << std::endl;
    o << "MachPhysMachine " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachPhysMachine::turn( const MexRadians& angle )
{
    return ( MexEpsilon::isWithinEpsilonOf( angle.asScalar(), 0.0 ) ? 0.0 : doTurn( angle.asScalar() ) );
}

PhysRelativeTime MachPhysMachine::doTurn( MATHEX_SCALAR angle )
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    //Construct a transform turning through angle radians about the z axis
    MexEulerAngles params( angle, 0, 0 );
    MexTransform3d turnRelativeTransform( params );

    //Construct the absolute local transform
    MexTransform3d startTransform( localTransform() );
    MexTransform3d turnTransform;
    startTransform.transform( turnRelativeTransform, &turnTransform );

    //Make a collection of the start and turn transforms
    PhysMotionPlan::TransformsPtr   transformsPtr( _NEW( PhysMotionPlan::Transforms ) );
    transformsPtr->reserve( 2 );

    transformsPtr->push_back( startTransform );
    transformsPtr->push_back( turnTransform );

    //Now a collection of the single turn angle
    MachPhysLocomotionMethod::TurnAngles  turnAngles;
    turnAngles.reserve( 1 );

    turnAngles.push_back( angle );


//	MATHEX_SCALAR rotationSpeed = maxRotationSpeed().asScalar();
//	MATHEX_SCALAR rotationAcceleration = maxRotationAcceleration().asScalar();
//	PhysRelativeTime accelerationTime;
//	PhysRelativeTime turnTime;

    ASSERT( pLocomotionMethod_ != NULL, "" );

    PhysMotionPlan::RampAccelerationsPtr    rampAccelerationsPtr( _NEW( PhysMotionPlan::RampAccelerations ) );
    rampAccelerationsPtr->reserve( 3 );

    MATHEX_SCALAR   dummyCapSpeed = 0.0;
    pLocomotionMethod_->reviseProfile( transformsPtr );

    const MATHEX_SCALAR startSpeed = 0.0;

    pLocomotionMethod_->calculateAccelerations(
     transformsPtr,
     startSpeed,
     dummyCapSpeed,
     MachPhys::AT_REST,
     rampAccelerationsPtr );

    PhysLinearTravelPlan* pPlan = _NEW( PhysLinearTravelPlan(
        transformsPtr,
        rampAccelerationsPtr ) );

    const MachPhysGeneralData& genData = MachPhysData::instance().generalData();
    pPlan->startRotationDistance( genData.startRotationDistance() );

    PhysMotionPlanPtr planPtr( pPlan );

    PhysAbsoluteTime nowTime = SimManager::instance().currentTime();
    entityPlanForEdit().absoluteMotion( planPtr, nowTime, 0 );

    if( pLocomotionMethod_ )
	{
		//define a MachPhysMachineMoveInfo to provide the start time for the moveAnimations(...).
		//other parameters are meaningless here
		MachPhysMachineMoveInfo	info(
          transformsPtr,
          rampAccelerationsPtr,
          nowTime );

        pLocomotionMethod_->moveAnimations( *pPlan, turnAngles, info );
    }
    return pPlan->duration();
}

// static
MachPhysFacePlate& MachPhysMachine::faceplateFactory( size_t brainLevel )
{
    static MachPhysObjectFactory< size_t, MachPhysFacePlate >
       factory_( MachPhysLevels::instance().nSoftwareIndices() );

    return factory_.part( brainLevel, brainLevel - 1 );
}

void MachPhysMachine::convertMaterials( const MachPhysMachineData& data, MachPhys::Race race )
{
    //Use the race changer to do the job
    const W4dCompositeMaterialVecChanger& changer =
        MachPhysRaceChanger::instance().changer( *this, data, race );

    changer.applyOverrides( this );
}

const MachPhysMachineExplosionData& MachPhysMachine::explosionData( void ) const
{
    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );
    return *explosionDataPtr_;
}

MachPhysMachineExplosionData& MachPhysMachine::explosionDataForEdit( void )
{
    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );
    return *explosionDataPtr_;
}

//void MachPhysMachine::explosionData( const MachPhysMachineExplosionData& data )
//{
//    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );
//    explosionDataPtr_ = data;
//}

void MachPhysMachine::defaultExplosionData( void )
{
    CB_DEPIMPL( MachPhysMachineExplosionDataPtr, explosionDataPtr_ );

	// set the explosion center position (bottom of the machine)
	MexPoint3d explosionCenter;
	explosionCenter.z( compositeBoundingVolume().minCorner().z() );
 	explosionDataForEdit().explosionCenter( explosionCenter );
 	explosionDataForEdit().maxToShootOff( MachPhysMachineExplosionData::ALL_LINKS );

    for( W4dLinks::const_iterator i = links().begin(); i != links().end(); ++i )
    {
        MexVec3    vector = (*i)->boundingVolume().maxCorner();
        vector -= (*i)->boundingVolume().minCorner();

        MATHEX_SCALAR size = vector.modulus();

        explosionDataForEdit().addData(
            MachPhysMachineExplosionData::LinkData( (*i)->id(), size, 1.0, 0.3, 0.6 ) );
    }
}

void MachPhysMachine::attachFireball( MachPhysFireball* pFireball, W4dLink* pLink, const MexTransform3d& localTransform )
{
    hold( pFireball, pLink, localTransform );
}

bool MachPhysMachine::intersectsLine
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy
) const
{
    //Just check against composite bounding volume in low and medium, but use
    //link bounding volumes for high
    if( accuracy == W4dEntity::MEDIUM )
        accuracy = W4dEntity::LOW;

    return defaultCompositeIntersectsLine( line, pDistance, accuracy );
}

void MachPhysMachine::initialiseShakeLinks()
{
    CB_DEPIMPL( W4dComposite::W4dLinks, shakeLinks_ );

    //Set reasonable collection sizes
    shakeLinks_.reserve( 2 );

    //These are the standard names we might shake
    const char* shakeNames[] =
    {
        "head", "neck", "body", ""
    };

    //Find up to 2 links with the suggested names
    uint n = 0;
    for( size_t i = 0; n != 2; ++i )
    {
        //Check current name
        string id( shakeNames[i] );
        if( id.length() == 0 )
            break;

        W4dLink* pLink;
        if( findLink( string( shakeNames[i] ), &pLink ) )
        {
            //Add this link pointer
            ++n;
            shakeLinks_.push_back( pLink );
        }
    }
}

PhysRelativeTime MachPhysMachine::shake( const PhysAbsoluteTime startTime )
{
    CB_DEPIMPL( const W4dComposite::W4dLinks, shakeLinks_ );

    //Set up hardcoded data
    //PhysRelativeTime duration = MachPhysData::instance().weaponData(MachPhys::BOLTER).extras()[0];
    //int nIntermediatePositions = MachPhysData::instance().weaponData(MachPhys::BOLTER).extras()[1];
    //MATHEX_SCALAR totalAngle = MachPhysData::instance().weaponData(MachPhys::BOLTER).extras()[2];
    //MATHEX_SCALAR damp = MachPhysData::instance().weaponData(MachPhys::BOLTER).extras()[3];
    PhysRelativeTime duration = 0.5;
    int nIntermediatePositions = 5;
    MATHEX_SCALAR totalAngle = 45;
    MATHEX_SCALAR damp = 0.8;

    //Set up working values
    MATHEX_SCALAR sx = 1.0;
    MATHEX_SCALAR sy = -1.0;
    MATHEX_SCALAR sz = 1.0;
    MexTransform3d intermediateTransform;
    PhysRelativeTime interval = duration / nIntermediatePositions;

    for( W4dComposite::W4dLinks::const_iterator it = shakeLinks_.begin();
         it != shakeLinks_.end(); ++it )
    {
        //Get the link
        W4dLink& link = *(*it);
        PhysLinearMotionPlan* pShakePlan = NULL;
        MATHEX_SCALAR angle = totalAngle;

        //Check this link has no motion plan - mustn't interfere with it
        W4dEntityPlan& linkEntityPlan = link.entityPlanForEdit();
        if( not linkEntityPlan.hasMotionPlan() )
        {
            //Set up transform plan for this link
            MexTransform3d unshakenTransform = link.localTransform();
            for( size_t j = 0; j != nIntermediatePositions; ++j )
            {
                //Generate the angle proportions randomly
                MATHEX_SCALAR px = MachPhysRandom::randomDouble(0.0, 1.0 );
                MATHEX_SCALAR py = (px == 1.0 ? 0.0 : MachPhysRandom::randomDouble(0.0, 1.0 - px ) );
                MATHEX_SCALAR pxPlusPy = px + py;
                MATHEX_SCALAR pz =
                    (pxPlusPy == 1.0 ? 0.0 : MachPhysRandom::randomDouble(0.0, 1.0 - pxPlusPy ) );

                //Now generate the angles
                MexEulerAngles angles
                (
                    MexDegrees( angle * pz * sz ),
                    MexDegrees( angle * py * sy ),
                    MexDegrees( angle * px * sx )
                );

                //Reverse direction modifiers
                sx = -sx;
                sy = -sy;
                sz = -sz;

                //Reduce wobble angle
                angle *= damp;

                //Hence make a rotation only transform, and combine with start to get intermediate
                MexTransform3d rotationTransform( angles );
                unshakenTransform.transform( rotationTransform, &intermediateTransform );

                //Add this to the plan, creating first time
                if( j == 0 )
                {
                    pShakePlan = _NEW( PhysLinearMotionPlan( unshakenTransform,
                                                             intermediateTransform,
                                                             interval ) );
                }
                else
                {
                    pShakePlan->add( intermediateTransform, interval * j );
                }
            }

            //Finish off the plan to return to start position, and apply to link
            pShakePlan->add( unshakenTransform, duration );
            PhysMotionPlanPtr planPtr( pShakePlan );
            linkEntityPlan.absoluteMotion( planPtr, startTime );
        }
    }

    return duration;
}

MachPhys::Race MachPhysMachine::race() const
{
    CB_DEPIMPL( MachPhys::Race, race_ );
    return race_;
}

void MachPhysMachine::createLocomotion( const MachPhysMachineData& data )
{
    //Get the locomotion type and parameter
    MachPhys::LocomotionType type = data.locomotionType();
    MATHEX_SCALAR v = data.locomotionParameter();

    //Construct appropriate type
    switch( type )
    {
        case MachPhys::WHEELS:
        {
            createWheelLocomotion( v );
            break;
        }
        case MachPhys::TRACKS:
        {
            createTracksLocomotion( v );
            break;
        }
        case MachPhys::HOVER:
        {
            createHoverBootLocomotion( v );
            break;
        }
        case MachPhys::SPIDER:
        {
            createSpiderLocomotion();
            break;
        }
        case MachPhys::GLIDER:
        {
            createGliderLocomotion( v );
            break;
        }
    }
}

bool MachPhysMachine::hasFacePlate() const
{
    CB_DEPIMPL( W4dLink*, pFaceplate_ );
	return pFaceplate_ != NULL;
}

W4dLink& MachPhysMachine::facePlate()
{
    CB_DEPIMPL( W4dLink*, pFaceplate_ );
	PRE( hasFacePlate() );
	return *pFaceplate_;
}

void MachPhysMachine::changeRace( MachPhys::Race newRace )
{
    CB_DEPIMPL( MachPhys::Race, race_ );

    //Store new race
    race_ = newRace;

    //Apply mesh overrides
    convertMaterials( machineData(), race_ );

	//also change the race of the weapon(s)
	if( hasCanAttack() )
	{
	    const uint nWeapons = canAttack()->nMountedWeapons();

		for( uint i=0; i<nWeapons; ++i )
		    canAttack()->weapon( i ).changeRace();
	}
}

void MachPhysMachine::changeColour( MachPhys::Race newRace )
{
    //Apply mesh overrides
    convertMaterials( machineData(), newRace );

	//also change the race of the weapon(s)
	if( hasCanAttack() )
	{
	    const uint nWeapons = canAttack()->nMountedWeapons();

		for( uint i=0; i<nWeapons; ++i )
		    canAttack()->weapon( i ).changeColour(newRace);
	}
}

void MachPhysMachine::burn
(
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,
    MATHEX_SCALAR percentage
)
{
    PRE( percentage > 0.0 );
    PRE( percentage <= 100.0 );

	damageData().machineBurning().burn( startTime, duration, percentage );
}

void perWrite( PerOstream& ostr, const MachPhysMachine& machine )
{
    const W4dComposite& base = machine;

    ostr << base;

    ostr << machine.pImpl_;
}

void perRead( PerIstream& istr, MachPhysMachine& machine )
{
    W4dComposite& base = machine;

    istr >> base;

    istr >> machine.pImpl_;
}

MachPhysMachine::MachPhysMachine( PerConstructor con )
: W4dComposite( con ),
    pImpl_( NULL )
{
}

void MachPhysMachine::addLocomotionSound( const MachPhysMachineData& data )
{
	SoundId moveId;
	SoundId idleId;

	getLocomotionSounds(&idleId, &moveId, data);

	//Have to attach the sound to the hip link if it is a hover
	//machine otherwise the sound will come from the ground
	//beneath the machine
	if(idleId == SID_GEO_FLY ||
	   idleId == SID_APC_FLY_LARGE ||
	   idleId == SID_APC_FLY_SMALL ||
	   idleId == SID_BEE_FLY ||
	   idleId == SID_WASP_FLY)
	{
		SOUND_STREAM("Attaching to hip Link" << std::endl);
		W4dLink* pHipLink;
    	if(findLink( "hip", &pHipLink ))
			W4dSoundManager::instance().play( pHipLink, idleId,
								PhysAbsoluteTime(0), 0 );
	}
	else
	{
		SOUND_STREAM("Playing normally" << std::endl);
		W4dSoundManager::instance().play( this, idleId,
                                 PhysAbsoluteTime(0), 0 );
	}
}

// virtual
void MachPhysMachine::stopDead()
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    ASSERT( pLocomotionMethod_ != NULL, "" );

    //Clear all animations associated with locomotion
    clearAnimation( MachPhys::MACHINE_LOCOMOTION );

    //Do locomotion processing
    pLocomotionMethod_->stopDead();
}

bool MachPhysMachine::canTurnUpperBody() const
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    return pTurnerTracker_ != NULL;
}

void MachPhysMachine::upperBodyTrackTarget( const W4dEntity& targetObject )
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );
    pTurnerTracker_->trackTarget( targetObject );
}

const W4dEntity& MachPhysMachine::upperBodyTrackingTarget() const
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( upperBodyIsTrackingTarget() );
    return pTurnerTracker_->targetObject();
}

bool MachPhysMachine::upperBodyIsTrackingTarget() const
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    return pTurnerTracker_ != NULL  and
           pTurnerTracker_->isTrackingTarget();
}

void MachPhysMachine::upperBodyStopTrackingTarget()
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    if( pTurnerTracker_ )
    {
        pTurnerTracker_->stopTracking();
        pTurnerTracker_->turnToAngle( MexRadians( 0.0 ) );
    }
}

MachPhysMachineDamageData& MachPhysMachine::damageData()
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	if( not hasDamageData() )
		pDamageData_ = _NEW( MachPhysMachineDamageData( this ) );

	return *pDamageData_;
}


bool MachPhysMachine::isBurning() const
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	return 	hasDamageData() and pDamageData_->isBurning();
}

bool MachPhysMachine::isBurningFinished() const
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	return 	hasDamageData() and pDamageData_->isBurningFinished();
}

void MachPhysMachine::damageLevel( const double& percent )
{
	damageData().damageLevel( percent );
}

const double& MachPhysMachine::damageLevel() const
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );
    static const double zero = 0;

	return hasDamageData()? pDamageData_->damageLevel(): zero;
}

void MachPhysMachine::updateDamageLevel()
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	if(damageLevel() > 0)
		pDamageData_->updateDamageLevel();
}

bool MachPhysMachine::isDamaged() const
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	return hasDamageData() and pDamageData_->isDamaged();
}

bool MachPhysMachine::hasDamageData() const
{
    CB_DEPIMPL( MachPhysMachineDamageData*, pDamageData_ );

	return pDamageData_ != NULL;
}

PhysRelativeTime MachPhysMachine::upperBodyTurnBy( MexRadians angle )
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );
    PRE( not upperBodyIsTrackingTarget() );

    return pTurnerTracker_->turnByAngle( angle );
}

PhysRelativeTime MachPhysMachine::upperBodyTurnTo( MexRadians angle )
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );
    PRE( not upperBodyIsTrackingTarget() );

    return pTurnerTracker_->turnToAngle( angle );
}

MexRadians MachPhysMachine::upperBodyCurrentAngle() const
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );

    return (pTurnerTracker_ ? pTurnerTracker_->currentAngle() : 0.0);
}

MachPhys::LocomotionType MachPhysMachine::locomotionType() const
{
    return machineData().locomotionType();
}

void MachPhysMachine::upperBodySnapTo( MexRadians angle, const PhysAbsoluteTime& atTime )
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );
    PRE( not upperBodyIsTrackingTarget() );

    pTurnerTracker_->snapToAngle( atTime, angle );
}

MexRadians MachPhysMachine::upperBodyTargetAngle() const
{
    CB_DEPIMPL( MachPhysTurnerTracker*, pTurnerTracker_ );

    PRE( canTurnUpperBody() );
    PRE( not upperBodyIsTrackingTarget() );

    return pTurnerTracker_->targetAngle();
}

const MachPhysLocomotionMethod& MachPhysMachine::locomotionMethod() const
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    PRE( pLocomotionMethod_ != NULL );
    return *pLocomotionMethod_;
}

MachPhysLocomotionMethod& MachPhysMachine::locomotionMethod()
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );

    PRE( pLocomotionMethod_ != NULL );
    return *pLocomotionMethod_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, MachPhysCanMove::OrientationInfo info )
{
    switch( info )
    {
        case MachPhysCanMove::USE_ORIENTATION:
            o << "USE_ORIENTATION";
            break;
        case MachPhysCanMove::DONT_USE_ORIENTATION:
            o << "DONT_USE_ORIENTATION";
            break;
    }

    return o;
}

void MachPhysMachine::inOrOnPadConstruction( MachPhysConstruction* pPhysConstruction )
{
    PRE( pPhysConstruction != NULL );

    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

    floors_.erase( floors_.begin(), floors_.end() );

    const MachPhysPlanetSurface::Floor floor(
        pPhysConstruction->externalGlobalBoundary(),
        pPhysConstruction->globalTransform().position().z() );

    floors_.push_back( floor );

}

void MachPhysMachine::notInOrOnPadConstruction()
{
    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

    floors_.erase( floors_.begin(), floors_.end() );

    POST( floors_.size() == 0 );
}

const MachPhysPlanetSurface::Floors& MachPhysMachine::floors() const
{
    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

    return floors_;
}

MachPhysMachine::ProfilePtr MachPhysMachine::profile
(
    const MexTransform3d& fromGlobalTransform,
    OrientationInfo orientationInfo, const MexPoint3d& destination,
    const MachPhysPlanetSurface& planetSurface
) const
{
    CB_DEPIMPL( MachPhysLocomotionMethod*, pLocomotionMethod_ );
    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

    MachPhysPlanetSurface::Profile moveProfile;
    moveProfile.reserve( 8 );

    if( orientationInfo == USE_ORIENTATION )
        planetSurface.pathProfile( fromGlobalTransform, destination, floors_, &moveProfile );
    else
        planetSurface.pathProfile( fromGlobalTransform.position(), destination, floors_, &moveProfile );

    const MATHEX_SCALAR machineLength = compositeBoundingVolume().xLength();

    //  Calculate the profile after the end of the move so that if the machine ends up on the
    //  transition to a slope the smoothing function will position it correctly.

    MexVec3 travelDirection( fromGlobalTransform.position(), destination );
    travelDirection.makeUnitVector();
    travelDirection *= machineLength;
    MexPoint3d  postMoveDestination( moveProfile.back().position() );
    postMoveDestination += travelDirection;

    MachPhysPlanetSurface::Profile postMoveProfile;
    postMoveProfile.reserve( 8 );

    planetSurface.pathProfile( moveProfile.back().position(), postMoveDestination, floors_, &postMoveProfile );

    CtlCountedPtr< MachPhysPlanetSurface::Profile > smoothedProfilePtr( _NEW( MachPhysPlanetSurface::Profile ) );
	//	The multiplication factor of 2 has been experimentally determined
	//  to give the fullest possible vectors
    smoothedProfilePtr->reserve( moveProfile.size() * 2 );

    PATH_PROFILE_STREAM( "Profile before smoothing\n" << moveProfile );
    PATH_PROFILE_STREAM( "Post move profile\n" << postMoveProfile );

    smoothProfile( moveProfile, postMoveProfile, machineLength, smoothedProfilePtr );

    PATH_PROFILE_STREAM( "Profile after smoothing\n" << *smoothedProfilePtr );

    addTurnTransformIfNecessary( smoothedProfilePtr );

    ASSERT( pLocomotionMethod_ != NULL, "" );

    pLocomotionMethod_->reviseProfile( smoothedProfilePtr );

    PATH_PROFILE_STREAM( "Profile after locomotion revision\n" << *smoothedProfilePtr );

    //  Get the inverse transform down to and including the machine's parent
    //  so that we can get everything into the machine's parents frame of
    //  reference.
    MexTransform3d inverseParentGlobal;
    pParent()->globalTransform().invert( &inverseParentGlobal );

    for( MachPhysPlanetSurface::Profile::iterator i = smoothedProfilePtr->begin();
      i != smoothedProfilePtr->end(); ++i )
    {
        (*i).preTransform( inverseParentGlobal );
    }

    PATH_PROFILE_STREAM( "Profile in local reference frame\n" << *smoothedProfilePtr );

    return smoothedProfilePtr;
}

static const RenMaterial& pulsingBlue()
{
	static const RenColour blue( 0, 210.0/255.0, 1.0 );
	static RenMaterial pulsingBlue(blue);
	static bool first = true;

	if( first )
	{
		first = false;


		pulsingBlue.diffuse(RenColour::black());
		pulsingBlue.emissive(blue);
	}

	return pulsingBlue;
}

static const RenMaterial& pulsingWhite()
{

	static RenMaterial pulsingWhite( RenColour::white() );

	static bool first = true;

	if( first )
	{
		first = false;

		pulsingWhite.diffuse(RenColour::black());
		pulsingWhite.emissive(RenColour::white());
	}

	return pulsingWhite;
}

void MachPhysMachine::doFadeInAnimation()
{
	const PhysAbsoluteTime now = SimManager::instance().currentTime();

	static W4dMaterialPlanPtr blueMaterialPlanPtr;
	static W4dMaterialPlanPtr whiteMaterialPlanPtr;
	static bool first = true;

	if( first )
	{
		first = false;

		const uint nMaterialsInVector = 100;

	    PhysLinearScalarPlan::ScalarVec bTimes, wTimes;

	    	bTimes.reserve(1);
			bTimes.push_back(0.75);

	    	wTimes.reserve(3);
			wTimes.push_back(0.5);
			wTimes.push_back(1.0);
			wTimes.push_back(1.25);

	    PhysLinearScalarPlan::ScalarVec blues, whites;

	    	blues.reserve(2);
	        blues.push_back(0);
	        blues.push_back(1.0);

	    	whites.reserve(4);
	        whites.push_back(1.0);
	        whites.push_back(0.0);
	        whites.push_back(1.0);
	        whites.push_back(1.0);

	    PhysLinearScalarPlan* pBlueAlphaPlan = _NEW( PhysLinearScalarPlan(bTimes, blues) );
	    PhysLinearScalarPlan* pWhiteAlphaPlan = _NEW( PhysLinearScalarPlan(wTimes, whites) );

		PhysScalarPlanPtr blueAlphaPlanPtr(pBlueAlphaPlan);
		PhysScalarPlanPtr whiteAlphaPlanPtr(pWhiteAlphaPlan);

	    W4dSimpleAlphaPlan*  pBluePlan = _NEW( W4dSimpleAlphaPlan( pulsingBlue(),  nMaterialsInVector,	blueAlphaPlanPtr, 10 ) );
	    W4dSimpleAlphaPlan* pWhitePlan = _NEW( W4dSimpleAlphaPlan( pulsingWhite(), nMaterialsInVector, whiteAlphaPlanPtr, 10 ) );

    	blueMaterialPlanPtr = pBluePlan;
    	whiteMaterialPlanPtr = pWhitePlan;
	}

    for( W4dEntityIter it( this ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( blueMaterialPlanPtr, now );
        (*it).entityPlanForEdit().materialPlan( whiteMaterialPlanPtr, now+blueMaterialPlanPtr->duration() );
	}

}



void MachPhysMachine::doFadeOutAnimation()
{
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();

	static W4dMaterialPlanPtr blueMaterialPlanPtr;
	static W4dMaterialPlanPtr whiteMaterialPlanPtr;
	static bool first = true;

	if( first )
	{
		first = false;

		const uint nMaterialsInVector = 100;

	    PhysLinearScalarPlan::ScalarVec bTimes, wTimes;

	    	bTimes.reserve(1);
			bTimes.push_back(0.75);

	    	wTimes.reserve(3);
			wTimes.push_back(0.25);
			wTimes.push_back(0.75);
			wTimes.push_back(1.25);

	    PhysLinearScalarPlan::ScalarVec blues, whites;

	    	blues.reserve(2);
	        blues.push_back(1.0);
	        blues.push_back(0);

	    	whites.reserve(4);
	        whites.push_back(1.0);
	        whites.push_back(1.0);
	        whites.push_back(0.0);
	        whites.push_back(1.0);

	    PhysLinearScalarPlan* pBlueAlphaPlan = _NEW( PhysLinearScalarPlan(bTimes, blues) );
	    PhysLinearScalarPlan* pWhiteAlphaPlan = _NEW( PhysLinearScalarPlan(wTimes, whites) );

		PhysScalarPlanPtr blueAlphaPlanPtr(pBlueAlphaPlan);
		PhysScalarPlanPtr whiteAlphaPlanPtr(pWhiteAlphaPlan);

	    W4dSimpleAlphaPlan*  pBluePlan = _NEW( W4dSimpleAlphaPlan( pulsingBlue(),  nMaterialsInVector,	blueAlphaPlanPtr, 10 ) );
	    W4dSimpleAlphaPlan* pWhitePlan = _NEW( W4dSimpleAlphaPlan( pulsingWhite(), nMaterialsInVector, whiteAlphaPlanPtr, 10 ) );

    	blueMaterialPlanPtr = pBluePlan;
    	whiteMaterialPlanPtr = pWhitePlan;
	}

    for( W4dEntityIter it( this ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( whiteMaterialPlanPtr, now );
        (*it).entityPlanForEdit().materialPlan( blueMaterialPlanPtr, now+whiteMaterialPlanPtr->duration() );
	}
}

MachPhysMachine::ControlType MachPhysMachine::getMachineControlType() const
{
    CB_DEPIMPL( MachPhysMachine::ControlType, machineControlType_ );
	return machineControlType_;
}

void MachPhysMachine::setMachineControlType(MachPhysMachine::ControlType newControlType)
{
    CB_DEPIMPL( MachPhysMachine::ControlType, machineControlType_ );
	machineControlType_ = newControlType;
}

/* End MACHINE.CPP **************************************************/
