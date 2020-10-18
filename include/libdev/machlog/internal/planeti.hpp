/*
 * P L A N E T I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPlanetImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_PLANETI_HPP
#define _MACHLOG_PLANETI_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"
#include "machlog/minesite.hpp"
#include "machlog/planet.hpp"
#include "ctl/pvector.hpp"
#include "ctl/map.hpp"

class MachLogPlanetImpl
// Canonical form revoked
{
public:

	typedef ctl_map< int , MachLogMineralSite*, std::less< int >	>	SiteMap;

    MachLogPlanetImpl( void );
    ~MachLogPlanetImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogPlanetImpl& t );
	friend class MachLogPlanet;

    MachLogPlanetImpl( const MachLogPlanetImpl& );
    MachLogPlanetImpl& operator =( const MachLogPlanetImpl& );

    MachPhysPlanetSurface* 		pSurface_; //The planet surface
    W4dEntity* 					pWorld_; //The world entity
	W4dRoot*	   				pHiddenRoot_;
	W4dDomain*	   				pHiddenRootDomain_;
    PhysConfigSpace2d* 			pConfigSpace_; //The main config space for the entire planet
    PhysConfigSpace2d* 			pHiddenConfigSpace_; //The hidden config space for hidding ML entities in.

    MachLogPressurePads* 		pPressurePads_;

	MachLogPlanet::Sites		oreSites_;
	MachLogPlanet::DebrisSites	debrisSites_;

	int							idsGenerated_;

	SiteMap						siteMap_;

    SysPathName                 spaceFilePath_;
    SysPathName                 surfaceFilePath_;

};


#endif

/* End PLANETI.HPP **************************************************/
