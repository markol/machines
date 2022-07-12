/*
 * R E S C A R R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/rescarr.hpp"

#include "stdlib/string.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox3d.hpp"

#include "system/pathname.hpp"
#include "world4d/root.hpp"
#include "world4d/link.hpp"
#include "world4d/composit.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"

#include "machphys/mexpdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/levels.hpp"
#include "machphys/loading.hpp"
#include "machphys/scavenge.hpp"
#include "machphys/snddata.hpp"

// I don't know why ctl/list.hpp must be included after the other #include
// ti avoid instantiation errors
#include "ctl/list.hpp"

PER_DEFINE_PERSISTENT( MachPhysResourceCarrier );

MachPhysResourceCarrier::MachPhysResourceCarrier(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race )
: MachPhysMachine( part( bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
   MachPhysData::instance().resourceCarrierData( bodyLevel, brainLevel ) ),
   bodyLevel_( bodyLevel )
{
    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a transporter from scratch

MachPhysResourceCarrier::MachPhysResourceCarrier( W4dEntity* pParent, size_t bodyLevel )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( bodyLevel ),
                   MachPhysData::instance().resourceCarrierData( bodyLevel, 1 ) )
{
    createExplosionData();

    TEST_INVARIANT;
}

MachPhysResourceCarrier::MachPhysResourceCarrier( PerConstructor con )
: MachPhysMachine( con )
{
}

SysPathName MachPhysResourceCarrier::compositeFileName( size_t bodyLevel ) const
{
    SysPathName result;

    switch( bodyLevel )
    {
        case 1:
            result = "models/transpor/resource/level1/trr1.cdf";
            break;

        case 2:
            result = "models/transpor/resource/level2/trr2.cdf";
            break;

        case 3:
            result = "models/transpor/resource/level3/trr3.cdf";
            break;

        case 4:
            result = "models/transpor/resource/level4/trr4.cdf";
            break;

        case 5:
            result = "models/transpor/resource/level5/trr5.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( bodyLevel );
            break;
    }

    return result;
}

// static
MachPhysResourceCarrier& MachPhysResourceCarrier::part( size_t bodyLevel )
{
    return factory().part(
        bodyLevel,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::RESOURCE_CARRIER, bodyLevel ) );
}

// static
MachPhysResourceCarrier::Factory& MachPhysResourceCarrier::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::RESOURCE_CARRIER ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysResourceCarrier::machineData() const
{
	return data();
}

const MachPhysResourceCarrierData& MachPhysResourceCarrier::data() const
{
	return MachPhysData::instance().resourceCarrierData( bodyLevel(), brainLevel() );
}

MachPhysResourceCarrier::~MachPhysResourceCarrier()
{
    TEST_INVARIANT;

}

void MachPhysResourceCarrier::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysResourceCarrier& t )
{

    o << "MachPhysResourceCarrier " << (void*)&t << " start" << std::endl;
    o << "MachPhysResourceCarrier " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysResourceCarrier::createExplosionData()
{
    MachPhysMachineExplosionData& dataForEdit = explosionDataForEdit();

//      for( MachPhysMachineExplosionData::LinkDatas::const_iterator i = dataForEdit.links().begin(); i != dataForEdit.links().end(); ++i )
//      {
//          MachPhysMachineExplosionData::LinkData  linkData = *i;
//
//  		W4dLink* pLink = links()[ linkData.linkId() ];
//          if( ( pLink->name() == "head" )
//  		    or
//  			( pLink->name() == "hip" )
//  		    or
//  			( pLink->name() == "fplat" ) )
//          {
//              linkData.shootOffProbability( 1.0 );
//              linkData.minExplosionTime( 0.3 );
//              linkData.maxExplosionTime( 0.6 );
//          }
//          else
//          {
//              linkData.shootOffProbability( 0.5 );
//              linkData.minExplosionTime( 0.3 );
//              linkData.maxExplosionTime( 0.6 );
//          }
//
//      }
//      dataForEdit.minToShootOff( 3 );
}

void perWrite( PerOstream& ostr, const MachPhysResourceCarrier& machine )
{
    const MachPhysMachine& base = machine;
    ostr << base;
	ostr << machine.bodyLevel_;
}

void perRead( PerIstream& istr, MachPhysResourceCarrier& machine )
{
    MachPhysMachine& base = machine;
    istr >> base;
	istr >> machine.bodyLevel_;
}

const PhysRelativeTime MachPhysResourceCarrier::doLoading( const PhysAbsoluteTime& startTime )
{
	MachPhysResourceLoading* pLoading = _NEW(MachPhysResourceLoading( this, MexTransform3d() ) );
	MATHEX_SCALAR size = 1.05 * std::max( compositeBoundingVolume().xLength(), compositeBoundingVolume().yLength() );
	const PhysRelativeTime loadingTime = pLoading->startLoading( startTime, size );

//	W4dSoundManager::instance().play(pLoading, SID_RECYCLE, startTime, 1); // this sound is not present in final game in this context

	W4dGarbageCollector::instance().add( pLoading, startTime + loadingTime );

 	return loadingTime;
}

bool MachPhysResourceCarrier::isScavenger() const
{
	return bodyLevel_ == 3;
}

//perform the scavenging animations
const PhysRelativeTime MachPhysResourceCarrier::doScavenge( const PhysAbsoluteTime& startTime )
{
	PRE( isScavenger() );

	const PhysRelativeTime duration = 1.0;

	MachPhysScavenger* pScavenger = _NEW( MachPhysScavenger( this, MexTransform3d() ) );
	pScavenger->startScavenge( startTime, duration );

	W4dGarbageCollector::instance().add( pScavenger, startTime + duration );

	return duration;
}

/* End RESCARR.CPP *************************************************/
