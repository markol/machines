/*
 * FINDSPAC . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "phys/cspace2.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machine.hpp"
#include "machlog/planet.hpp"

bool FindMachineSpace( const MachLogMachine* pMachine, const MexPoint2d& centrePoint,
                       MexPoint2d* pResult )
{
	//get the config space
	PhysConfigSpace2d& configSpace = MachLogPlanet::instance().configSpace();

	//Get the machine clearance, and hence compute a suitable radius for
	//the search
	MATHEX_SCALAR clearance = pMachine->highClearence();
	MATHEX_SCALAR radius = clearance * 2;

	//Get the machine's current position in 2d space
	MexPoint3d loc3d = pMachine->position();
	MexPoint2d startPoint( loc3d.x(), loc3d.y() );
	return configSpace.findSpace( startPoint, centrePoint, clearance, radius, pMachine->obstacleFlags(), pResult );
}
