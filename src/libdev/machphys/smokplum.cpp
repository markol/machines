/*
 * S M O K P L U M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdio.h>
#include <iomanip>

#include "device/time.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/smokplum.hpp"
#include "machphys/random.hpp"
#include "machphys/smokpuff.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "sim/manager.hpp"

#include "phys/linemoti.hpp"

PER_DEFINE_PERSISTENT( MachPhysSmokePlume );

MachPhysSmokePlume::MachPhysSmokePlume(
    W4dEntity* pParent,
    const MexTransform3d& localBaseTransform,
    MATHEX_SCALAR maxHeight,
    MATHEX_SCALAR depthOffset,
    size_t nPuffs,
    MATHEX_SCALAR averagePuffSize,
    MachPhysPuffType puffType,
    const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration )
: W4dComposite( pParent, localBaseTransform, NOT_SOLID )
{
	// The smoke puffs are either emissive or black.  Hence, they should not
	// need lighting.  This could change if the model changes.
	doNotLight(true);

    puffs_.reserve( nPuffs );
    constructEntityPlan(
        pParent,
        localBaseTransform,
        maxHeight,
        depthOffset,
        nPuffs,
        averagePuffSize,
        puffType,
        duration );

    startSmokePlume( startTime );
}

MachPhysSmokePlume::MachPhysSmokePlume(
    W4dEntity* pParent,
    const MexTransform3d& localBaseTransform,
    MATHEX_SCALAR maxHeight,
    MATHEX_SCALAR depthOffset,
    size_t nPuffs,
    MATHEX_SCALAR averagePuffSize,
    MachPhysPuffType puffType,
    const PhysRelativeTime& duration )
: W4dComposite( pParent, localBaseTransform, NOT_SOLID )
{
	// The smoke puffs are either emissive or black.  Hence, they should not
	// need lighting.  This could change if the model changes.
	doNotLight(true);

    constructEntityPlan(
        pParent,
        localBaseTransform,
        maxHeight,
        depthOffset,
        nPuffs,
        averagePuffSize,
        puffType,
        duration );
}

void MachPhysSmokePlume::constructEntityPlan(
    W4dEntity*,
    const MexTransform3d&,
    MATHEX_SCALAR maxHeight,
    MATHEX_SCALAR depthOffset,
    size_t nPuffs,
    MATHEX_SCALAR averagePuffSize,
    MachPhysPuffType puffType,
    const PhysRelativeTime& duration )
{

    //  riseTime is the maximum length of time it will take a
    //  puff of smoke to reach its final height.

    PhysRelativeTime    riseTime = duration / 2;

    PhysLinearMotionPlan*   pMotionPlan =
        generateMotionPlan( riseTime, maxHeight );

    PhysMotionPlanPtr   planPtr( pMotionPlan );

    //Create the scale plans. TBD: Invent and use a segmented scale plan to replace 2 simple ones
    W4dScalePlan* pScalePlan1 = _NEW( W4dSimpleUniformScalePlan( 0.01, 1.0, riseTime / 2.0 ) );
    W4dScalePlan* pScalePlan2 = _NEW( W4dSimpleUniformScalePlan( 1.0, 0.01, riseTime / 2.0 ) );

    W4dScalePlanPtr scalePlan1Ptr( pScalePlan1 );
    W4dScalePlanPtr scalePlan2Ptr( pScalePlan2 );

    W4dVisibilityPlan*  pVisibilityPlan2 = _NEW( W4dVisibilityPlan( true ) );
    pVisibilityPlan2->add( false, riseTime );

    W4dVisibilityPlanPtr  visibilityPlanPtr2( pVisibilityPlan2 );

    puffs_.reserve( nPuffs );

    for( size_t i = 0; i < nPuffs; ++i )
    {
        MATHEX_SCALAR puffSize = MachPhysRandom::randomDouble( averagePuffSize * 0.9, averagePuffSize * 1.3 );

        //  By rotating the puff's starting transform we can use the same
        //  motion plan to give apparently different motions

        MexTransform3d  puffTransform;
        planPtr->transform( PhysRelativeTime( 0.0 ), &puffTransform );

        MachPhysSmokePuff* pPuff = _NEW( MachPhysSmokePuff(
            this, puffTransform, puffType,
            puffSize, depthOffset ) );

        W4dEntityPlan*  pEntityPlan = _NEW( W4dEntityPlan() );

        pEntityPlan->absoluteMotion( planPtr, 0.0 );
        pEntityPlan->scalePlan( scalePlan1Ptr, 0.0 );
        pEntityPlan->scalePlan( scalePlan2Ptr, riseTime / 2.0 );
        pEntityPlan->visibilityPlan( visibilityPlanPtr2, 0.0 );

        //  Make the puffs invisible until the visibility plan kicks in
        pPuff->visible( false );

        PhysRelativeTime    puffStartTime = MachPhysRandom::randomDouble( 0, duration - riseTime );

        puffs_.push_back( PuffData( pPuff, pEntityPlan, puffStartTime ) );
    }

    TEST_INVARIANT;
}

MachPhysSmokePlume::~MachPhysSmokePlume()
{
    TEST_INVARIANT;

    for( PuffDatas::iterator i = puffs_.begin(); i != puffs_.end(); ++i )
    {
        _DELETE( (*i).pPlan() );
    }
}

void MachPhysSmokePlume::startSmokePlume( const PhysAbsoluteTime& startTime )
{

    for( PuffDatas::iterator i = puffs_.begin(); i != puffs_.end(); ++i )
    {
        (*i).pSmokePuff()->entityPlanForEdit().plan( *(*i).pPlan(), startTime + (*i).startTimeOffset() );
    }
}

PhysLinearMotionPlan* MachPhysSmokePlume::generateMotionPlan(
    const PhysRelativeTime& riseTime,
    MATHEX_SCALAR maxHeight )
{
    //  Generate a random motion plan to use for the rising smoke
    MexEulerAngles orientation( MachPhysRandom::randomDouble( 0.0, 2.0 * Mathex::PI ),
                                0.0,
                                0.0 );

    const   size_t  nTimes = 4;

    ctl_vector< MexTransform3d >    transforms;
    ctl_vector< PhysRelativeTime >          times;

    transforms.reserve( nTimes );
    times.reserve( nTimes );

    for( size_t i = 0; i < nTimes; ++i )
    {
        times.push_back( ( i + 1 ) * riseTime / nTimes );

        MATHEX_SCALAR height = ( i + 1 ) * maxHeight / nTimes;
        MATHEX_SCALAR x = MachPhysRandom::randomDouble( -height / 10.0, height / 10.0 );
        MATHEX_SCALAR y = MachPhysRandom::randomDouble( -height / 10.0, height / 10.0 );

        transforms.push_back( MexTransform3d( orientation, MexPoint3d( x, y, height ) ) );
    }

    PhysLinearMotionPlan* pMotionPlan = _NEW( PhysLinearMotionPlan(
        MexTransform3d(), transforms.front(), times.front() ) );

    ctl_vector< MexTransform3d >::const_iterator    transformIterator = transforms.begin();
    ctl_vector< PhysRelativeTime >::const_iterator          timeIterator = times.begin();

    ++transformIterator;
    ++timeIterator;

    for( ; transformIterator != transforms.end(); ++transformIterator, ++timeIterator )
    {
        pMotionPlan->add( *transformIterator, *timeIterator );
    }

    return pMotionPlan;
}

void MachPhysSmokePlume::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSmokePlume& t )
{

    o << "MachPhysSmokePlume " << (void*)&t << " start" << std::endl;
    o << "MachPhysSmokePlume " << (void*)&t << " end" << std::endl;

    return o;
}

bool MachPhysSmokePlume::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysSmokePlume::MachPhysSmokePlume( PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysSmokePlume& plume )
{
    const W4dComposite& base = plume;

    ostr << base;

    ostr << plume.puffs_;
}

void perRead( PerIstream& istr, MachPhysSmokePlume& plume )
{
    W4dComposite& base = plume;

    istr >> base;
    istr >> plume.puffs_;
}

MachPhysSmokePlume::PuffData::PuffData( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysSmokePlume::PuffData& data )
{
    ostr << data.pSmokePuff_;
    ostr << data.pPlan_;
    ostr << data.startTimeOffset_;
}

void perRead( PerIstream& istr, MachPhysSmokePlume::PuffData& data )
{
    istr >> data.pSmokePuff_;
    istr >> data.pPlan_;
    istr >> data.startTimeOffset_;
}

/* End SMOKPLUM.CPP *************************************************/
