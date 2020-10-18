/*
 * G R P M O V E I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/grpmovei.hpp"

#include "ctl/list.hpp"
#include "mathex/vec3.hpp"
#include "mathex/circle2d.hpp"
#include "phys/motchunk.hpp"
#include "machlog/actor.hpp"
#include "machlog/move.hpp"
#include "machlog/machine.hpp"
#include "machlog/planet.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/machop.hpp"
#include "machlog/grpmover.hpp"
#include "machlog/internal/grpmvutl.hpp"

MachLogGroupSimpleMoveImplementation::MachLogGroupSimpleMoveImplementation(
    const Actors& actors,
    const Points& points,
    size_t commandId,
    string* pReason,
    PhysPathFindingPriority pathFindingPriority )
: points_( points ),
  configSpace_( MachLogPlanet::instance().configSpace() ),
  pathFindingPriority_( pathFindingPriority )
{
    PRE( points.size() != 0 );

	Machines machines;
    machines.reserve( actors.size() + 1 );

    points_.reserve( 64 );
    destData_.reserve( 64 );

	for( Actors::const_iterator i = actors.begin(); i!= actors.end(); ++i )
		if( (*i)->objectIsMachine() )
			machines.push_back( &(*i)->asMachine() );

    complexMove( machines, points, commandId, pReason );

    TEST_INVARIANT;
}

MachLogGroupSimpleMoveImplementation::~MachLogGroupSimpleMoveImplementation()
{
    TEST_INVARIANT;

}

ostream& operator <<( ostream& ostr, const MachLogGroupSimpleMoveImplementation::Actors& actors )
{
    for( MachLogGroupSimpleMoveImplementation::Actors::const_iterator i = actors.begin(); i != actors.end(); ++i )
    {
        ostr << "Actor " << (void*)(*i) << " position " << (*i)->position() << std::endl;
    }

    return ostr;
}

void MachLogGroupSimpleMoveImplementation::complexMove(
    const Machines& machines,
    const Points& points,
    size_t commandId,
    string* /* pReason */ )
{
    PRE( points.size() != 0 );
    PRE( machines.size() != 0 );

    const MATHEX_SCALAR clumpDistance = 20.0;

    MachLogGroupMoverUtility moverUtility( machines, clumpDistance );

    calculateGridSpacing( machines );

    const Clump& largestClump = moverUtility.clump( moverUtility.largestClumpIndex() );
    Machines unlocatedMachines;

    positionControllingClump( largestClump, &unlocatedMachines );

    positionMachinesOnGrid( unlocatedMachines );

    for( uint i = 0; i < moverUtility.nClumps(); ++i )
    {
        if( i != moverUtility.largestClumpIndex() )
        {
            const Machines& machines = moverUtility.clump( i ).machines();
            positionMachinesOnGrid( machines );
        }
    }

    //  Give the move commands to the machines

    //  Construct a path in the correct format
    MachLogMoveToOperation::Path path;
    size_t nPoints = points.size();
    //path.reserve( nPoints );

    for( size_t i = 0; i != nPoints; ++i )
        path.push_back( points[i] );

    //  Sort the list so that the machines that are nearest are first.
    //  This will make sure that these machines' destination calls are
    //  made first - they should be the first machines to move off. In
    //  addition, we give these machines a higher priority for their
    //  pathfinding.
    //  This will fail dreadfully in some cases but should work for
    //  a lot of simple cases where the first move is in the direction
    //  of the ultimate destination.
    sort( destData_.begin(), destData_.end(), compareDistanceToGroupDestination );

    PhysPathFindingPriority priorityModifier = destData_.size();

    MachLogMachineOperations machineOperations;

    for( DestData::const_iterator i = destData_.begin(); i != destData_.end(); ++i )
    {
        MachLogMachine* pMachine = (*i).pMachine();

        path.pop_back();
        path.push_back( (*i).destination() );

        //Construct a move operation
        MachLogMoveToOperation* pOp = _NEW(
          MachLogMoveToOperation( pMachine, path, commandId,
          pathFindingPriority_ + priorityModifier ) );

        --priorityModifier;

        //Give it to the actor

        // The operation is now given to the actor by the group move code
        //        pMachine->newOperation( pOp );

        machineOperations.push_back( MachLogMachineOperation( pMachine, pOp ) );
    }

    //  Handle the extra information which helps the group
    //  move to work effectively
    MachLogGroupMover   groupMover( machineOperations, moverUtility );

	MachLogMachine& firstActor = **( machines.begin() );
}

void MachLogGroupSimpleMoveImplementation::calculateGridSpacing( const Machines& machines )
{
	PRE( machines.size() );
    MATHEX_SCALAR   clearance = 0;
    for( Machines::const_iterator i = machines.begin(); i != machines.end(); ++i )
    {
        if( (*i)->objectIsMachine() )
        {
            const MachLogMachine& machine = (*i)->asMachine();

            clearance += machine.lowClearence();
        }
    }

    gridSpacing_ = 2.1 * clearance / machines.size();
}

