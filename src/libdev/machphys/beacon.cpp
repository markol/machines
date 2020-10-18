/*
 * B E A C O N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/beacon.hpp"

#include "stdlib/string.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "phys/rampacce.hpp"

#include "world4d/soundman.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"

#include "machphys/beacdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/snddata.hpp"
#include "machphys/ofactory.hpp"

#include "sim/manager.hpp"

#include "phys/timeangl.hpp"

//#include "mathex

PER_DEFINE_PERSISTENT( MachPhysBeacon );

MachPhysBeacon::MachPhysBeacon(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysBeaconData( part( level ).data(), globalTransform() ) ) )
{

	pTurner_ = links()[part( level).pTurner_->id()];
	pTurner_->visible( true );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a beacon from scratch

MachPhysBeacon::MachPhysBeacon( W4dEntity* pParent, size_t level )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( level ), 
	wireframeFileName( level ),10.0, level,
    MachPhysData::instance().beaconData( level ) ),
  pData_( _NEW( MachPhysBeaconData( MachPhysData::instance().beaconData( level ), W4dTransform3d() ) ) )
{
    if(!findLink("Turner", &pTurner_))
    	pTurner_ = NULL;
	ASSERT(pTurner_, "Can't find turner mesh");

    TEST_INVARIANT;
}

MachPhysBeacon::MachPhysBeacon( PerConstructor con )
: MachPhysConstruction( con ), 
  pData_( NULL ),
  pTurner_( NULL )
{
}

MachPhysBeacon::~MachPhysBeacon()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

// static
MachPhysBeacon& MachPhysBeacon::part( size_t level )
{
    return factory().part(
        level,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::BEACON, level ) );
}

// static
MachPhysBeacon::Factory& MachPhysBeacon::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::BEACON ) );
    
    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysBeacon::constructionData() const
{
	return data();
}

const MachPhysBeaconData& MachPhysBeacon::data() const
{
	return *pData_;
}

SysPathName MachPhysBeacon::compositeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
			//Line commented by SJA 19/11 and added case 2 and 3
            //result = "models/beacon/level3/bk3.cdf";
			result = "models/beacon/level1/exterior/bk1e.cdf";
            break;

		case 2:
			result = "models/beacon/level2/exterior/bk2e.cdf";
            break;

		case 3:
			result = "models/beacon/level3/exterior/bk3e.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

SysPathName MachPhysBeacon::wireframeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
			result = "models/beacon/level1/wirefram/bk1w.cdf";
            break;

		case 2:
			result = "models/beacon/level2/wirefram/bk2w.cdf";
            break;

		case 3:
			result = "models/beacon/level3/wirefram/bk3w.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

void MachPhysBeacon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysBeacon::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysBeaconData(
      MachPhysData::instance().beaconData( level() ), W4dTransform3d() ) );
      
    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysBeacon& construction )
{
    const MachPhysConstruction& base = construction;
    ostr << base;
    ostr << construction.pTurner_;
}

void perRead( PerIstream& istr, MachPhysBeacon& construction )
{
    MachPhysConstruction& base = construction;
    istr >> base;
	istr >> construction.pTurner_;

    construction.persistenceInitialiseData();
}

void MachPhysBeacon::doWorking( bool setWorking )
{
	if( setWorking and not isWorking() )
    {
       	PhysMotionPlan::Times* pTimesList = _NEW( PhysMotionPlan::Times );
       	pTimesList->push_back(1.2);
       	pTimesList->push_back(2.4);
       	pTimesList->push_back(3.6);

      	PhysMotionPlan::AnglesPtr anglesListPtr = PhysMotionPlan::AnglesPtr( _NEW( PhysMotionPlan::Angles ) );
       	anglesListPtr->push_back(MexDegrees(0));
      	anglesListPtr->push_back(MexDegrees(120));
       	anglesListPtr->push_back(MexDegrees(240));
       	anglesListPtr->push_back(MexDegrees(360));


        MexVec3 newvec(0, 0, 1);
        MexVec3 position( 0,0,0 );

        position = pTurner_->localTransform().position();
      	PhysTimedAnglePlan* pAnglePlan = _NEW(PhysTimedAnglePlan(anglesListPtr, 
   						 					 PhysMotionPlan::TimesPtr( pTimesList ), 
      						 				 newvec,
      						 				 position));

		PhysMotionPlanPtr anglePlanPtr( pAnglePlan );

       	pTurner_->entityPlanForEdit().absoluteMotion(anglePlanPtr, 
       						  SimManager::instance().currentTime(),
                              1000000, MachPhys::CONSTRUCTION_WORKING );

		SoundId newID = SID_BEACON1;

		switch(level())
		{
			case 1:
				newID = SID_BEACON1;
				break;
			case 3:
				newID = SID_BEACON3;
				break;
			default:
				newID = SID_BEACON1;
				break;
		}

	    W4dSoundManager::instance().play( this, newID,
                                     PhysAbsoluteTime( 0 ),
                                     W4dSoundManager::LOOP_CONTINUOUSLY );

    }
    else if( isWorking() and setWorking == false )
    {
        finishAnimation( MachPhys::CONSTRUCTION_WORKING );
		W4dSoundManager::instance().stop( this );
    }
}

void MachPhysBeacon::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );
	damageSmoke1Type( BEACON_RED );
	damageSmoke2Type( BEACON_AQUA );
}


/* End BEACON.CPP *****************************************************/
