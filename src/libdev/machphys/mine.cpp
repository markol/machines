/*
 * M I N E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "machphys/mine.hpp"

#include "stdlib/string.hpp"

#include "ctl/countptr.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/radians.hpp"
#include "mathex/vec3.hpp"
#include "phys/rampacce.hpp"
#include "phys/timespin.hpp"
#include "phys/linemoti.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/compplan.hpp"
#include "sim/manager.hpp"

#include "machphys/ofactory.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/minedata.hpp"
#include "machphys/levels.hpp"
#include "machphys/snddata.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysMine );

MachPhysMine::MachPhysMine(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysMineData( part( level ).data(), globalTransform() ) ) )
{
	size_t nFans = part(level).fans_.size();
	fans_.reserve(nFans);

	for( size_t iFan=0; iFan<nFans; ++iFan)
    	fans_.push_back( links()[ (part(level).fans_)[iFan]->id() ] );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a mine from scratch

MachPhysMine::MachPhysMine( W4dEntity* pParent, size_t level )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( level ), wireframeFileName( level ), 21.0, level,
  MachPhysData::instance().mineData( level ) ),
  pData_( _NEW( MachPhysMineData( MachPhysData::instance().mineData( level ), W4dTransform3d() ) ) )
{
	//This function altered by SJA 21/11 to handle cases 1 and 3
    W4dLinks    mineAdornments;
    W4dLink*    pArm;
    W4dLink*    pDecals;

    if( findLink( "arm", &pArm ) )
    	mineAdornments.push_back( pArm );

    if( findLink( "decals", &pDecals ) )
        mineAdornments.push_back( pDecals );

	fans_.reserve( 10 );
	string fanLinkName0 = "fan0";
	for( int i = 1; i<= 10; ++i )
	{
	    // TODO check this
		char textN[4];
//		itoa(i, textN, 10 );
        sprintf(textN, "%d", i);
		string fanLinkName = fanLinkName0 + string(textN);
		if( i == 10 )
		{
			fanLinkName = "fan" + string(textN);
//			itoa(0, textN, 10 );
            sprintf(textN, "%d", 0);
			fanLinkName += string(textN);
		}


		W4dLink* fanLink = NULL;
 	    if( findLink(fanLinkName, &fanLink) )
		{
	    	fans_.push_back( fanLink );
		}
	}

    adornments( mineAdornments );
    TEST_INVARIANT;
}

MachPhysMine::MachPhysMine( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL )
{
}

MachPhysMine::~MachPhysMine()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

// static
MachPhysMine& MachPhysMine::part( size_t level )
{
    return factory().part(
        level,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::MINE, level ) );
}

// static
MachPhysMine::Factory& MachPhysMine::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::MINE ) );

    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysMine::constructionData() const
{
	return data();
}

const MachPhysMineData& MachPhysMine::data() const
{
	return *pData_;
}

SysPathName MachPhysMine::compositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
			//Altered path SJA 19/11 and added case 3
            result = "models/mine/level1/exterior/mn1e.cdf";
            break;

		case 3:
            result = "models/mine/level3/exterior/mn3e.cdf";
            break;

		case 5:
            result = "models/mine/level5/exterior/mn5e.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

SysPathName MachPhysMine::wireframeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
            result = "models/mine/level1/wirefram/mn1w.cdf";
            break;

		case 3:
            result = "models/mine/level3/wirefram/mn3w.cdf";
            break;

		case 5:
            result = "models/mine/level5/wirefram/mn5w.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

void MachPhysMine::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

W4dCompositePlanPtr MachPhysMine::workingAnimationLevel4()
{
    //Find the arm link
    W4dLink* pArmLink;
    bool linkFound = findLink( "minearm04", &pArmLink );
    ASSERT( linkFound, "No arm link in mine level 1" );

    //Construct a timed angle plan for the arm
    PhysTimedSpinPlan* pSpinPlan = _NEW( PhysTimedSpinPlan( MexVec3( 1, 0, 0),
                                         MexVec3(pArmLink->localTransform().position()),
                                         MexRadians( 0 ), MexRadians( 0 ), 5 ) );

    const MATHEX_SCALAR turnAngle = 40;
    const MATHEX_SCALAR maxTurnRate = 10;
    const MATHEX_SCALAR turnAcceleration = 20;
    PhysRelativeTime accelerationTime;
    PhysRelativeTime moveTime = PhysRampAcceleration::totalTime( 0, turnAngle,
                               maxTurnRate, turnAcceleration, &accelerationTime );

    pSpinPlan->addSegment( accelerationTime, MexDegrees( turnAcceleration ) );
    pSpinPlan->addSegment( moveTime - 2 * accelerationTime, 0 );
    pSpinPlan->addSegment( accelerationTime * 2, MexDegrees( -turnAcceleration ) );
    pSpinPlan->addSegment( moveTime - 2 * accelerationTime, 0 );
    pSpinPlan->addSegment( accelerationTime, MexDegrees( turnAcceleration ) );

    //Hence make a composite plan
    PhysMotionPlanPtr spinPlanPtr( pSpinPlan );
    W4dEntityPlan linkPlan;
    linkPlan.absoluteMotion( spinPlanPtr, 0 );
    W4dCompositePlan* pMinePlan = _NEW( W4dCompositePlan( "mine" ) );
    pMinePlan->linkPlan( pArmLink->id(), linkPlan );

    return W4dCompositePlanPtr( pMinePlan );
}

//virtual
void MachPhysMine::doWorking( bool setWorking )
{

	// only do anything if situation has changed
	if( setWorking != isWorking() )
	{
	    //Enable or disable the animation and sound
	    if( not isWorking() )			// setWorking must == true
	    {
	        W4dCompositePlanPtr workingPlanPtr;
	        if( cycleAnims( &workingPlanPtr ) )
	            plan( *workingPlanPtr, SimManager::instance().currentTime(),
	                  1000000, MachPhys::CONSTRUCTION_WORKING );

			SoundId newID = SID_MINE1;

			switch(level())
			{
				case 1:
					newID = SID_MINE1;
					break;
				case 3:
					newID = SID_MINE3;
					break;
				case 5:
					newID = SID_MINE5;
					break;
				default:
					newID = SID_MINE1;
				break;
			}
			W4dSoundManager::instance().stop( this );
			W4dSoundManager::instance().play( this, newID,
	                                  PhysAbsoluteTime(0), 0 );
	        //Construct transform for rotating by 120 degrees
	        //MexTransform3d spinTransform( MexEulerAngles( 0.0, 0.0, MexDegrees(120) ) );
	        MexTransform3d spinTransform( MexEulerAngles( 0.0, 0.0, MexDegrees(120) ) );

			size_t nFans = fans_.size();
			for( size_t iFan = 0; iFan < nFans; ++iFan )
			{
		        if( fans_[iFan] != NULL )
		        {
		            //Set start and first position transforms
		            const MexTransform3d& startPosition = (fans_[iFan])->localTransform();

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
		           	(fans_[iFan])->entityPlanForEdit().absoluteMotion(planPtr,
		            						  SimManager::instance().currentTime(),
		                                      1000000, MachPhys::CONSTRUCTION_WORKING);
				}
	        }
	    }
	    else if( isWorking() )			// setWorking must == false
	    {
	        finishAnimation( MachPhys::CONSTRUCTION_WORKING );

			size_t nFans = fans_.size();
			for( size_t iFan = 0; iFan < nFans; ++iFan )
			{
				ASSERT( fans_[iFan] != NULL, "" );
	    		fans_[iFan]->entityPlanForEdit().clearAnimation( MachPhys::CONSTRUCTION_WORKING );
			}

	        W4dSoundManager::instance().stop( this );
		    W4dSoundManager::instance().play( this, SID_IDLE_CONSTRUCTION,
	                                     PhysAbsoluteTime( 0 ),
	                                     W4dSoundManager::LOOP_CONTINUOUSLY );
	    }
	}
}

void MachPhysMine::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysMineData(
      MachPhysData::instance().mineData( level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysMine& construction )
{
    const MachPhysConstruction& base = construction;

    ostr << base;
    ostr << construction.fans_;
}

void perRead( PerIstream& istr, MachPhysMine& construction )
{
    MachPhysConstruction& base = construction;

    istr >> base;
    istr >> construction.fans_;

    construction.persistenceInitialiseData();
}

void MachPhysMine::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );
	damageSmoke1Type( MINE_YELLOW );
	damageSmoke2Type( PUFF_2 );
}

/* End MINE.CPP *****************************************************/
