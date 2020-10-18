/*
 * V A P T R A I L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/vaptrail.hpp"

#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "world4d/manager.hpp"
#include "world4d/garbcoll.hpp"
#include "sim/manager.hpp"

#include "machphys/vappuff.hpp"
#include "machphys/fireball.hpp"

PER_DEFINE_PERSISTENT( MachPhysVapourTrail );

MachPhysVapourTrail::MachPhysVapourTrail(
    W4dEntity* pParent,
    size_t nPuffs,
    MATHEX_SCALAR averagePuffSize, size_t missile_level )
: currentPuff_( 0 ),
  nPuffs_( nPuffs )
{
    vapourPuffs_.reserve( nPuffs );

    for( size_t i = 0; i < nPuffs; ++i )
    {
        vapourPuffs_.push_back( _NEW( MachPhysVapourPuff(
            pParent,
            W4dTransform3d(),
            averagePuffSize, missile_level ) ) );
    }

    updateRate_ = vapourPuffs_[currentPuff_]->nFrames( missile_level ) / nPuffs;
    updateAtFrame_ = W4dManager::instance().frameNumber();

    TEST_INVARIANT;
}

MachPhysVapourTrail::MachPhysVapourTrail( PerConstructor )
{
}

MachPhysVapourTrail::~MachPhysVapourTrail()
{
    TEST_INVARIANT;

    //The vapour puffs will be deleted by the garbage collector,
    //provided finish() is called, or parent is deleted.
}

void    MachPhysVapourTrail::update( const MexPoint3d& currentPosition, const MexVec3& offset )
{
    size_t currentFrame = W4dManager::instance().frameNumber();

    if( currentFrame >= updateAtFrame_ )
    {
        MexPoint3d      position1( currentPosition );
        position1 += offset;

        MexTransform3d  transform( position1 );

        vapourPuffs_[ currentPuff_ ]->localTransform( transform );
        vapourPuffs_[ currentPuff_ ]->startPuff();
        vapourPuffs_[ currentPuff_ ]->rotate( MexDegrees( 90 ) * ( rand() % 4 ) );
        currentPuff_ = ( currentPuff_ + 1 ) % nPuffs_;

        updateAtFrame_ = currentFrame + updateRate_;
    }
}

void MachPhysVapourTrail::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVapourTrail& t )
{

    o << "MachPhysVapourTrail " << (void*)&t << " start" << std::endl;
    o << "MachPhysVapourTrail " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysVapourTrail::finish( const PhysAbsoluteTime& endTime )
{
    W4dGarbageCollector& garbageCollector = W4dGarbageCollector::instance();

    for( size_t i = 0; i < vapourPuffs_.size(); ++i )
        garbageCollector.add( vapourPuffs_[ i ], endTime );
}

void perWrite( PerOstream& ostr, const MachPhysVapourTrail& trail )
{
    ostr << trail.vapourPuffs_;
    ostr << trail.updateRate_;
    ostr << trail.currentPuff_;
    ostr << trail.updateAtFrame_;
    ostr << trail.nPuffs_;
    ostr << trail.lastPosition_;
}

void perRead( PerIstream& istr, MachPhysVapourTrail& trail )
{
    istr >> trail.vapourPuffs_;
    istr >> trail.updateRate_;
    istr >> trail.currentPuff_;
    istr >> trail.updateAtFrame_;
    istr >> trail.nPuffs_;
    istr >> trail.lastPosition_;
}

/* End VAPTRAIL.CPP *************************************************/
