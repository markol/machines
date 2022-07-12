/*
 * G E O L O C A T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/internal/spylocai.hpp"

#include "stdlib/string.hpp"
#include "mathex/point3d.hpp"
#include "world4d/link.hpp"
#include "machphys/mexpdata.hpp"
#include "machphys/slocdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"

#include "system/pathname.hpp"
// I don't know why ctl/list.hpp must be included after the other #include
#include "ctl/list.hpp"


PER_DEFINE_PERSISTENT( MachPhysSpyLocator );

MachPhysSpyLocator::MachPhysSpyLocator(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race )
: MachPhysMachine( part( bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
   MachPhysData::instance().spyLocatorData( bodyLevel, brainLevel ) ),
    pImpl_( _NEW( MachPhysSpyLocatorImpl ) )
{
    CB_DEPIMPL( W4dCompositePlanPtr, locatingPlanPtr_ );
	cycleAnims( &locatingPlanPtr_ );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a locator from scratch

MachPhysSpyLocator::MachPhysSpyLocator( W4dEntity* pParent, size_t bodyLevel )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( bodyLevel ),
                   MachPhysData::instance().spyLocatorData( bodyLevel, 1 ) ),
    pImpl_( _NEW( MachPhysSpyLocatorImpl ) )
{
    createExplosionData();

    TEST_INVARIANT;
}

MachPhysSpyLocator::MachPhysSpyLocator( PerConstructor con )
: MachPhysMachine( con ),
    pImpl_( NULL )
{
}

MachPhysSpyLocator::~MachPhysSpyLocator()
{
    TEST_INVARIANT;
    _DELETE( pImpl_ );
}

SysPathName MachPhysSpyLocator::compositeFileName( size_t bodyLevel ) const
{
    SysPathName result;

    switch( bodyLevel )
    {
        case 3:
            result = "models/locator/spy/level3/los3.cdf";
            break;

        case 5:
            result = "models/locator/spy/level5/los5.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( bodyLevel );
            break;
    }

    return result;
}

// static
MachPhysSpyLocator& MachPhysSpyLocator::part( size_t hardwareLevel )
{
    return factory().part(
        hardwareLevel,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::SPY_LOCATOR, hardwareLevel ) );
}

// static
MachPhysSpyLocator::Factory& MachPhysSpyLocator::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::SPY_LOCATOR ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysSpyLocator::machineData() const
{
	return data();
}

const MachPhysSpyLocatorData& MachPhysSpyLocator::data() const
{
	return MachPhysData::instance().spyLocatorData( bodyLevel(), brainLevel() );
}

void MachPhysSpyLocator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSpyLocator& t )
{

    o << "MachPhysSpyLocator " << (void*)&t << " start" << std::endl;
    o << "MachPhysSpyLocator " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysSpyLocator::isLocating( bool doLocate )
{
    CB_DEPIMPL( bool, isLocating_ );

    if( doLocate != isLocating_ )
    {
        //Set mode flag
        isLocating_ = doLocate;

        //Modify walking speed - set to half normal when locating
        maxTranslationSpeed( maxTranslationSpeed() * (doLocate ? 0.5 : 2.0) );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachPhysSpyLocator::isLocating() const
{
    CB_DEPIMPL( bool, isLocating_ );

    return isLocating_;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachPhysSpyLocator::move( const MachPhysMachineMoveInfo& info )
{
    CB_DEPIMPL( bool, isLocating_ );
    CB_DEPIMPL( PhysAbsoluteTime, locatingPlanEndTime_ );
    CB_DEPIMPL( W4dCompositePlanPtr, locatingPlanPtr_ );

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

void MachPhysSpyLocator::createExplosionData()
{
    MachPhysMachineExplosionData& dataForEdit = explosionDataForEdit();

    MexPoint3d explosionCenter( dataForEdit.explosionCenter() );
    explosionCenter.z( explosionCenter.z()+0.8 );
	dataForEdit.explosionCenter( explosionCenter );

}

void perWrite( PerOstream& ostr, const MachPhysSpyLocator& machine )
{
    const MachPhysMachine& base = machine;
    ostr << base;

    ostr << machine.pImpl_;
}

void perRead( PerIstream& istr, MachPhysSpyLocator& machine )
{
    MachPhysMachine& base = machine;
    istr >> base;

    istr >> machine.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End GEOLOCAT.CPP *************************************************/
