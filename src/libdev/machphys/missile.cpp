/*
 * M I S S I L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>

#include "machphys/missile.hpp"

#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "mathex/vec3.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/random.hpp"
#include "phys/lsclplan.hpp"
#include "phys/timeangl.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/generic.hpp"
#include "sim/manager.hpp"

#include "system/pathname.hpp"

#include "machphys/fireball.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/vaptrail.hpp"
#include "machphys/compmgr.hpp"
#include "machphys/snddata.hpp"

PER_DEFINE_PERSISTENT( MachPhysMissile );

MachPhysMissile::MachPhysMissile
(
    W4dEntity* pParent,
	const W4dTransform3d& localTransform,
	size_t level,
	CreateLights lights
)
: MachPhysTrailedProjectile( part( level ), pParent, localTransform,
							 (lights==CREATE_LIGHTS)? COPY_LIGHTS: DONT_COPY_LIGHTS )
{
    PRE( level > 0 and level < 10 );

    if(!findLink("flame", &pFlame_))
    	pFlame_ = NULL;

	if( pFlame_ )
		pFlame_->visible( false );	//only be visible when the missile is launched

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a missile from scratch
MachPhysMissile::MachPhysMissile( W4dEntity* pParent, size_t level )
: MachPhysTrailedProjectile( pParent, W4dTransform3d(), level )
//  pImpl_( _NEW( MachPhysMissileImpl( level ) ) )
{
    readCompositeFile( compositeFileName( level ) );

	//CB_DEPIMPL(W4dLink*, pFlame_ );

    if(!findLink("flame", &pFlame_))
    	pFlame_ = NULL;
}

MachPhysMissile::~MachPhysMissile()
{
    TEST_INVARIANT;

    //  Stop any explosion sound associated with this
    W4dSoundManager::instance().stop( this );

}

SysPathName MachPhysMissile::compositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case MISSILE1:
            result = "models/weapons/missile/level1/mis1.cdf";
            break;

        case MISSILE2:
            result = "models/weapons/missile/level2/mis2.cdf";
            break;

        case MISSILE3:
            result = "models/weapons/missile/level3/mis3.cdf";
            break;

        case MISSILE4:
            result = "models/weapons/missile/level4/mis4.cdf";
            break;

        case MISSILE5:
            result = "models/weapons/missile/level5/mis5.cdf";
            break;

        case MISSILE6:
            result = "models/weapons/missile/level6/mis6.cdf";
            break;

        case MISSILE7:
            result = "models/weapons/missile/level7/mis7.cdf";
            break;

        case MISSILE_LARGE:
            result = "models/weapons/lmissile/lmiss.cdf";
            break;

        case NUCLEAR_MISSILE:
            result = "models/weapons/nmissile/nmisea.cdf";
            break;

        DEFAULT_ASSERT_BAD_CASE( level );
    }

    return result;
}
/*
// static
MachPhysMissile& MachPhysMissile::factory( size_t level )
{
    static  Factory   factory_( MAX_LEVELS );

    //  The index must run from 0, the level is running from 1
    size_t  index = level - 1;

    return factory_.part( level, index );
}
*/
// static
MachPhysMissile& MachPhysMissile::part( size_t level )
{
    //  The index must run from 0, the level is running from 1
    size_t  index = level - 1;

    return factory().part( level, index );
}

// static
MachPhysMissile::Factory& MachPhysMissile::factory()
{
    static  Factory   factory_( MAX_LEVELS );

    return factory_;
}

