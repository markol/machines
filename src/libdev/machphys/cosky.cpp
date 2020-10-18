/*
 * C O S K Y. C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "envirnmt/planet.hpp"
#include "machphys/cosky.hpp"
#include "machphys/compmgr.hpp"
#include "machphys/plansurf.hpp"

MachPhysSkyComplexity::MachPhysSkyComplexity( const ItemId& id )
: MachPhysComplexityChoiceItem( id, 3, 2 )
{
}

MachPhysSkyComplexity::~MachPhysSkyComplexity()
{
}

// virtual
void MachPhysSkyComplexity::update()
{
		MachPhysPlanetSurface* pSurface=MachPhysComplexityManager::instance().planetSurface();
		if( pSurface and pSurface->hasEnvironment() )
		{
			EnvPlanetEnvironment::VisibilityLevel vLevel = EnvPlanetEnvironment::NO_SKY;
			switch( choice_ )
			{
				case 0:
					vLevel = EnvPlanetEnvironment::NO_SKY;
					break;
				case 1:
					vLevel = EnvPlanetEnvironment::SKY_WO_SATELLITES;
					break;
				case 2:
					vLevel = EnvPlanetEnvironment::FULL_SKY;
					break;
				default:
                    ASSERT_BAD_CASE_INFO( choice_ );
			}
			pSurface->environment().setVisibilityLevel( vLevel );
			changed_=false;
		}
}

/* End COSKY.CPP **************************************************/
