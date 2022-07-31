/*
 * A C T O P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachiuneOperation

	Utility class to store a pointer to a machine and its associated operation.
	Used to make group moves work better
*/

#ifndef _MACHLOG_MACHOP_HPP
#define _MACHLOG_MACHOP_HPP

#include "base/base.hpp"
#include "ctl/utility.hpp"
#include "ctl/vector.hpp"

class MachLogMachine;
class MachLogOperation;

CTL_PAIR( MachLogMachineOperation, MachLogMachine*, pMachine, MachLogOperation*, pOperation );

typedef ctl_vector< MachLogMachineOperation > MachLogMachineOperations;

#endif
