/*
 * S M E L T E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/smelter.hpp"

#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "world4d/soundman.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/compplan.hpp"
#include "sim/manager.hpp"

#include "machphys/ofactory.hpp"
#include "machphys/entrance.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/smeldata.hpp"
#include "machphys/levels.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/snddata.hpp"

#include "system/pathname.hpp"


PER_DEFINE_PERSISTENT( MachPhysSmelter );

MachPhysSmelter::MachPhysSmelter(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction(
    part( level ),
    pParent,
    localTransform,
    level,
    race ),
  pData_( _NEW( MachPhysSmelterData( part( level ).data(), globalTransform() ) ) ),
  pSmoke_(NULL),
  lastSmokePlumeTime_( 0.0 ),
  level_(level)
{
    W4dLink* pFan1 = part(level).pFan1_;
    W4dLink* pFan2 = part(level).pFan2_;
	pFan1_ = (pFan1 ? links()[pFan1->id()] : NULL );
	pFan2_ = (pFan2 ? links()[pFan2->id()] : NULL );

    turnFansOn();

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a smelter from scratch

MachPhysSmelter::MachPhysSmelter( W4dEntity* pParent, size_t level )
: MachPhysConstruction(
    pParent,
    W4dTransform3d(),
    compositeFileName( level ),
	wireframeFileName( level ),
    interiorCompositeFileName( level ),
    50.0,
    level,
    MachPhysData::instance().smelterData( level ) ),
  pData_( _NEW( MachPhysSmelterData( MachPhysData::instance().smelterData( level ), W4dTransform3d() ) ) ),
  lastSmokePlumeTime_( 0.0 ),
  level_(level)
{
    if(!findLink("fan01", &pFan1_))
    	pFan1_ = NULL;

    if(!findLink("fan02", &pFan2_))
    	pFan2_ = NULL;

    TEST_INVARIANT;
}

MachPhysSmelter::MachPhysSmelter( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL ),
  pFan1_( NULL ),
  pFan2_( NULL ),
  pAnglePlan1_( NULL ),
  pAnglePlan2_( NULL ),
  pSmoke_( NULL )
{
}

MachPhysSmelter::~MachPhysSmelter()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

// static
MachPhysSmelter& MachPhysSmelter::part( size_t hardwareLevel )
{
    return factory().part(
        hardwareLevel,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::SMELTER, hardwareLevel ) );
}

// static
MachPhysSmelter::Factory& MachPhysSmelter::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::SMELTER ) );

    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysSmelter::constructionData() const
{
	return data();
}

const MachPhysSmelterData& MachPhysSmelter::data() const
{
	return *pData_;
}

SysPathName MachPhysSmelter::compositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/smelter/level1/exterior/sm1e.cdf";
            break;

        case 3:
            result = "models/smelter/level3/exterior/sm3e.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

SysPathName MachPhysSmelter::wireframeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
            result = "models/smelter/level1/wirefram/sm1w.cdf";
            break;

        case 3:
            result = "models/smelter/level3/wirefram/sm3w.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

SysPathName MachPhysSmelter::interiorCompositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/smelter/level1/interior/sm1i.cdf";
            break;

        case 3:
            result = "models/smelter/level3/interior/sm3i.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

void MachPhysSmelter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysSmelter::doWorking( bool setWorking )
{
	// only do anything if situation has changed
	if( setWorking != isWorking() )
	{
	    //Enable or disable the animation and sound
	    if( not isWorking() )		// setWorking must == true
	    {
	        W4dCompositePlanPtr workingPlanPtr;
	        if( cycleAnims( &workingPlanPtr ) )
	            plan( *workingPlanPtr, SimManager::instance().currentTime(),
	                  1000000, MachPhys::CONSTRUCTION_WORKING );

			SoundId newID = SID_SMELTER1;

			switch(level())
			{
				case 1:
					newID = SID_SMELTER1;
					break;
				case 3:
					newID = SID_SMELTER3;
					break;
				default:
				break;
			}

			W4dSoundManager::instance().stop( this );
			W4dSoundManager::instance().play( this, newID, PhysAbsoluteTime( 0 ), 0 );

	 	    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

			MexPoint3d offset;
			switch( level_ )
			{
				case 1:
					offset = MexPoint3d(1.7, 0.9, 19);
					break;
				case 3:
					offset = MexPoint3d(0, 0, 16.0) ; //(-2.0, 2.7, 30.6);

					break;
				default:
					ASSERT_BAD_CASE_INFO( level_ );
					break;
			}

			if(!pSmoke_)
			{
				pSmoke_ =  _NEW(MachPhysSmokePlume(
				    	this,
						offset,
				        40,
				        0,
				        5,
				        12.5,
				        PUFF_2,
				        45));

		    	pSmoke_->startSmokePlume( timeNow );
			}

			if(timeNow > (lastSmokePlumeTime_ + 50) )
			{
		    	pSmoke_->startSmokePlume( timeNow );
				lastSmokePlumeTime_ = timeNow;
			}
	   	}
	    else if( isWorking() )		// setWorking must == false
	    {
	        finishAnimation( MachPhys::CONSTRUCTION_WORKING );

	        W4dSoundManager::instance().stop( this );
			W4dSoundManager::instance().play( this, SID_IDLE_CONSTRUCTION, PhysAbsoluteTime( 0 ), 0 );
	    }
	}
}

void MachPhysSmelter::turnFansOn()
{
    //Construct transform for rotating by 120 degrees
    MexTransform3d spinTransform( MexEulerAngles( 0.0, MexDegrees(120), 0.0 ) );

    //Set up a plan for each fan
    if( pFan1_ )
    {
        //Set start and first position transforms
        const MexTransform3d& startPosition = pFan1_->localTransform();
        MexTransform3d endPosition( startPosition );
        endPosition.transform( spinTransform );

        PhysLinearMotionPlan* pPlan =
            _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

        //Add a further rotation of 120 degrees
        endPosition.transform( spinTransform );
        pPlan->add( endPosition, 0.5 );

        //Finally move back to start position
        pPlan->add( startPosition, 0.75 );

        //Add the plan
	    PhysMotionPlanPtr planPtr( pPlan );
       	pFan1_->entityPlanForEdit().absoluteMotion(planPtr,
        						  SimManager::instance().currentTime(),
                                  1000000);
    }

    if( pFan2_ )
    {
        //Set start and first position transforms
        const MexTransform3d& startPosition = pFan2_->localTransform();
        MexTransform3d endPosition( startPosition );
        endPosition.transform( spinTransform );

        PhysLinearMotionPlan* pPlan =
            _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

        //Add a further rotation of 120 degrees
        endPosition.transform( spinTransform );
        pPlan->add( endPosition, 0.5 );

        //Finally move back to start position
        pPlan->add( startPosition, 0.75 );

        //Add the plan
	    PhysMotionPlanPtr planPtr( pPlan );
       	pFan2_->entityPlanForEdit().absoluteMotion(planPtr,
        						  SimManager::instance().currentTime(),
                                  1000000);
    }
}

void MachPhysSmelter::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysSmelterData( MachPhysData::instance().smelterData( level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysSmelter& construction )
{
    const MachPhysConstruction& base = construction;
    ostr << base;

	ostr << construction.pFan1_;
	ostr << construction.pFan2_;
	//ostr << construction.pAnglePlan1_;
	//ostr << construction.pAnglePlan2_;

	//ostr << construction.pSmoke_;
    //ostr << construction.lastSmokePlumeTime_;

	ostr << construction.level_;
}

void perRead( PerIstream& istr, MachPhysSmelter& construction )
{
    MachPhysConstruction& base = construction;
    istr >> base;

	istr >> construction.pFan1_;
	istr >> construction.pFan2_;
	//istr >> construction.pAnglePlan1_;
	//istr >> construction.pAnglePlan2_;

	//istr >> construction.pSmoke_;
    //istr >> construction.lastSmokePlumeTime_;

	istr >> construction.level_;

    construction.persistenceInitialiseData();
}

void MachPhysSmelter::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );
	damageSmoke1Type( SMELTER_PURPLE );
	damageSmoke2Type( SMELTER_RED );
}


/* End SMELTER.CPP *****************************************************/
