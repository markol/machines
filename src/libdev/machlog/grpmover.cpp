/*
 * G R P M O V E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/grpmover.hpp"
#include "machlog/machine.hpp"
#include "machlog/operatio.hpp"
#include "machlog/internal/grpmvutl.hpp"
#include "machlog/grpmvinf.hpp"
#include "mathex/vec2.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"

//  Helper class just to keep MachLogGroupMoverUtility out of
//  the public interface

class MachLogGroupMoverInternal
{
public:

    static void processClump(
        const MachLogGroupMoverUtility::Clump& clump,
        const MachLogMachineOperations& machineOperations );

    static MachLogOperation* pOperation(
        const MachLogMachine* pMachine,
        const MachLogMachineOperations& machineOperations );

private:
    MachLogGroupMoverInternal();
    ~MachLogGroupMoverInternal();
    MachLogGroupMoverInternal( const MachLogGroupMoverInternal& );
    MachLogGroupMoverInternal& operator =( const MachLogGroupMoverInternal& );
};

MachLogGroupMover::MachLogGroupMover( const MachLogMachineOperations& machineOperations )
{
    MachLogGroupMoverUtility::Machines machines;
    machines.reserve( machineOperations.size() );

    for( MachLogMachineOperations::const_iterator i = machineOperations.begin();
      i != machineOperations.end(); ++i )
    {
        machines.push_back( (*i).pMachine() );
    }

    const MATHEX_SCALAR groupDistance = 20.0;

    MachLogGroupMoverUtility utility( machines, groupDistance );

    for( size_t i = 0; i < utility.nClumps(); ++i )
    {
        MachLogGroupMoverInternal::processClump( utility.clump( i ), machineOperations );
    }

    TEST_INVARIANT;
}

MachLogGroupMover::MachLogGroupMover(
  const MachLogMachineOperations& machineOperations,
  const MachLogGroupMoverUtility& moverUtility )
{
    for( size_t i = 0; i < moverUtility.nClumps(); ++i )
    {
        MachLogGroupMoverInternal::processClump( moverUtility.clump( i ), machineOperations );
    }

    TEST_INVARIANT;
}

MachLogGroupMover::~MachLogGroupMover()
{
    TEST_INVARIANT;

}

void MachLogGroupMoverInternal::processClump(
    const MachLogGroupMoverUtility::Clump& clump,
    const MachLogMachineOperations& machineOperations )
{
    const MachLogGroupMoverUtility::Machines& machines = clump.machines();

    for( MachLogGroupMoverUtility::Machines::const_iterator i = machines.begin(); i != machines.end(); ++i )
    {
        MachLogMachine* pMachine = (*i);
        MachLogOperation* pOperation = MachLogGroupMoverInternal::pOperation( pMachine, machineOperations );

        MexVec2 offset( clump.centroid(), pMachine->position() );

        MachLogGroupMoveInfo info( offset, clump.radius() );
        pOperation->groupMoveInfo( info );

        // We must make sure that we don't give the new operation to the
        // machine until after we have set up the group move information.

        pMachine->newOperation( pOperation );
    }
}

MachLogOperation* MachLogGroupMoverInternal::pOperation(
    const MachLogMachine* pMachine,
    const MachLogMachineOperations& machineOperations )
{
    MachLogOperation* result = NULL;

    for( MachLogMachineOperations::const_iterator i = machineOperations.begin();
      i != machineOperations.end() and result == NULL; ++i )
    {
        if( pMachine == (*i).pMachine() )
            result = (*i).pOperation();
    }

    POST( result != NULL );

    return result;
}

void MachLogGroupMover::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGroupMover& t )
{

    o << "MachLogGroupMover " << (void*)&t << " start" << std::endl;
    o << "MachLogGroupMover " << (void*)&t << " end" << std::endl;

    return o;
}

/* End GRPMOVER.CPP *************************************************/
