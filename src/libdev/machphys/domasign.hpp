/*
 * D O M A S I G N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _MACHPHYS_DOMASIGN_HPP
#define _MACHPHYS_DOMASIGN_HPP

#include "base/base.hpp"
#include "world4d/domasign.hpp"

class MachPhysPlanetSurface;

// A W4dDomainAssignor which knows about the layout of Machines terrain
// tiles.  It can easily determine domains using a grid system.
class MachPhysDomainAssignor : public W4dDomainAssignor
{
public:
    MachPhysDomainAssignor(const MachPhysPlanetSurface*);
    virtual ~MachPhysDomainAssignor();

	virtual void assignDomains(W4dRoot*, W4dLight*);

private:
	const MachPhysPlanetSurface* const pPlanet_;
};

#endif

/* End DOMASIGN.HPP *************************************************/