// virtual
PhysRelativeTime    MachPhysMissile::doBeDestroyedAt
(
    const PhysAbsoluteTime& time, MachPhys::StrikeType
)
{
	SOUND_STREAM("Doing missile xplosion" << std::endl);
    destructionTime_ = time;
    PhysRelativeTime    duration = 2.0;

    //  Make the missile disappear
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( false ) );
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.visibilityPlan( visibilityPlanPtr, time );

    //Make sure the vapour trail gets garbage collected
	if( pVapourTrail_ != NULL )
    	pVapourTrail_->finish( time + duration );

    //  Get the position of the missile when it explodes
    MexTransform3d  explosionXform;
    uint n;

    if( entityPlan.hasMotionPlan() )
        entityPlan.transform( time, &explosionXform, &n );
    else
        explosionXform = localTransform();

    //  Set up some fireballs
    if( level_ != NUCLEAR_MISSILE )
    {
		enum ExplosionType
		{
			SMALL,
			BIG
		};
		ExplosionType thisMissileExplosion = SMALL;

		SoundId explosionId = SID_XPLODE1_MISSILE;
		MATHEX_SCALAR missileStrength = 1, lightDuration = 0.6;
		switch( level_ )
		{
		  // The multiplicative factor is directly related to the damage level associated with
		  // the missile
		  case MISSILE1:		missileStrength = 2;   lightDuration = 0.3; thisMissileExplosion = SMALL; break;
		  case MISSILE2:		missileStrength = 3;   lightDuration = 0.5; thisMissileExplosion = SMALL; break;
		  case MISSILE3:		missileStrength = 4;   lightDuration = 0.5; thisMissileExplosion = BIG; break;
		  case MISSILE4:		missileStrength = 3;   lightDuration = 0.5; thisMissileExplosion = SMALL; break;
		  case MISSILE5:		missileStrength = 6;   lightDuration = 1.0; thisMissileExplosion = BIG; break;
		  case MISSILE6:		missileStrength = 3.5; lightDuration = 0.5; thisMissileExplosion = SMALL; break;
		  case MISSILE7:		missileStrength = 2;   lightDuration = 0.3; thisMissileExplosion = SMALL; break;
		  case MISSILE_LARGE:	missileStrength = 6;   lightDuration = 0.9; thisMissileExplosion = BIG; break;
		  default:				missileStrength = 1;   lightDuration = 0.6; thisMissileExplosion = SMALL; break;
		}

		//Use the random numbers to put some variety into the explosions
		MexBasicRandom tempRandom(MexBasicRandom::constructSeededFromTime());
		int randomNumber =
			mexRandomInt(&tempRandom, 0, 100);
		if(thisMissileExplosion == SMALL)
		{
			if(randomNumber < 50)
			{
				explosionId = SID_XPLODE1_MISSILE;
			}
			else
			{
				explosionId = SID_XPLODE2_MISSILE;
			}
		}
		else //thisMissileExplosion == BIG
		{
			if(randomNumber < 50)
			{
				explosionId = SID_XPLODE3_MISSILE;
			}
			else
			{
				explosionId = SID_XPLODE4_MISSILE;
			}
		}

		// Start an explosion.  If the missile model has no lights then,
		// don't put any on the explosion either.
		const RenColour liteCol(2.7, 1.4, 0.3);
		if (lightsSuppressed())
			explosionWithoutLights(pParent(), explosionXform, time, duration, missileStrength);
		else
			explosion(pParent(), explosionXform, time, duration, liteCol,  missileStrength);

	    W4dGeneric* pExplosionSite = _NEW(W4dGeneric(pParent(), explosionXform));

		PhysAbsoluteTime flightDuration = destructionTime_ - SimManager::instance().currentTime();
		W4dSoundManager::instance().play(pExplosionSite, explosionId, destructionTime_, 1);

		if(getLauncherControl() != MachPhysMachine::FIRST_PERSON_LOCAL)
		{
			SOUND_STREAM("Playing missile fly" << std::endl);
			W4dSoundManager::instance().playForDuration( this, SID_MFLY, 0, flightDuration);
		}

	    W4dGarbageCollector::instance().add(pExplosionSite, destructionTime_ + 10);
		SOUND_STREAM("Missile destruction time " << destructionTime_ << std::endl);
		SOUND_STREAM("Missile duration " << flightDuration << std::endl);
		SOUND_STREAM("Playing missile xplosion" << std::endl);
	}

    return duration;
}

