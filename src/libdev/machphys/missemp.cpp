/*
 * M I S S E M P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"
#include "machphys/missemp.hpp"

#include "world4d/soundman.hpp"
#include "world4d/link.hpp"
#include "world4d/compplan.hpp"
#include "world4d/entity.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/levels.hpp"
#include "machphys/armourer.hpp"
#include "machphys/turntrak.hpp"
#include "machphys/snddata.hpp"

#include "mathex/transf3d.hpp"
#include "system/pathname.hpp"
#include "ctl/countptr.hpp"
#include "ctl/list.hpp"

PER_DEFINE_PERSISTENT( MachPhysMissileEmplacement );

MachPhysMissileEmplacement::MachPhysMissileEmplacement(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::MissileEmplacementSubType subType,
    size_t level,
    MachPhys::Race race,
    MachPhys::WeaponCombo combo )
: MachPhysConstruction( part( subType, level ), pParent, localTransform, level, race ),
  MachPhysCanAttack( part( subType, level ), this ),
  pData_( _NEW( MachPhysMissileEmplacementData( part( subType, level ).data(), globalTransform() ) ) ),
  subType_( subType )
{
    //Get the exemplar
    MachPhysMissileEmplacement& exemplar = part( subType, level );

    //Create the entity which controls the turning link
	pTurnLink_   = links()[ exemplar.pTurnLink_->id()];

    //If an appropriate type, make the shadow turn with the head
	W4dEntity* pShadow = NULL;

	bool shadowTurn = ( subType == MachPhys::TURRET and level != 1 ) or
					  ( subType == MachPhys::SENTRY and level == 3 );

	if( shadowTurn )
	{

	    for( W4dEntity::W4dEntities::const_iterator i = this->children().begin(); i != this->children().end(); ++i )
	    {
		    if ((*i)->name() == "SHADOW_PROJ" || (*i)->name() == "SHADOW_FIXED")
				pShadow =  *i;
	    }

		if( pShadow != NULL )
		{
			MexTransform3d holdXform = pTurnLink_->globalTransform();
			holdXform.transformInverse( pShadow->globalTransform() );
			hold( pShadow, pTurnLink_, holdXform );
		}
	}

    pTurnerTracker_ = _NEW( MachPhysTurnerTracker( pTurnLink_, W4d::Z_AXIS, W4d::X_AXIS, exemplar.data().maxRotation() ));

    //Mount the appropriate weapons
    MachPhysArmourer::fitWeapons( this, this, combo );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a missile emplacement from scratch

MachPhysMissileEmplacement::MachPhysMissileEmplacement( W4dEntity* pParent, Id id )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
  wireframeFileName( id.subType_, id.level_ ),
  50.0, id.level_,
  MachPhysData::instance().missileEmplacementData( id.subType_, id.level_ ) ),
  pData_( _NEW( MachPhysMissileEmplacementData( MachPhysData::instance().missileEmplacementData( id.subType_, id.level_ ), W4dTransform3d() ) ) ),
  subType_( id.subType_ ),
  pTurnerTracker_( NULL )
{
    //Find the turn link
    if ( findLink("body", &pTurnLink_) );
    //else if ( findLink("gun", &pTurnLink_) );
    else
    {
        ASSERT(false, "No ME Turn Link");
    }

    //Set up the mounting link info for attaching weapons
    initialiseMountingLinks();

    TEST_INVARIANT;
}

MachPhysMissileEmplacement::MachPhysMissileEmplacement( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL ),
  pTurnLink_( NULL )
{
}

MachPhysMissileEmplacement::~MachPhysMissileEmplacement()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
    _DELETE( pTurnerTracker_ );
}

MachPhys::MissileEmplacementSubType MachPhysMissileEmplacement::subType( void ) const
{
    return subType_;
}

// static
MachPhysMissileEmplacement& MachPhysMissileEmplacement::part(
    MachPhys::MissileEmplacementSubType subType,
    size_t level )
{
    return factory().part(
        Id( subType, level ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, level ) );
}

// static
MachPhysMissileEmplacement::Factory& MachPhysMissileEmplacement::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::MISSILE_EMPLACEMENT ) );

    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysMissileEmplacement::constructionData() const
{
	return data();
}

const MachPhysMissileEmplacementData& MachPhysMissileEmplacement::data() const
{
	return *pData_;
}

SysPathName MachPhysMissileEmplacement::compositeFileName(
    MachPhys::MissileEmplacementSubType subType,
    size_t level ) const
{
    SysPathName result;

	switch( subType )
	{
		case MachPhys::TURRET:
		{
		    switch( level )
		    {
        		case 1:
		            result = "models/missilee/turret/level1/exterior/met1e.cdf";
        		    break;

        		case 2:
		            result = "models/missilee/turret/level2/exterior/met2e.cdf";
        		    break;

        		case 3:
		            result = "models/missilee/turret/level3/exterior/met3e.cdf";
        		    break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::SENTRY:
		{
		    switch( level )
		    {
        		case 3:
		            result = "models/missilee/sentry/level3/exterior/mes3e.cdf";
					break;

        		case 4:
		            result = "models/missilee/sentry/level4/exterior/mes4e.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::LAUNCHER:
		{
		    switch( level )
		    {
        		case 4:
		            result = "models/missilee/launcher/level4/exterior/mel4e.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::ICBM:
		{
		    switch( level )
		    {
        		case 5:
		            result = "models/missilee/icbm/level5/exterior/mei5e.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		default:
		    ASSERT_BAD_CASE_INFO( subType );
			break;
	}

    return result;
}

SysPathName MachPhysMissileEmplacement::wireframeFileName(
    MachPhys::MissileEmplacementSubType subType,
    size_t level ) const
{
    SysPathName result;

	switch( subType )
	{
		case MachPhys::TURRET:
		{
		    switch( level )
		    {
        		case 1:
		            result = "models/missilee/turret/level1/wirefram/met1w.cdf";
        		    break;

        		case 2:
		            result = "models/missilee/turret/level2/wirefram/met2w.cdf";
        		    break;

        		case 3:
		            result = "models/missilee/turret/level3/wirefram/met3w.cdf";
        		    break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::SENTRY:
		{
		    switch( level )
		    {
        		case 3:
		            result = "models/missilee/sentry/level3/wirefram/mes3w.cdf";
					break;

        		case 4:
		            result = "models/missilee/sentry/level4/wirefram/mes4w.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::LAUNCHER:
		{
		    switch( level )
		    {
        		case 4:
		            result = "models/missilee/launcher/level4/wirefram/mel4w.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		case MachPhys::ICBM:
		{
		    switch( level )
		    {
        		case 5:
		            result = "models/missilee/icbm/level5/exterior/mei5e.cdf";
					break;

		        default:
        		    ASSERT_BAD_CASE_INFO( level );
		            break;
		    }
			break;
    	}

		default:
		    ASSERT_BAD_CASE_INFO( subType );
			break;
	}

    return result;
}

PhysRelativeTime MachPhysMissileEmplacement::turn( const MexRadians& angle )
{
    PRE( pTurnerTracker_ != NULL );

    //This is a fix for the SILO. Its turn link moves up and down as the silo opens and closes.
    //Hence we might need to adjust the location of the base transform.
    if( subType_ == MachPhys::ICBM )
        pTurnerTracker_->updateBaseLocation();

    return pTurnerTracker_->turnByAngle( angle );
}

PhysRelativeTime MachPhysMissileEmplacement::turnTo( const MexRadians& angle )
{
    PRE( pTurnerTracker_ != NULL );

    //This is a fix for the SILO. Its turn link moves up and down as the silo opens and closes.
    //Hence we might need to adjust the location of the base transform.
    if( subType_ == MachPhys::ICBM )
        pTurnerTracker_->updateBaseLocation();

    return pTurnerTracker_->turnToAngle( angle );
}


void MachPhysMissileEmplacement::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

const W4dTransform3d& MachPhysMissileEmplacement::globalAspectTransform() const
{
	return pTurnLink_->globalTransform();
}

//virtual
W4dComposite& MachPhysMissileEmplacement::asComposite()
{
    return _STATIC_CAST( W4dComposite&, *this );
}

//virtual
const W4dComposite& MachPhysMissileEmplacement::asComposite() const
{
    return _STATIC_CAST( const W4dComposite&, *this );
}

void MachPhysMissileEmplacement::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysMissileEmplacementData(
      MachPhysData::instance().missileEmplacementData( subType(), level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysMissileEmplacement& construction )
{
    const MachPhysConstruction& base1 = construction;
    const MachPhysCanAttack& base2 = construction;

    ostr << base1;
    ostr << base2;

    ostr << construction.subType_;
    ostr << construction.pTurnLink_;
}

void perRead( PerIstream& istr, MachPhysMissileEmplacement& construction )
{
    MachPhysConstruction& base1 = construction;
    MachPhysCanAttack& base2 = construction;

    istr >> base1;
    istr >> base2;

    istr >> construction.subType_;
    istr >> construction.pTurnLink_;

    construction.persistenceInitialiseData();

    //Turner not persisted
    construction.pTurnerTracker_ = NULL;
}

PhysRelativeTime MachPhysMissileEmplacement::prepForLaunch(const PhysAbsoluteTime& startTime, AnimKey key)
{
	PhysRelativeTime prepTime = 0;

	if( subType_ == MachPhys::ICBM )
	{
		W4dCompositePlanPtr openingPlanPtr;

		if( key==DOME_OPEN && findCompositePlan( "Opening", &openingPlanPtr ) )
		{
            plan( *openingPlanPtr, startTime );
			prepTime = openingPlanPtr->finishTime();
			W4dSoundManager::instance().play(this, SID_MISSDOMEOPEN, startTime, 1);
		}

		W4dCompositePlanPtr closingPlanPtr;

		if( key==DOME_CLOSE && findCompositePlan( "Closing", &closingPlanPtr ) )
		{
			//recover the turnLink's position then close
			const PhysRelativeTime recoverTime = turnTo( 0 );

            plan( *closingPlanPtr, startTime + recoverTime );
			prepTime = closingPlanPtr->finishTime() + recoverTime;
			W4dSoundManager::instance().play(this, SID_MISSDOMECLOSE, startTime, 1);
		}
	}
	else
	{
		prepTime = 0;
	}

	return prepTime;
}

void MachPhysMissileEmplacement::trackTarget( const W4dEntity& targetObject )
{
    PRE( pTurnerTracker_ != NULL );
    pTurnerTracker_->trackTarget( targetObject );
}

void MachPhysMissileEmplacement::stopTracking()
{
    PRE( pTurnerTracker_ != NULL );
    pTurnerTracker_->stopTracking();
}

//virtual
bool MachPhysMissileEmplacement::canTrackWeaponBase() const
{
    return pTurnerTracker_ != NULL;
}

//virtual
void MachPhysMissileEmplacement::doWeaponBaseTrackTarget( const W4dEntity& targetObject )
{
    trackTarget( targetObject );
}

//virtual
void MachPhysMissileEmplacement::doStopWeaponBaseTrackingTarget()
{
    stopTracking();
}

void MachPhysMissileEmplacement::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );

	damageSmoke1Type( EMPLACEMENT_RED );
	damageSmoke2Type( EMPLACEMENT_YELLOW );
}


/* End MISSEMP.CPP *****************************************************/
