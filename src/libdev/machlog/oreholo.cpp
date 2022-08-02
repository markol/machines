/*
 * O R E H O L O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/domain.hpp"
#include "machphys/oreholo.hpp"
#include "machphys/consdata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/oreholo.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/planet.hpp"
#include "machlog/minesite.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

MachLogOreHolograph::MachLogOreHolograph
(
    MachLogRace* pRace, uint concentration, uint quantity, const MexPoint3d& location
)
:   MachActor( pRace, pPhysHolo_ = pNewOreHolograph( pRace, concentration, quantity, location), MachLog::ORE_HOLOGRAPH )
{
    TEST_INVARIANT;
}

MachLogOreHolograph::MachLogOreHolograph
(
    MachLogRace* pRace, uint concentration, uint quantity, const MexPoint3d& location, UtlId withId
)
:   MachActor( pRace, pPhysHolo_ = pNewOreHolograph( pRace, concentration, quantity, location), MachLog::ORE_HOLOGRAPH, withId )
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhysOreHolograph* MachLogOreHolograph::pNewOreHolograph
(
    MachLogRace* pRace, uint concentration, uint quantity, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the holograph
    return _NEW( MachPhysOreHolograph( pDomain, localTransform, pRace->race(),
                                       concentration, quantity ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogOreHolograph::~MachLogOreHolograph()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogOreHolograph::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogOreHolograph::beDestroyed()
{
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogOreHolograph::exists() const
{
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
//virtual
void MachLogOreHolograph::doStartExplodingAnimation()
{
}

//virtual
void MachLogOreHolograph::doEndExplodingAnimation()
{
}

//virtual
const MachPhysObjectData& MachLogOreHolograph::objectData() const
{
	return data();
}

const MachPhysConstructionData& MachLogOreHolograph::constructionData() const
{
	return data();
}

const MachPhysConstructionData& MachLogOreHolograph::data() const
{
	return _REINTERPRET_CAST( const MachPhysConstructionData&, MachPhysData::instance().oreHolographData() );
}

//virtual
void MachLogOreHolograph::doVisualiseSelectionState()
{
	//Do nothing - they can't be selected
}

const MachLogMineralSite& MachLogOreHolograph::mineralSite() const
{
	// Find the mineral site associated with this ore holograph. Bit of a hack, needs
	// sorting out properly at some point.
	const MachLogPlanet::Sites mineralSites = MachLogPlanet::instance().sites();
	MachLogMineralSite* pRetVal = NULL;

	for ( MachLogPlanet::Sites::const_iterator iter = mineralSites.begin(); iter != mineralSites.end(); ++iter )
	{
		MexPoint2d mineralPos( (*iter)->position() );
		MexPoint2d oreHoloPos( position() );

		if ( mineralPos == oreHoloPos )
		{
			pRetVal = (*iter);
		}
	}

	POST( pRetVal );

	return *pRetVal;
}

void MachLogOreHolograph::assignToDifferentRace(MachLogRace &newRace)
{
	// OreHolograph race change does not work:
	// Save files do not have Holographes but only MineralSites
	// During the load MineralSites create Holographes for the 'Race discoveredBy_'

	// See void perRead( PerIstream& istr, MachLogMineralSiteImpl& siteImpl ) in minesiti.cpp:64 for details
	return;
}

void MachLogOreHolograph::removeMe()
{
	isDead( true );
}

/* End OREHOLO.CPP **************************************************/