//  Attempt to position the controlling clump at the destination. This function tries
//  to maintain the relative positions of the machines. Any machines that cannot be
//  positioned (due to intersections with terrain or other machines) will be added to
//  the unlocated machines list

void MachLogGroupSimpleMoveImplementation::positionControllingClump(
    const Clump& controllingClump,
    Machines* pUnlocatedMachines )
{
    const Machines& machines = controllingClump.machines();

    const MexPoint3d&  clumpOrigin = controllingClump.centroid();

    for( Machines::const_iterator i = machines.begin(); i != machines.end(); ++i )
    {
        MexVec3 offset( (*i)->position() );
        offset -= clumpOrigin;

        MexPoint3d  desiredFinalDestination( points_.back() );
        desiredFinalDestination += offset;

        MachLogMachine* pMachine = *i;

        if( canPlaceAtDestination( pMachine, desiredFinalDestination ) )
        {
        }
        else
        {
            pUnlocatedMachines->insert( pUnlocatedMachines->begin(), pMachine );
        }
    }
}

bool MachLogGroupSimpleMoveImplementation::canPlaceAtDestination(
  MachLogMachine* pMachine,
  const MexPoint3d& desiredDestination )
{
    bool    canPlace = true;

    const MachLogMachine& machine = *pMachine;

    //  Check against terrain / other machines

    if( not machine.motionSeq().positionContainedInSpace( desiredDestination, configSpace_ ) )
    {
        canPlace = false;
    }

    //  Check against machines in this group that have already been given destinations

    const MATHEX_SCALAR clearance = machine.lowClearence();

    if( canPlace )
    {
        for( DestData::const_iterator i = destData_.begin(); i != destData_.end() and canPlace; ++i )
        {
            MATHEX_SCALAR sqrDistance = desiredDestination.sqrEuclidianDistance( (*i).destination() );
            MATHEX_SCALAR totalClearance = clearance + (*i).clearance();
            MATHEX_SCALAR sqrTotalClearance = totalClearance * totalClearance;

            if( sqrDistance < sqrTotalClearance )
            {
                canPlace = false;
            }
        }
    }

    if( canPlace )
    {
        //  Add to the machines in this group that have been given destinations

        const MATHEX_SCALAR sqrDistanceToGroupDestination =
          pMachine->position().sqrEuclidianDistance( points_[ 0 ] );

        destData_.push_back( MachineData(
          pMachine, desiredDestination,
          clearance, sqrDistanceToGroupDestination ) );
    }

    return canPlace;
}

// MexPoint3d MachLogGroupSimpleMoveImplementation::centroid( const Actors& actors ) const
// {
//     MexPoint3d  result( 0.0, 0.0, 0.0 );
//
//     for( Actors::const_iterator i = actors.begin(); i != actors.end(); ++i )
//     {
//         result.x( result.x() + (*i)->position().x() );
//         result.y( result.y() + (*i)->position().y() );
//         result.z( result.z() + (*i)->position().z() );
//     }
//
//     result.x( result.x() / actors.size() );
//     result.y( result.y() / actors.size() );
//     result.z( result.z() / actors.size() );
//
//     return result;
// }

void MachLogGroupSimpleMoveImplementation::positionMachinesOnGrid(
    const Machines& unlocatedMachines )
{
    for( Machines::const_iterator i = unlocatedMachines.begin(); i != unlocatedMachines.end(); ++i )
    {
        bool    placedMachine = false;

        while( not placedMachine )
        {
            MexPoint3d  desiredDestination( nextGridDestination() );

            if( canPlaceAtDestination( *i, desiredDestination ) )
                placedMachine = true;
        }
    }
}

bool    MachLogGroupSimpleMoveImplementation::moveOK() const
{
    return true;
}

MexPoint3d  MachLogGroupSimpleMoveImplementation::nextGridDestination()
{
    MexPoint3d  result( points_.back() );
    result += MexVec3( spiral_.x() * gridSpacing(), spiral_.y() * gridSpacing(), 0.0 );

    ++spiral_;

    return result;
}

MATHEX_SCALAR MachLogGroupSimpleMoveImplementation::gridSpacing() const
{
    return gridSpacing_;
}

void MachLogGroupSimpleMoveImplementation::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGroupSimpleMoveImplementation& t )
{

    o << "MachLogGroupSimpleMoveImplementation " << (void*)&t << " start" << std::endl;
    o << "MachLogGroupSimpleMoveImplementation " << (void*)&t << " end" << std::endl;

    return o;
}

// static
bool MachLogGroupSimpleMoveImplementation::compareDistanceToGroupDestination( const MachineData& a, const MachineData& b )
{
    //  Need to sort so that the closest machines are last in the list
    return a.sqrDistanceToGroupDestination() < b.sqrDistanceToGroupDestination();
}

/* End GRPMOVEI.CPP *************************************************/
