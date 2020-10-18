/*
 * MISSILEM . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/vec2.hpp"
#include "phys/phys.hpp"
#include "world4d/domain.hpp"
#include "sim/manager.hpp"
#include "machphys/missemp.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/machlog.hpp"
#include "machlog/missilem.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/weapon.hpp"
#include "machlog/armourer.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/stats.hpp"
#include "machlog/vmman.hpp"

#include "machlog/internal/firedata.hpp"

#ifndef _INLINE
    #include "machlog/missilem.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogMissileEmplacement );

//////////////////////////////////////////////////////////////////////////////////////////
// static
MexRadians MachLogMissileEmplacement::angleRange()
{
	static MexRadians result_( Mathex::PI / 6 );
	return result_;
}

// static
MATHEX_SCALAR MachLogMissileEmplacement::sinAngleRange()
{
	static MATHEX_SCALAR result_ = sin( MachLogMissileEmplacement::angleRange().asScalar() );
	return result_;
}

MachLogMissileEmplacement::MachLogMissileEmplacement( const MachPhys::MissileEmplacementSubType& subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc )
:   MachLogConstruction( pRace, pNewPhysMissileEmplacement( subType, pRace, level, location, angle, wc ),
						 MachLog::MISSILE_EMPLACEMENT,
                         MachPhysData::instance().missileEmplacementData( subType, level ) ),
	MachLogCanAttack( this, pPhysMissileEmplacement(), wc ),
	subType_( subType ),
	inTransition_( false ),
	domeOpen_( false ),
    lastStrengthEstimateTime_ ( SimManager::instance().currentTime() )
{

    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().missileEmplacements( pRace->race() ).push_back( this );
	MachLogArmourer::arm( this );
}

MachLogMissileEmplacement::MachLogMissileEmplacement( const MachPhys::MissileEmplacementSubType& subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysMissileEmplacement( subType, pRace, level, location, angle, wc ),
						 MachLog::MISSILE_EMPLACEMENT,
						 withId,
                         MachPhysData::instance().missileEmplacementData( subType, level ) ),
	MachLogCanAttack( this, pPhysMissileEmplacement(), wc ),
	subType_( subType ),
	inTransition_( false ),
	domeOpen_( false ),
    lastStrengthEstimateTime_ ( SimManager::instance().currentTime() )
{

    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().missileEmplacements( pRace->race() ).push_back( this );
	MachLogArmourer::arm( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogMissileEmplacement::~MachLogMissileEmplacement()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMissileEmplacement::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogMissileEmplacement& t )
{

    o << "MachLogMissileEmplacement " << (void*)&t << " start" << std::endl;
    o << "MachLogMissileEmplacement " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysMissileEmplacement* MachLogMissileEmplacement::pNewPhysMissileEmplacement
(
    const MachPhys::MissileEmplacementSubType& subType, MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle, MachPhys::WeaponCombo wc
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter and return pointer
    return _NEW( MachPhysMissileEmplacement( pDomain, localTransform, subType, level, pRace->race(), wc ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogMissileEmplacement::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk )
{
	DEBUG_STREAM( DIAG_HAL,"(" << id() << ") MLMissileEmplacement::update\n" );
	//HAL_STREAM(" call isComplete\n" );
    bool complete = isComplete();

	PhysRelativeTime interval = 0.25;
	PhysRelativeTime alertnessResponse = 10.0;

	//HAL_STREAM(" call updateCompletionVisulaistaion or isWorking\n" );
    if( not updateCompletionVisualisation() )
		interval = 0.1;
	else
	{
		if( not pPhysMissileEmplacement()->isWorking() )

		{
			pPhysMissileEmplacement()->isWorking( true );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );
		}
	}

	//TBD: canAttack types should be direct observers of currentTarget and not rely on operations
	//to switch states fro them
//	if( hasCurrentTarget() and not MachLogRaces::instance().objectExists( &currentTarget() ) )
//		currentTarget( NULL );

	//if current target is > than 50 metres then possible retarget
	if( complete )
	{
		switch( subType() )
		{
			case MachPhys::ICBM:
				if( nukeReady()  and not domeOpen() )
				{
					if( inTransition_ )
					{
						inTransition_ = false;
						domeOpen_ = true;

						// inform owning race of this weapon's availability.
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_NUKE_READY, /*id(),*/  race() );
					}
					else
					{
						pPhysMissileEmplacement()->prepForLaunch( SimManager::instance().currentTime(), MachPhysMissileEmplacement::DOME_OPEN );
						inTransition_ = true;
						interval = 3.5;
					}
				}
				else if( ( not nukeReady() ) and domeOpen() )
				{
					if( inTransition_ )
					{
						inTransition_ = false;
						domeOpen_ = false;
						interval = 7.0;
					}
					else
					{
						PhysRelativeTime timeNow = SimManager::instance().currentTime();

						// wait a little while for the missile to clear the launch tube.........
						if( timeNow - weapons().front()->lastFireTime() >= 2.5 )
						{
							pPhysMissileEmplacement()->prepForLaunch( timeNow + 2.0, MachPhysMissileEmplacement::DOME_CLOSE );
							inTransition_ = true;
							interval = 2.0;
						}
					}
				}
				// whatever the situation now, observers should be prompted to check for a new status
				notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::WEAPON_AVAILABILITY_CHANGED );

				break;

			// otherwise (i.e. for all other missileEmp types ), no dome faffling, just attack nearest baddy.
			default:
				if( isComplete()
					and not( isDead() )
					and not( MachLogRaces::instance().inSpecialActorUpdate() )
					and willCheckForTargets() )
				{
					checkAndAttackCloserTarget( this );
				}
				diminishAlertnessAndInaccuracy();
				//alertnessResponse = ( 4.2 - ( _CONST_CAST( PhysRelativeTime, std::min( 100, alertness() ) ) / 30.0 ) );
				alertnessResponse = ( 4.2 - std::min( 100, alertness() ) / 30.0  );
		}
	}
	else
	{
		// emplacement is not yet complete
		if( isNukeSilo() )
		{
			// ensure that the nuke ain't charging up while the silo is being built
			weapons().front()->setChargeToZero()	;
		}
	}

	interval = std::min( MachActor::update( alteredMaxCPUTime, junk ), alertnessResponse );
	DEBUG_STREAM( DIAG_HAL,"(" << id() << ") MLMissileEmplacement::update returning " << interval << std::endl );

    return interval;
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysMissileEmplacement* MachLogMissileEmplacement::pPhysMissileEmplacement()
{
    return (MachPhysMissileEmplacement*)( &physObject() );
}