void MachPhysMissile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMissile& t )
{

    o << "MachPhysMissile " << (void*)&t << " start" << std::endl;
    o << "MachPhysMissile " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachPhysMissile::beLaunched
(
    const PhysAbsoluteTime& startTime,
    const MachPhysWeaponData& data,
    const MexPoint3d& targetOffsetGlobal
)
{
	//start the missile animations
	W4dCompositePlanPtr flameSpinPlanPtr;

	if( findCompositePlan( "spin", &flameSpinPlanPtr ) )
	{
        plan( *flameSpinPlanPtr, startTime, 1000000);
	}

	if( pFlame_ )
	{
		startFlame( startTime, data );
	}

	if( data.trailOn() and MachPhysComplexityManager::instance().vapourTrailsEnabled() )
	{

		pVapourTrail_ = _NEW( MachPhysVapourTrail( pParent(), 16, 1.5, level_ ) );
	}

    //Initiate the motion using standard superclass function
    PhysRelativeTime flyTime;
    if( level_ == NUCLEAR_MISSILE )
    {
		HAL_STREAM("MachPhysMissile::beLaunched for a NUCLEAR_MISSILE\n" );
		HAL_INDENT( 2 );
	    MATHEX_SCALAR range = data.range();
	    MATHEX_SCALAR speed = data.projectileSpeed();
	    MATHEX_SCALAR inertial = data.extras()[3];

		const MexTransform3d& parentGlobalTransform = pParent()->globalTransform();

		MexPoint3d origin = localTransform().position();
		parentGlobalTransform.transform(&origin);

		MexPoint3d targetPosition = targetOffsetGlobal;

		MexLine3d line(origin, targetPosition);

		if(line.length() > range )
    		targetPosition = line.pointAtDistance( range );

	    MexTransform3d  startTransform = localTransform();
		MexPoint3d startPosition = startTransform.position();

		MexPoint3d topPosition(inertial, 0, 0);	 //along the gun barrel
		startTransform.transform( &topPosition ); //in the local system

	    MexTransform3d  topTransform = startTransform;
						topTransform.position(topPosition);

		MexPoint3d endPosition = targetPosition;
		parentGlobalTransform.transformInverse( &endPosition );

//		MexPoint3d endTopPosition = targetPosition;   oopsy this is wrong
		MexPoint3d endTopPosition = endPosition;
				   endTopPosition.z( topPosition.z() );

		MexLine3d dropLine(endTopPosition, topPosition);

		MexPoint3d dropPosition = dropLine.pointAtDistance(line.length()/4.0);

		MexVec3 xAxis(dropPosition, endPosition);
		MexVec3 planeDefiningVector = MexVec3(xAxis.x() + xAxis.z(), xAxis.y(), xAxis.x() + xAxis.y() + xAxis.z() );
		//such a definition assures that planeDefiningVector is always defined.

	    MexTransform3d endTransform = MexTransform3d( MexTransform3d::X_XY, xAxis, planeDefiningVector, endPosition);

		MexTransform3d dropTransform = endTransform;
		dropTransform.position( dropPosition );

		MexLine3d climPath(startPosition, topPosition);
		PhysRelativeTime climbDuration = climPath.length()/speed;

		MexLine3d flyPath(topPosition, dropPosition);
		PhysRelativeTime flyDuration = climbDuration + flyPath.length()/speed;

		MexLine3d dropPath(endPosition, dropPosition);
		PhysRelativeTime duration = flyDuration + dropPath.length()/speed;

		//Construct and apply the transform plan
	    PhysLinearMotionPlan* pPlan =
	        _NEW( PhysLinearMotionPlan( startTransform, topTransform,  climbDuration ) );

		pPlan->add( dropTransform, flyDuration );
		pPlan->add( endTransform, duration );
		HAL_STREAM("Motion of missile is from startTransform:\n" << startTransform << std::endl );
		HAL_STREAM("\nTo the top flight position:\n" << topTransform << std::endl );
		HAL_STREAM("To the drop transform:\n" << dropTransform << std::endl );
		HAL_STREAM("To the end position:\n" << endTransform << std::endl );
		HAL_STREAM("startPosition  :" << startPosition << std::endl );
		HAL_STREAM("topPosition    :" << topPosition << std::endl );
		HAL_STREAM("endTopPosition :" << endTopPosition << std::endl );
		HAL_STREAM("dropPosition   :" << dropPosition << std::endl );
		HAL_STREAM("endPosition    :" << endPosition << std::endl );

	    PhysMotionPlanPtr planPtr( pPlan );

	    W4dEntityPlan& entityPlan = entityPlanForEdit();
	    entityPlan.absoluteMotion( planPtr, startTime );

	    //Construct and apply a visibility plan to switch it on at startTime
	    //and off at end time
	    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
//	    visibilityPlanPtr->add( false, climbDuration );
//	    visibilityPlanPtr->add( true, flyDuration );
	    visibilityPlanPtr->add( false, duration );

	    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

	    //Store default flight path data

	    startPosition = startTransform.position();
	    endPosition = endTransform.position();

	    MexPoint3d globalStartPosition( startPosition );
	    MexPoint3d globalEndPosition( endPosition );

	    const MexTransform3d& parentTransform = pParent()->globalTransform();
	    parentTransform.transform( &globalStartPosition );
	    parentTransform.transform( &globalEndPosition );
		#ifndef PRODUCTION
		MexPoint3d debugStartPosition( startTransform.position() );
		parentTransform.transform( &debugStartPosition );
		MexPoint3d debugTopPosition( topTransform.position() );
		parentTransform.transform( &debugTopPosition );
		MexPoint3d debugDropPosition( dropTransform.position() );
		parentTransform.transform( &debugDropPosition );
		MexPoint3d debugEndPosition( endTransform.position() );
		parentTransform.transform( &debugEndPosition );
		HAL_STREAM("Global flight path (from the transforms) is:\n" );
		HAL_INDENT( 2 );
		HAL_STREAM("Start " << debugStartPosition << std::endl );
		HAL_STREAM("Top   " << debugTopPosition << std::endl );
		HAL_STREAM("drop  " << debugDropPosition << std::endl );
		HAL_STREAM("end   " << debugEndPosition << std::endl );
		HAL_STREAM("time values: startTime " << startTime << " climbDuration " << climbDuration << " flyDuration " << flyDuration << " duration " << duration << std::endl );
		HAL_INDENT( -2 );
		#endif

		flyTime = duration;
		//take off the time the missile body passes through the target offset
		flyTime -= 20.790564/speed;	  //data is copied from the .x file

	    flightStartTime( startTime );
	    flightDuration( flyTime );
	    flightPath( MexLine3d( globalStartPosition, globalEndPosition ) );
		HAL_INDENT( -2 );

    }
    else
    {
    	flyTime = move( startTime, data );
	}

	SOUND_STREAM("Launching missile at " << startTime << " for " << flyTime << std::endl);
	SOUND_STREAM("Missile destruction " << destructionTime_ << std::endl);
    //Attach sound to missile
//	W4dSoundManager::instance().playForDuration( this, SID_MFLY, startTime, flyTime);

    //Set the vapour trail offset vector
    const MATHEX_SCALAR missileToVapourTrailDistance = 2.5;

    vapourTrailOffset_ = localTransform().xBasis();
    vapourTrailOffset_ *= -missileToVapourTrailDistance;

    //Store the scheduled destruction time
    destructionTime_ = startTime + flyTime;
    return flyTime;
}

MachPhysMissile::MachPhysMissile( PerConstructor con )
: MachPhysTrailedProjectile( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysMissile& missile )
{
    const MachPhysTrailedProjectile& base = missile;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMissile& missile )
{
    MachPhysTrailedProjectile& base = missile;

    istr >> base;
}

//static
MachPhys::WeaponType MachPhysMissile::weaponType(size_t missile_level)
{
	MachPhys::WeaponType weaponType = MachPhys::MULTI_LAUNCHER3;

	switch( missile_level )
	{
		case MachPhysMissile::MISSILE1:
		{
			weaponType = MachPhys::MULTI_LAUNCHER3;
			break;
		}
		case MachPhysMissile::MISSILE2:
		{
			weaponType = MachPhys::MULTI_LAUNCHER5;
			break;
		}
		case MachPhysMissile::MISSILE3:
		{
			weaponType = MachPhys::MULTI_LAUNCHER6;
			break;
		}
		case MachPhysMissile::MISSILE4:
		{
			weaponType = MachPhys::MULTI_LAUNCHER4;
 		break;
		}
		case MachPhysMissile::MISSILE5:
		{
			weaponType = MachPhys::MULTI_LAUNCHER2;
			break;
		}
		case MachPhysMissile::MISSILE6:
		{
			weaponType = MachPhys::MULTI_LAUNCHER1;
			break;
		}
		case MachPhysMissile::MISSILE7:
		{
			weaponType = MachPhys::MULTI_LAUNCHER7;
			break;
		}
		case MachPhysMissile::MISSILE_LARGE:
		{
			weaponType = MachPhys::LARGE_MISSILE;
			break;
		}
		case MachPhysMissile::NUCLEAR_MISSILE:
		{
			weaponType = MachPhys::NUCLEAR_MISSILE;
			break;
		}
        default:
            ASSERT_BAD_CASE_INFO( missile_level );
            break;
	}

	return weaponType;
}

MachPhys::WeaponType MachPhysMissile::weaponType() const
{
	return MachPhysMissile::weaponType(level_);
}

/* End MISSILE.CPP **************************************************/
