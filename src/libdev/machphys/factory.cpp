/*
 * F A C T O R Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"
#include "machphys/factory.hpp"
#include "stdlib/string.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

#include "mathex/point3d.hpp"
#include "phys/rampacce.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/soundman.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/coldata.hpp"
#include "world4d/colpulse.hpp"
#include "world4d/matplan.hpp"
#include "world4d/compplan.hpp"

#include "machphys/factdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/snddata.hpp"

#include "sim/manager.hpp"
#include "phys/linemoti.hpp"
#include "mathex/eulerang.hpp"

PER_DEFINE_PERSISTENT( MachPhysFactory );

MachPhysFactory::MachPhysFactory(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::FactorySubType subType,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( subType, level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysFactoryData( part( subType, level ).data(), globalTransform() ) ) ),
  subType_( subType ),
  level_(level),
  pSmoke1_( NULL ),
  pSmoke2_( NULL ),
  pSmoke3_( NULL ),
  pSmoke4_( NULL ),
  pSmoke5_( NULL ),
  nextSmokeTime_( SimManager::instance().currentTime() )

{
    W4dLink* pFan1 = part(subType, level).pFan1_;
    W4dLink* pFan2 = part(subType, level).pFan2_;
    W4dLink* pFan3 = part(subType, level).pFan3_;
    W4dLink* pFan4 = part(subType, level).pFan4_;

	pFan1_ = (pFan1 ? links()[pFan1->id()] : NULL );
	pFan2_ = (pFan2 ? links()[pFan2->id()] : NULL );
	pFan3_ = (pFan3 ? links()[pFan3->id()] : NULL );
	pFan4_ = (pFan4 ? links()[pFan4->id()] : NULL );

    //Ensure the fans are turning
    //doWorking( true );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a factory from scratch

MachPhysFactory::MachPhysFactory( W4dEntity* pParent, Id id )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
  wireframeFileName( id.subType_, id.level_ ),  10.0, id.level_,
  MachPhysData::instance().factoryData( id.subType_, id.level_ ) ),
  pData_( _NEW( MachPhysFactoryData( MachPhysData::instance().factoryData( id.subType_, id.level_ ), W4dTransform3d() ) ) ),
  subType_( id.subType_ ),
  level_(id.level_),
  pSmoke1_( NULL ),
  pSmoke2_( NULL ),
  pSmoke3_( NULL ),
  pSmoke4_( NULL ),
  pSmoke5_( NULL ),
  nextSmokeTime_( SimManager::instance().currentTime() )
{
    if(!findLink("fan01", &pFan1_))
    	pFan1_ = NULL;

    if(!findLink("fan02", &pFan2_))
    	pFan2_ = NULL;

    if(!findLink("fan03", &pFan3_))
    	pFan3_ = NULL;

    if(!findLink("fan04", &pFan4_))
    	pFan4_ = NULL;

    TEST_INVARIANT;
}

MachPhysFactory::MachPhysFactory( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL ),
  pFan1_( NULL ),
  pFan2_( NULL ),
  pFan3_( NULL ),
  pFan4_( NULL ),
  pSmoke1_( NULL ),
  pSmoke2_( NULL ),
  pSmoke3_( NULL ),
  pSmoke4_( NULL ),
  pSmoke5_( NULL ),
  nextSmokeTime_( SimManager::instance().currentTime() )
{
}

MachPhysFactory::~MachPhysFactory()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

// static
MachPhysFactory& MachPhysFactory::part( MachPhys::FactorySubType subType, size_t hardwareLevel )
{
    return factory().part(
        Id( subType, hardwareLevel ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, hardwareLevel ) );
}

// static
MachPhysFactory::Factory& MachPhysFactory::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::FACTORY ) );

    return factory_;
}

MachPhys::FactorySubType MachPhysFactory::subType( void ) const
{
    return subType_;
}

//virtual
const MachPhysConstructionData& MachPhysFactory::constructionData() const
{
	return data();
}

const MachPhysFactoryData& MachPhysFactory::data() const
{
	return *pData_;
}

SysPathName MachPhysFactory::compositeFileName( MachPhys::FactorySubType subType, size_t level ) const
{
    SysPathName result;

	//New switch (for new paths) added by SJA 19/11
	switch( subType )
	{
		case MachPhys::MILITARY:
		{
			switch( level )
		    {
        		case 1:
	        	    result = "models/factory/military/level1/exterior/fam1e.cdf";
	            	break;

        		case 3:
	        	    result = "models/factory/military/level3/exterior/fam3e.cdf";
	            	break;

        		case 4:
	        	    result = "models/factory/military/level4/exterior/fam4e.cdf";
	            	break;

        		case 5:
	        	    result = "models/factory/military/level5/exterior/fam5e.cdf";
	            	break;

		        default:
        	    	ASSERT_BAD_CASE_INFO( level );
					break;
			}
            break;
		}

		case MachPhys::CIVILIAN:
		{
			switch( level )
		    {
        		case 1:
	        	    result = "models/factory/civilian/level1/exterior/fac1e.cdf";
					break;

        		case 3:
	        	    result = "models/factory/civilian/level3/exterior/fac3e.cdf";
					break;

        		case 5:
	        	    result = "models/factory/civilian/level5/exterior/fac5e.cdf";
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

SysPathName MachPhysFactory::wireframeFileName( MachPhys::FactorySubType subType, size_t level ) const
{
    //Wire frames are obsolete
    SysPathName result;
    return result;
}

void MachPhysFactory::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysFactory::doWorking( bool setWorking )
{
	enum { CHIM_HOT };
    //Enable or disable the animation and sound
    if( setWorking )
    {
     	if( not isWorking() )
	    {
			PhysAbsoluteTime now = SimManager::instance().currentTime();
	        W4dCompositePlanPtr workingPlanPtr;
	        if( cycleAnims( &workingPlanPtr ) )
	            plan( *workingPlanPtr, now,
	                  1000000, MachPhys::CONSTRUCTION_WORKING );

			//materials with texture chimhot1.bmp pulse colour from RenColour(0.5, 0.5, 0.5) to RenCOlour( 1, 1, 1) every 0.2 sec
			static W4dColourPulseData pulseData( "chimhot1.bmp", RenColour(0.5, 0.5, 0.5), RenColour( 1, 1, 1), 0.2 );
			static ctl_pvector< W4dColourPulseData > dataVec;
			static bool first = true;
			if( first )
			{
				dataVec.reserve(1);
				dataVec.push_back ( &pulseData );
			}

			size_t nLinks = links().size();

			for( size_t iLink=0; iLink<nLinks; ++iLink)
			{
				W4dLink* pLink = links()[iLink];
				if ( pLink->hasMesh() )
				{
					W4dColourPulsePlan::makePlan( pLink, dataVec, now, CHIM_HOT);
				}
			}

	        //Set up a plan for each fan

	        //Construct transform for rotating by 120 degrees
	        MexTransform3d spinTransform( MexEulerAngles( 0.0, MexDegrees(120), 0.0 ) );

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

	        if( pFan3_ )
	        {
	            //Set start and first position transforms
	            const MexTransform3d& startPosition = pFan3_->localTransform();
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
	           	pFan3_->entityPlanForEdit().absoluteMotion(planPtr,
	            						  SimManager::instance().currentTime(),
	                                      1000000);
	        }

	        if( pFan4_ )
	        {
	            //Set start and first position transforms
	            const MexTransform3d& startPosition = pFan4_->localTransform();
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
	           	pFan4_->entityPlanForEdit().absoluteMotion(planPtr,
	            						  SimManager::instance().currentTime(),
	                                      1000000);
	        }

            //Set the working sound going
			if(subType() == MachPhys::MILITARY)
			{
			    W4dSoundManager::instance().play( this, SID_MILITARYFACTORY,
	    	   			                          PhysAbsoluteTime( 0 ), 0 );
			}
			else
			{
			    W4dSoundManager::instance().play( this, SID_CIVILIANFACTORY,
	           			                          PhysAbsoluteTime( 0 ), 0 );
			}
	    }

		// deal with the possibility of belching smoke out even if we're not setting the working status
		// to true for the first time

		dealWithSmoke();
	}


    else if( isWorking() and setWorking == false )
    {
        clearAnimation( CHIM_HOT );
        finishAnimation( MachPhys::CONSTRUCTION_WORKING );
        W4dSoundManager::instance().stop( this );
	    W4dSoundManager::instance().play( this, SID_IDLE_CONSTRUCTION,
                                     PhysAbsoluteTime( 0 ),
                                     W4dSoundManager::LOOP_CONTINUOUSLY );
    }
}

void MachPhysFactory::dealWithSmoke()
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	PhysRelativeTime smokeDuration = 40.0; //seconds

	if( timeNow >= nextSmokeTime_ )
	{
		nextSmokeTime_ = timeNow + smokeDuration;

		//light smoke emitting from chimneys
		switch( subType() )
		{
			case MachPhys::MILITARY:
			{

				switch( level_ )
			    {
					case 1:
					{
						//2 chimneys

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 2.2, -6.4, 12 ),  		//start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_6,								  //white puff
							   						            smokeDuration ) );
						}

						if(pSmoke2_==NULL)
						{
							pSmoke2_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 2.2, 6.4, 12 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_6,								  //white puff
							   						            smokeDuration ) );
						}

						pSmoke1_->startSmokePlume( timeNow );
						pSmoke2_->startSmokePlume( timeNow );

						break;
					}

	        		case 3:
					{
						//one chimney

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -5.3, 0, 24.1 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        3,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_6,								  //white puff
							   						            smokeDuration ) );
						}

						pSmoke1_->startSmokePlume( timeNow );
		            	break;
					}

		        	case 4:
					{
                        //TBD: add required data here
		            	break;
					}

	        		case 5:
					{
						//two chimneys

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 0.7, -19.7, 38.3 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        3,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_6,								  //white puff
							   						            smokeDuration ) );
						}

						if(pSmoke2_==NULL)
						{
							pSmoke2_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 0.7, 20.5, 38.3 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        3,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_6,								  //white puff
							   						            smokeDuration ) );

						}

						pSmoke1_->startSmokePlume( timeNow );
				    	pSmoke2_->startSmokePlume( timeNow );
		            	break;
					}
			        default:
	        	    	ASSERT_BAD_CASE_INFO( level_ );
						break;

				}

				break;
			}

			case MachPhys::CIVILIAN:
			{
				switch( level_ )
			    {
	        		case 1:
					{
						//3 chimneys

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -6.2, -5.6, 22.8 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );
						}

						if(pSmoke2_==NULL)
						{
							pSmoke2_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -1.0, -5.6, 22.8 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );
						}

						if(pSmoke3_==NULL)
						{
							pSmoke3_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -1.8, 13.6, 24.1 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        3,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );
						}

						pSmoke1_->startSmokePlume( timeNow );
				    	pSmoke2_->startSmokePlume( timeNow );
				    	pSmoke3_->startSmokePlume( timeNow );
		            	break;
					}

	        		case 3:
					{
						//three chimneys

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -2.5, 7, 27.5 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						if(pSmoke2_==NULL)
						{
							pSmoke2_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -10, -11.6, 29 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        3,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						if(pSmoke3_==NULL)
						{
							pSmoke3_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 11.8, 14.8, 29 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						pSmoke1_->startSmokePlume( timeNow );
				    	pSmoke2_->startSmokePlume( timeNow );
				    	pSmoke3_->startSmokePlume( timeNow );
		            	break;
					}
	        		case 5:
					{
						//five chimneys

						if(pSmoke1_==NULL)
						{
							pSmoke1_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -2.8, 7.7, 34.6 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						if(pSmoke2_==NULL)
						{
							pSmoke2_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( -4.5, -14.5, 28.1 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						if(pSmoke3_==NULL)
						{
							pSmoke3_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 4.3, -14.5, 28.1 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}

						if(pSmoke4_==NULL)
						{
							pSmoke4_ = _NEW( MachPhysSmokePlume(this,								  //parent
																MexPoint3d( 13.2, -14.5, 28.1 ),  //start Point
														        40,									  //MaxHeight
														        0,									  //zDepthOffset
														        2,									  //nWisps
														        12.5,								  //wispSize
														        PUFF_5,								  //white puff
							   						            smokeDuration ) );

						}


					   	pSmoke1_->startSmokePlume( timeNow );
				    	pSmoke2_->startSmokePlume( timeNow );
				    	pSmoke3_->startSmokePlume( timeNow );
				    	pSmoke4_->startSmokePlume( timeNow );

		            	break;
					}

			        default:
	        	    	ASSERT_BAD_CASE_INFO( level_ );
						break;
				}

				break;
			}

	        default:
    	    	ASSERT_BAD_CASE_INFO( subType() );
				break;

		}
	}
}

void MachPhysFactory::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysFactoryData(
      MachPhysData::instance().factoryData( subType(), level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysFactory& construction )
{
    const MachPhysConstruction& base = construction;
    ostr << base;

    ostr << construction.subType_;
    ostr << construction.pFan1_;
    ostr << construction.pFan2_;
    ostr << construction.pFan3_;
    ostr << construction.pFan4_;
    ostr << construction.level_;
}

void perRead( PerIstream& istr, MachPhysFactory& construction )
{
    MachPhysConstruction& base = construction;
    istr >> base;

    istr >> construction.subType_;
    istr >> construction.pFan1_;
    istr >> construction.pFan2_;
    istr >> construction.pFan3_;
    istr >> construction.pFan4_;
    istr >> construction.level_;

    construction.persistenceInitialiseData();
}

void MachPhysFactory::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );


	switch ( subType() )
	{

		case MachPhys::MILITARY:

			damageSmoke1Type( M_FACTORY_GREEN );
			damageSmoke2Type( M_FACTORY_BROWN );
			break;

		case MachPhys::CIVILIAN:

			damageSmoke1Type( C_FACTORY_GREEN );
			damageSmoke2Type( C_FACTORY_WHITE );
			break;
	}
}

/* End FACTORY.CPP *****************************************************/
