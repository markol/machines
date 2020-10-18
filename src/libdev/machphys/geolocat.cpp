/*
 * G E O L O C A T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/internal/geolocai.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "ctl/list.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"

#include "phys/lsclplan.hpp"
#include "render/colour.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/link.hpp"
#include "world4d/composit.hpp"
#include "world4d/entity.hpp"
#include "world4d/soundman.hpp"
#include "world4d/garbcoll.hpp"

#include "machphys/mexpdata.hpp"
#include "machphys/glocdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/locator.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysGeoLocator );

MachPhysGeoLocator::MachPhysGeoLocator(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race )
: MachPhysMachine( part( bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
   MachPhysData::instance().geoLocatorData( bodyLevel, brainLevel ) ),
   pImpl_( _NEW( MachPhysGeoLocatorImpl ) )
{
    CB_DEPIMPL( W4dCompositePlanPtr, locatingPlanPtr_ );

    if( findCompositePlan( "geo1_Anim1", &locatingPlanPtr_ ))
		cycleAnims( &locatingPlanPtr_ );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a locator from scratch

MachPhysGeoLocator::MachPhysGeoLocator( W4dEntity* pParent, size_t bodyLevel )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( bodyLevel ),
                   MachPhysData::instance().geoLocatorData( bodyLevel, 1 ) ),
   pImpl_( _NEW( MachPhysGeoLocatorImpl ) )
{
    createExplosionData();

    TEST_INVARIANT;
}

MachPhysGeoLocator::MachPhysGeoLocator( PerConstructor con )
: MachPhysMachine( con ),
    pImpl_( NULL )
{
}

MachPhysGeoLocator::~MachPhysGeoLocator()
{
    TEST_INVARIANT;
    _DELETE( pImpl_ );
}

SysPathName MachPhysGeoLocator::compositeFileName( size_t bodyLevel ) const
{
    SysPathName result;

    switch( bodyLevel )
    {
        case 1:
            result = "models/locator/geo/level1/log1.cdf";
            break;

        case 2:
            result = "models/locator/geo/level2/log2.cdf";
            break;

        case 3:
            result = "models/locator/geo/level3/log3.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( bodyLevel );
            break;
    }

    return result;
}

// static
MachPhysGeoLocator& MachPhysGeoLocator::part( size_t hardwareLevel )
{
    return factory().part(
        hardwareLevel,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::GEO_LOCATOR, hardwareLevel ) );
}

// static
MachPhysGeoLocator::Factory& MachPhysGeoLocator::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::GEO_LOCATOR ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysGeoLocator::machineData( void ) const
{
	return data();
}

const MachPhysGeoLocatorData& MachPhysGeoLocator::data( void ) const
{
	return MachPhysData::instance().geoLocatorData( bodyLevel(), brainLevel() );
}

void MachPhysGeoLocator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGeoLocator& t )
{

    o << "MachPhysGeoLocator " << (void*)&t << " start" << std::endl;
    o << "MachPhysGeoLocator " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysGeoLocator::isLocating( bool doLocate )
{
    CB_DEPIMPL( bool, isLocating_ );
	CB_DEPIMPL( MachPhysLocator*, pLocator_ );
	CB_DEPIMPL( MATHEX_SCALAR, locatorSize_ );

    if( doLocate != isLocating_ )
    {
        //Set mode flag
        isLocating_ = doLocate;

		if(isLocating_)
		{
			if( pLocator_ == NULL )
			{
				W4dLink* pHip;
				if( findLink( "hip", &pHip ) or findLink( "hips", &pHip) )
				{
					const MexAlignedBox3d& hipBox = pHip->boundingVolume();
					locatorSize_ = MachPhysData::instance().geoLocatorData( bodyLevel(), brainLevel() ).scannerRange();

					//locator position in the hip system
					MexPoint3d position(0, 0, 0.5*hipBox.zLength());
					//transform to the global system
					pHip->globalTransform().transform(&position);

					//transform to the machine system
					globalTransform().transformInverse( &position );

					pLocator_ = _NEW( MachPhysLocator( this,  position ) );
				}
				else
				{
					pHip = NULL;
					ASSERT( pHip, "hip not found" );
				}
			}

			ASSERT( pLocator_, " ");
			pLocator_->startLocate( SimManager::instance().currentTime(), 5.0, locatorSize_, 10000, LOCATOR_LOCATING );

			//The locator sound is attached to the first link in the list,
			//so that it is not stopped when the locomotion sounds
			//are stopped
			//SOUND_STREAM("Locating for " << _STATIC_CAST(int, links()[0]) << std::endl);
			SOUND_STREAM("Locating for " << _REINTERPRET_CAST(size_t, links()[0]) << std::endl);
			W4dSoundManager::instance().play(links()[0], SID_GEOLOCATE, PhysAbsoluteTime(0), 0);
		}
		else
		{
			if( pLocator_ )
			{
				//pLocator_->entityPlanForEdit().clearAnimation( LOCATOR_LOCATING );
				//pLocator_->visible( false );
				_DELETE( pLocator_ );
				pLocator_ = NULL;
			}

			//SOUND_STREAM("Stopped locating for " << _STATIC_CAST(int, links()[0]) << std::endl);
			SOUND_STREAM("Stopped locating for " << _REINTERPRET_CAST(size_t, links()[0]) << std::endl);
			W4dSoundManager::instance().stop(links()[0]);
		}

        //Modify walking speed - set to half normal when locating
        maxTranslationSpeed( maxTranslationSpeed() * (doLocate ? 0.5 : 2.0) );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachPhysGeoLocator::isLocating() const
{
    CB_DEPIMPL( bool, isLocating_ );

    return isLocating_;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachPhysGeoLocator::move( const MachPhysMachineMoveInfo& info )
{
    CB_DEPIMPL( bool, isLocating_ );
    CB_DEPIMPL( W4dCompositePlanPtr, locatingPlanPtr_ );
    CB_DEPIMPL( PhysAbsoluteTime, locatingPlanEndTime_ );

    //Use the generic method to set up basic move animations
    MachPhysMachine::move( info );

    //Get the data we need from info
    PhysRelativeTime interval = info.totalTime();

    MATHEX_SCALAR startTime = info.startTime();

    //If locating, need to ensure we have a locating animation to cover the move period
    PhysAbsoluteTime moveEndTime = startTime + interval;
    if( isLocating_ and locatingPlanPtr_.isDefined() and (moveEndTime > locatingPlanEndTime_) )
    {
        //Get time to start new locating animation
        PhysAbsoluteTime locatingStartTime = locatingPlanEndTime_;
        if( locatingStartTime < startTime )
            locatingStartTime = startTime;

        //Get its duration, and hence compute number of animations to play
        PhysRelativeTime animationDuration = locatingPlanPtr_->finishTime();
        POST( animationDuration > 0.0 );

        uint nTimes = (uint)((moveEndTime - locatingStartTime + 0.95 * animationDuration) /
                             animationDuration );

        //Queue these animations
        if( nTimes != 0 )
        {
            plan( *locatingPlanPtr_, locatingStartTime, nTimes - 1);
            locatingPlanEndTime_ = locatingStartTime + nTimes * animationDuration;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysGeoLocator::createExplosionData( void )
{
    MachPhysMachineExplosionData& dataForEdit = explosionDataForEdit();

    dataForEdit.minToShootOff( 4 );
}

void perWrite( PerOstream& ostr, const MachPhysGeoLocator& machine )
{
    const MachPhysMachine& base = machine;
    ostr << base;

    ostr << machine.pImpl_;
}

void perRead( PerIstream& istr, MachPhysGeoLocator& machine )
{
    MachPhysMachine& base = machine;
    istr >> base;

    istr >> machine.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End GEOLOCAT.CPP *************************************************/
