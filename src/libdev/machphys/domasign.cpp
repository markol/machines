/*
 * D O M A S I G N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "machphys/domasign.hpp"
#include "ctl/list.hpp"
#include "world4d/light.hpp"
#include "world4d/domain.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "machphys/plansurf.hpp"
#include <algorithm>

MachPhysDomainAssignor::MachPhysDomainAssignor(const MachPhysPlanetSurface* p):
	pPlanet_(p)
{
	PRE(p);
}

// virtual
MachPhysDomainAssignor::~MachPhysDomainAssignor()
{
}

// virtual
void MachPhysDomainAssignor::assignDomains(W4dRoot* root, W4dLight* light)
{
	//ASSERT(light->findRoot() == root, "wrong tree; roots don't match");

	if (light->hasBoundingSphere())
	{
		pPlanet_->domainsAt( light );
	}
}

/* End DOMASIGN.CPP *************************************************/
