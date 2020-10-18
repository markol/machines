/*
 * G R P M V U T L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/grpmvutl.hpp"
#include "machlog/machine.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"

MachLogGroupMoverUtility::MachLogGroupMoverUtility( const Machines& actors, MATHEX_SCALAR distance )
: distance_( distance )
{
    PRE( actors.size() > 0 );

    clumps_.reserve( 32 );

    createClumps( actors );

    TEST_INVARIANT;
}

MachLogGroupMoverUtility::~MachLogGroupMoverUtility()
{
    TEST_INVARIANT;

}

uint MachLogGroupMoverUtility::nClumps() const
{
    uint result = clumps_.size();

    POST( result > 0 );

    return result;
}

uint MachLogGroupMoverUtility::largestClumpIndex() const
{
    return largestClumpIndex_;
}

const MachLogGroupMoverUtility::Clump& MachLogGroupMoverUtility::clump( uint index ) const
{
    PRE( index < nClumps() );

    return clumps_[ index ];
}

void MachLogGroupMoverUtility::createClumps(
    const Machines& actors )
{
    PRE( actors.size() != 0 );

    Machines  unprocessed( actors );

    size_t largestClumpSize = 0;
    MATHEX_SCALAR largestClumpDistance = -1;

    while( unprocessed.size() != 0 )
    {
        Machines  actorsInClump;
		actorsInClump.reserve( unprocessed.size() );

        actorsInClump.push_back( unprocessed.back() );
        unprocessed.pop_back();

        while( addToClump( &actorsInClump, &unprocessed ) != 0 )
        {
            //  Do nothing
        }

        if( actorsInClump.size() > largestClumpSize )
        {
            largestClumpSize = actorsInClump.size();
            largestClumpIndex_ = clumps_.size();
        }

        MexPoint2d centroid;
        MATHEX_SCALAR radius;

        calculateCentroidAndRadius( actorsInClump, &centroid, &radius );

        clumps_.push_back( Clump( actorsInClump, centroid, radius ) );
    }

    ASSERT( largestClumpSize > 0, "" );
}

//  Do a single pass through the actors in pMachines and, if they are close
//  enough to any of the actors in pClump, remove them from pMachines and
//  add them to pClump. Return the number of actors added to the clump.
size_t MachLogGroupMoverUtility::addToClump( Machines* pClump, Machines* pMachines )
{
    size_t  nAdded = 0;
    size_t  index = 0;

    const MATHEX_SCALAR sqrDistance = distance_ * distance_;

    while( index < pMachines->size() )
    {
        //  See if this machine is close enough to any machine
        //  in the clump to be included

        MachLogMachine* pMachine = (*pMachines)[ index ];
        const MexPoint3d& position = pMachine->position();

        bool    added = false;

        for( size_t j = 0; j < pClump->size() and not added; ++j )
        {
            const MexPoint3d& processedPosition = (*pClump)[ j ]->position();
            if( position.sqrEuclidianDistance( processedPosition ) < sqrDistance )
            {
                added = true;
                ++nAdded;
                pClump->push_back( pMachine );

                pMachines->erase( pMachines->begin() + index );
            }
        }

        if( not added )
            ++index;
    }

    return nAdded;
}

void MachLogGroupMoverUtility::calculateCentroidAndRadius(
  const Machines& actors,
  MexPoint2d* pCentroid,
  MATHEX_SCALAR* pRadius ) const
{
    MexPoint2d  accumulator;

    for( Machines::const_iterator i = actors.begin(); i != actors.end(); ++i )
    {
        accumulator += MexVec2( MexPoint2d( (*i)->position() ) );
    }

    pCentroid->x( accumulator.x() / actors.size() );
    pCentroid->y( accumulator.y() / actors.size() );

    *pRadius = 0.0;

    for( Machines::const_iterator i = actors.begin(); i != actors.end(); ++i )
    {
        //  TBD: Should really take account of clearance here as well
        const MATHEX_SCALAR newRadius = pCentroid->euclidianDistance( (*i)->position() );

        *pRadius = std::max( *pRadius, newRadius );
    }

}

void MachLogGroupMoverUtility::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGroupMoverUtility& t )
{

    o << "MachLogGroupMoverUtility " << (void*)&t << " start" << std::endl;
    o << "MachLogGroupMoverUtility " << (void*)&t << " end" << std::endl;

    return o;
}

/* End GRPMVUTL.CPP *************************************************/
