/*
 * FINDSPAC . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//forward decls
#include "base/base.hpp"

class MachLogMachine;
class MexPoint2d;

//True iff succeeds in finding a suitable free space for pMachine near centrePoint,
//which is returned in pResult.

bool FindMachineSpace( const MachLogMachine* pMachine, const MexPoint2d& centrePoint,
                       MexPoint2d* pResult );
