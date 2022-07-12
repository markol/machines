/*
 * C E X P D A T A . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/cexpdata.hpp"

MachPhysConstructionExplosionData::MachPhysConstructionExplosionData( const PhysRelativeTime& duration )
: duration_( duration )
{

    TEST_INVARIANT;
}

MachPhysConstructionExplosionData::~MachPhysConstructionExplosionData()
{
    TEST_INVARIANT;

}

void    MachPhysConstructionExplosionData::addPoint( const ExplosionPoint& point )
{
    explosionPoints_.push_back( point );
}

const MachPhysConstructionExplosionData::ExplosionPoints& MachPhysConstructionExplosionData::explosionPoints() const
{
    return explosionPoints_;
}

const PhysRelativeTime&    MachPhysConstructionExplosionData::duration() const
{
    return duration_;
}

void MachPhysConstructionExplosionData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


/* End CEXPDATA.CPP *************************************************/
