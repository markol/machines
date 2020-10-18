/*
 * G R P M O V E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGroupMover

    A number of machines have been tasked with the same operation.
    This class takes those machines and their operations and sets
    up the appropriate group move data that they need.
*/

#ifndef _MACHLOG_GRPMOVER_HPP
#define _MACHLOG_GRPMOVER_HPP

#include "base/base.hpp"
#include "machlog/machop.hpp"

class MachLogGroupMoverUtility;

class MachLogGroupMover
// Canonical form revoked
{
public:
    MachLogGroupMover( const MachLogMachineOperations& );
    MachLogGroupMover( const MachLogMachineOperations&,
      const MachLogGroupMoverUtility& );
    ~MachLogGroupMover( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogGroupMover& t );

    MachLogGroupMover( const MachLogGroupMover& );
    MachLogGroupMover& operator =( const MachLogGroupMover& );

};


#endif

/* End GRPMOVER.HPP *************************************************/