const MachPhysMissileEmplacement* MachLogMissileEmplacement::pPhysMissileEmplacement() const
{
    return (MachPhysMissileEmplacement*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogMissileEmplacement::constructionData() const
{
	return data();
}

const MachPhysMissileEmplacementData& MachLogMissileEmplacement::data() const
{
	return pPhysMissileEmplacement()->data();
}

const MachPhys::MissileEmplacementSubType& MachLogMissileEmplacement::subType() const
{
	return subType_;
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogMissileEmplacement::canAttack( const MachActor& other ) const
{
	if( position().sqrEuclidianDistance( other.position() ) == 0 )
		return true;

   	return canTurnToAttack( other ) and inAngleRange( other.physObject() );
	                // and hasSightOf( other )
}

//////////////////////////////////////////////////////////////////////

bool MachLogMissileEmplacement::canTurnToAttack( const MachActor& other ) const
{
	return MachLogCanAttack::inWeaponRange( other, MachLogCanAttack::NONE );
//				and orthogonal( coord(), other.coord() )
//					and lineOfSightIsClear( coord(), other.coord() );
}

//////////////////////////////////////////////////////////////////////

bool MachLogMissileEmplacement::canDetect( const MachActor& other ) const
{
	return inScanningRange( other );
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogMissileEmplacement::inAngleRange( const W4dEntity& entity ) const
{

    //Compute this' inverse global transform
    MexTransform3d inverse;
    pPhysMissileEmplacement()->globalAspectTransform().invert( &inverse );

    //Hence get coordinates of entity in local frame of this
    MexPoint3d entityPosition( entity.globalTransform().position() );
    inverse.transform( &entityPosition );

    //Assume 30 degrees either side of straight on is good enough
    //NB sin( 30 ) = 0.5
    return abs( entityPosition.y() ) < MachLogMissileEmplacement::sinAngleRange() * entityPosition.x();
}

PhysRelativeTime MachLogMissileEmplacement::attack( MachActor* pTarget )
{
	// ensure that the guns miss together, fire similar (or same if same weapon) numbers of bursts together etc.
	MachLogFireData fireData = asCanAttack().createFireData();

	HAL_STREAM("(" << id() << ") MLMissileEmplacement::attack  target id " << pTarget->id() << std::endl );
	HAL_STREAM(" weapons.size() " << weapons().size() << std::endl );
	PhysRelativeTime rt = 0;
	for( MachLogCanAttack::Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		PhysRelativeTime wft = (*i)->fire( pTarget, fireData );
		if(wft > rt) rt = wft;
	}

	asCanAttack().improveAim();

	return rt;
}

//virtual
PhysRelativeTime MachLogMissileEmplacement::turn( const MexRadians& angle )
{
	if( MachLogNetwork::instance().isNetworkGame() )
		MachLogNetwork::instance().messageBroker().sendMachineTurnMessage( id(), angle );
	return pPhysMissileEmplacement()->turn( angle );
}

PhysRelativeTime MachLogMissileEmplacement::turnWithoutEcho( const MexRadians& angle )
{
	return pPhysMissileEmplacement()->turn( angle );
}


bool MachLogMissileEmplacement::inScanningRange( const MachActor& other ) const
{
	const MATHEX_SCALAR scanRange = scannerRange();
	if( other.objectIsConstruction() )
		return MachLogSpacialManipulation::withinRangeOfMexAlignedBox2d( other.asConstruction().globalBoundary(), position(), scanRange );

	MexPoint3d myPos = position();
	MexPoint3d targetPos = other.position();
	return myPos.sqrEuclidianDistance( targetPos ) <= ( scanRange * scanRange );
}

bool MachLogMissileEmplacement::domeOpen() const
{
	return domeOpen_;
}

bool MachLogMissileEmplacement::inAngleRange( const MexPoint3d& targetPosition ) const
{

    //Compute this' inverse global transform
    MexTransform3d inverse;
    pPhysMissileEmplacement()->globalAspectTransform().invert( &inverse );

    //Hence get coordinates of entity in local frame of this
    MexPoint3d tempEntityPosition( targetPosition );
    inverse.transform( &tempEntityPosition );

    //Assume 30 degrees either side of straight on is good enough
    //NB sin( 30 ) = 0.5
    return abs( tempEntityPosition.y() ) < MachLogMissileEmplacement::sinAngleRange() * tempEntityPosition.x();
}

PhysRelativeTime MachLogMissileEmplacement::attack( const MexPoint3d& position )
{
	PhysRelativeTime rt = 0;
	for( MachLogCanAttack::Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		PhysRelativeTime wft = (*i)->fire( position );
		if(wft > rt) rt = wft;
	}
	return rt;
}

const MexTransform3d& MachLogMissileEmplacement::globalAspectTransform() const
{
	return pPhysMissileEmplacement()->globalAspectTransform();
}

bool MachLogMissileEmplacement::inWeaponRange( const MexPoint3d& targetPosition ) const
{
	MATHEX_SCALAR sqrWeaponRange = getMaximumWeaponRange();
	sqrWeaponRange *= sqrWeaponRange;

	return position().sqrEuclidianDistance( targetPosition ) <= sqrWeaponRange;
}

MexRadians
MachLogMissileEmplacement::angleToTurnToFace( const MexPoint3d& pos )
{
	MexPoint2d actorPos( position() );

	MexVec2 bearingToTarget( pos.x() - actorPos.x(), pos.y() - actorPos.y() );

	MexVec3 xBasis3;
	pPhysMissileEmplacement()->globalAspectTransform().xBasis( &xBasis3 );

	MexVec2 xBasis2( xBasis3 );
	return xBasis2.angleBetween( bearingToTarget );
}

//virtual
int MachLogMissileEmplacement::militaryStrength() const
{
	// this method has multiple exit-points for efficiency

	if( not isComplete() )
   		return 0;

	switch( subType() )
	{
		// ICBMs do not afford any conventional military protection from aggressors
		case MachPhys::ICBM:
			return 0;
		default:
		{
			return _STATIC_CAST( int, hpRatio() * _STATIC_CAST( MATHEX_SCALAR, objectData().cost() ) );
		}
	}
}

//virtual
int MachLogMissileEmplacement::localStrength() const
{
	if( SimManager::instance().currentTime() - lastStrengthEstimateTime_ > 10.0 )
	{
		// const-cast naughtiness, but this is okay as it doesn't violate the spirit of const
		// from the client's point of view - this is for cached lazy evaluation.
		MachLogMissileEmplacement* pThis = _CONST_CAST( MachLogMissileEmplacement*, this ) ;
		pThis->updateLocalStrengthEstimate();
		pThis->lastStrengthEstimateTime_ = SimManager::instance().currentTime();
	}

	return localStrengthValue();

}

void perWrite( PerOstream& ostr, const MachLogMissileEmplacement& actor )
{
	const MachLogConstruction& base1 = actor;
	const MachLogCanAttack& base2 = actor;

	ostr << base1;
	ostr << base2;
	ostr << actor.subType_;
	ostr << actor.domeOpen_;
	ostr << actor.inTransition_;
	ostr << actor.lastStrengthEstimateTime_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysMissileEmplacement()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogMissileEmplacement& actor )
{
	MachLogConstruction& base1 = actor;
	MachLogCanAttack& base2 = actor;

	istr >> base1;
	istr >> base2;
	istr >> actor.subType_;
	istr >> actor.domeOpen_;
	istr >> actor.inTransition_;
	istr >> actor.lastStrengthEstimateTime_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysMissileEmplacement* pPhysMissileEmplacement = MachLogMissileEmplacement::pNewPhysMissileEmplacement( actor.subType_, &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth(), base2.weaponCombo() );
	actor.setObjectPtr( pPhysMissileEmplacement, trans );
    actor.setPhysCanAttack( pPhysMissileEmplacement );
	actor.id( id );
	actor.constructionCreated( pPhysMissileEmplacement );
	MachLogArmourer::rearm( &actor );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysMissileEmplacement->isWorking( isWorking );

    if( actor.subType_ == MachPhys::ICBM )
	{
        //Restore the silo to its former position. If in transition when saved, we'll jump it to its new state
        if( actor.inTransition_ )
        {
            actor.inTransition_ = false;
            actor.domeOpen_ = not actor.domeOpen_;
        }

        //Only open it - model by default is closed
        if( actor.domeOpen_ )
			pPhysMissileEmplacement->prepForLaunch( PhysAbsoluteTime( 0.0 ), MachPhysMissileEmplacement::DOME_OPEN );
    }

	actor.persistenceReadStrategy( istr );
}

MachLogMissileEmplacement::MachLogMissileEmplacement( PerConstructor con )
:	MachLogConstruction( con ),
	MachLogCanAttack( con )
{
}

MATHEX_SCALAR MachLogMissileEmplacement::scannerRange() const
{
	return getMaximumWeaponRange() * MachLogRaces::instance().stats().scannerRangeToWeaponRangeRatio();
}

bool MachLogMissileEmplacement::isNukeSilo() const
{
	return subType() == MachPhys::ICBM;
}

bool MachLogMissileEmplacement::nukeReady() const
{
	PRE( isNukeSilo() );

	return weapons().front()->recharged();
}

//virtual
void MachLogMissileEmplacement::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
//	MachActor::beHit( damage, byType, pByActor, pByDirection, echo );
	MachLogConstruction::beHit( damage, byType, pByActor, pByDirection, echo ); // somehow it was called for actor not constructions

	if( pByActor
		and allowedToCheckForNewTarget()
		and not isDead()
		and canFireAt( *pByActor )
		and subType() != MachPhys::ICBM
		and willTestHitByCandidate( *pByActor ) )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pByActor->race() );

		// only take a pop at this schmo if we don't think it's a friend (same race or ally),
		if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
		{
			// will try to initiate a counterattack against the actor who hit me.
			checkAndAttackCloserTarget( this, pByActor );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachLogMissileEmplacement::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
//	MachActor::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );
	MachLogConstruction::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );

	if( pByActor
		and allowedToCheckForNewTarget()
		and not isDead()
		and canFireAt( *pByActor )
		and subType() != MachPhys::ICBM
		and willTestHitByCandidate( *pByActor ) )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pByActor->race() );

		// only take a pop at this schmo if we don't think it's a friend (same race or ally),
		if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
		{
			// will try to initiate a counterattack against the actor who hit me.
			checkAndAttackCloserTarget( this, pByActor );
		}
	}
}

/* End MISSILEM.CPP **************************************************/
