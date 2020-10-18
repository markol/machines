/*
 * P O D . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/pod.hpp"

#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"
#include "phys/timeangl.hpp"
#include "world4d/soundman.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "sim/manager.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/entrance.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/poddata.hpp"
#include "machphys/levels.hpp"
#include "machphys/armourer.hpp"
#include "machphys/snddata.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysPod );

MachPhysPod::MachPhysPod(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( level ), pParent, localTransform, level, race ),
  MachPhysCanAttack( part( level ), this ),
  pData_( _NEW( MachPhysPodData( part( level ).data(), globalTransform() ) ) )
{
    //Register its sound
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/pod.wav" ),
//                                      PhysAbsoluteTime( 0 ), 100.0, 35.0,
//                                      W4dSoundManager::LOOP_CONTINUOUSLY );
    W4dSoundManager::instance().play( this, SID_POD,
                                      PhysAbsoluteTime( 0 ),
                                      (size_t)0 );

	pTurner_ = links()[part( level).pTurner_->id()];
	pTop_ = links()[part( level).pTop_->id()];
	pIonTop_ = links()[part( level).pIonTop_->id()];

	pTurner_->visible( true );
	pTop_->visible( true );
	pIonTop_->visible( false );

    //Mount the ion cannon
    MachPhysArmourer::fitWeapons( this, this, MachPhys::T_ION_ORBITAL_CANNON );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a pod from scratch

MachPhysPod::MachPhysPod( W4dEntity* pParent, size_t level )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( level ), wireframeFileName( level ), 
  interiorCompositeFileName( level ), 50.0, level,  MachPhysData::instance().podData( level ) ),
  pData_( _NEW( MachPhysPodData( MachPhysData::instance().podData( level ), W4dTransform3d() ) ) )
{
    PRE( level == 1 );

    //Add a dummy gun link for mounting the ion cannon weapon on
    W4dLink* pGunLink = addLink( MexTransform3d(), W4dEntity::NOT_SOLID );
    pGunLink->name( "gun" );
    pGunLink->visible( false );

    if(!findLink("turner", &pTurner_))
    	pTurner_ = NULL;
	ASSERT(pTurner_, "Can't find turner mesh");

    if(!findLink("top", &pTop_))
    	pTop_ = NULL;
	ASSERT(pTop_, "Can't find top mesh");

    if(!findLink("ion_top", &pIonTop_))
    	pIonTop_ = NULL;
	ASSERT(pIonTop_, "Can't find ion_top mesh");

    //Set up the mounting link info for attaching weapons
    initialiseMountingLinks();
    
    TEST_INVARIANT;
}

MachPhysPod::MachPhysPod( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL ),
  pTurner_( NULL ),
  pTop_( NULL ),
  pIonTop_( NULL )
{
}

MachPhysPod::~MachPhysPod()
{
    TEST_INVARIANT;
    _DELETE( pData_ );
}

// static
MachPhysPod& MachPhysPod::part( size_t level )
{
    return factory().part(
        level,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::POD, level ) );
}

// static
MachPhysPod::Factory& MachPhysPod::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::POD ) );
    
    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysPod::constructionData() const
{
	return data();
}

const MachPhysPodData& MachPhysPod::data() const
{
	return *pData_;
}

SysPathName MachPhysPod::compositeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
            result = "models/pod/level1/exterior/pd1e.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

SysPathName MachPhysPod::wireframeFileName( size_t ) const
{
	//Pods are not built so do not have wireframes
    return SysPathName();
}


SysPathName MachPhysPod::interiorCompositeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
            result = "models/pod/level1/interior/pd1i.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

void MachPhysPod::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysPod::doWorking(bool setWorking)
{
	// only do anything if situation has changed
	if( setWorking != isWorking() )
	{
		if( not isWorking() )		// setWorking must == true
	    {
	       	PhysMotionPlan::Times* pTimesList = _NEW( PhysMotionPlan::Times );
            pTimesList->reserve( 3 );
	       	pTimesList->push_back(1.2);
	       	pTimesList->push_back(2.4);
	       	pTimesList->push_back(3.6);

	      	PhysMotionPlan::AnglesPtr anglesListPtr = PhysMotionPlan::AnglesPtr( _NEW( PhysMotionPlan::Angles ) );
            anglesListPtr->reserve( 4 );
	       	anglesListPtr->push_back(MexDegrees(0));
	      	anglesListPtr->push_back(MexDegrees(120));
	       	anglesListPtr->push_back(MexDegrees(240));
	       	anglesListPtr->push_back(MexDegrees(360));


	        MexVec3 newvec(0, 0, 1);
	        MexVec3 position( 0,0,0 );

			ASSERT( (pTop_->visible() and not pIonTop_->visible() ) or
			        (not pTop_->visible() and pIonTop_->visible() ), "Only one is visible.");

	        position = pTurner_->localTransform().position();
	      	PhysTimedAnglePlan* pAnglePlan = _NEW(PhysTimedAnglePlan(anglesListPtr, 
	   						 					 PhysMotionPlan::TimesPtr( pTimesList ), 
	      						 				 newvec,
	      						 				 position));

			PhysMotionPlanPtr anglePlanPtr( pAnglePlan );

	       	pTurner_->entityPlanForEdit().absoluteMotion(anglePlanPtr, 
	       						  SimManager::instance().currentTime(),
	                              1000000, MachPhys::CONSTRUCTION_WORKING );
	    }
	    else if( isWorking() )			// setWorking must == false
	    {
	        finishAnimation( MachPhys::CONSTRUCTION_WORKING );
		    W4dSoundManager::instance().stop( this );
	    }
	}
}

//virtual
W4dComposite& MachPhysPod::asComposite()
{
    return _STATIC_CAST( W4dComposite&, *this );
}

//virtual
const W4dComposite& MachPhysPod::asComposite() const
{
    return _STATIC_CAST( const W4dComposite&, *this );
}

void MachPhysPod::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysPodData( 
      MachPhysData::instance().podData( level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysPod& construction )
{
    const MachPhysConstruction& base1 = construction;
    const MachPhysCanAttack& base2 = construction;

    ostr << base1;
    ostr << base2;

    ostr << construction.pTop_;
    ostr << construction.pIonTop_;
    ostr << construction.pTurner_;
}

void perRead( PerIstream& istr, MachPhysPod& construction )
{
    MachPhysConstruction& base1 = construction;
    MachPhysCanAttack& base2 = construction;

    istr >> base1;
    istr >> base2;

    istr >> construction.pTop_;
    istr >> construction.pIonTop_;
    istr >> construction.pTurner_;

    construction.persistenceInitialiseData();
}

void MachPhysPod::ionCannonResearched( bool researched )
{
	if( researched )
	{
		pIonTop_->visible( true );
		pTop_->visible( false );
	}
	else
	{
		pIonTop_->visible( false );
		pTop_->visible( true );
	}
}

//virtual
bool MachPhysPod::canTrackWeaponBase() const
{
    return false;
}

void MachPhysPod::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );
	damageSmoke1Type( POD_GREEN );
	damageSmoke2Type( POD_PURPLE );
}


/* End POD.CPP *****************************************************/
