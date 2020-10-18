/*
 * P L A N D O M S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/radians.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/grid2d.hpp"
#include "world4d/domain.hpp"
#include "machphys/plansurf.hpp"

#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

W4dDomain* MachLogPlanetDomains::pDomainPosition
(
    const MexPoint3d& globalLocation, const MexRadians& zAngle,
    MexTransform3d* pLocalTransform
)
{
    PRE( pLocalTransform != NULL );

    //Get the domain from the planet surface
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
    W4dDomain* pDomain = pSurface->domainAt( globalLocation );

    //Create the global transform to the position
    MexTransform3d globalTransform( MexEulerAngles( zAngle, 0, 0 ), globalLocation );

    //Get the domain's global inverse transform
    MexTransform3d domainInverseTransform;
    pDomain->globalTransform().invert( &domainInverseTransform );

    //Hence compute the desired local transform
    domainInverseTransform.transform( globalTransform, pLocalTransform );

    return pDomain;
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
void MachLogPlanetDomains::intersecting( const MexAlignedBox2d& boundary, Domains* pDomains )
{
    //Get the planet surface, and hence its tile spacing
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
    MATHEX_SCALAR tileXSpacing = pSurface->xTileSpacing();
    MATHEX_SCALAR tileYSpacing = pSurface->yTileSpacing();
    MATHEX_SCALAR xPlanetMax = tileXSpacing * pSurface->nXTiles() - MexEpsilon::instance();
    MATHEX_SCALAR yPlanetMax = tileYSpacing * pSurface->nYTiles() - MexEpsilon::instance();

    //Trim the boundary to the planet, just in case
    const MexPoint2d& minCorner = boundary.minCorner();
    const MexPoint2d& maxCorner = boundary.maxCorner();
    MATHEX_SCALAR xMin = minCorner.x();
    MATHEX_SCALAR xMax = maxCorner.x();
    MATHEX_SCALAR yMin = minCorner.y();
    MATHEX_SCALAR yMax = maxCorner.y();
    bool valid = xMin <= xMax  and  yMin <= yMax;

    if( xMin < 0.0 )
        xMin = 0.0;
    else if( xMin > xPlanetMax )
        valid = false;

    if( xMax > xPlanetMax )
        xMax = xPlanetMax;
    else if( xMax < 0.0 )
        valid = false;

    if( yMin < 0.0 )
        yMin = 0.0;
    else if( yMin > yPlanetMax )
        valid = false;

    if( yMax > yPlanetMax )
        yMax = yPlanetMax;
    else if( yMax < 0.0 )
        valid = false;

    if( valid )
    {
        //Hence work out which domains we're covering
        uint xDomains = 1 + (uint)( xMax / tileXSpacing )
                          - (uint)( xMin / tileXSpacing );
        uint yDomains = 1 + (uint)( yMax / tileYSpacing )
                          - (uint)( yMin / tileYSpacing );

        MATHEX_SCALAR dy = minCorner.y();

        while( yDomains-- )
        {
            MATHEX_SCALAR dx = minCorner.x();

            for( uint i = xDomains; i--; )
            {
                pDomains->push_back( pSurface->domainAt( MexPoint3d( dx, dy, 0 ) ) );
                dx += tileXSpacing;
            }

            dy += tileYSpacing;
        }
    }
}

//static
void MachLogPlanetDomains::intersecting( const MexLine2d& line, Domains* pDomains )
{
	PRE( pDomains );
    //Get the planet surface, and hence its tile spacing
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
    MATHEX_SCALAR tileXSpacing = pSurface->xTileSpacing();
    MATHEX_SCALAR tileYSpacing = pSurface->yTileSpacing();
    size_t nXTiles = pSurface->nXTiles();
    size_t nYTiles = pSurface->nYTiles();
	MexGrid2d grid( nXTiles, nYTiles, tileXSpacing, tileYSpacing, MexPoint2d( 0, 0 ) );
	MexGrid2d::Cells cells;
	MexGrid2d::Points points;
	grid.intersect( line, &cells, &points );
	ASSERT( cells.size() > 0, "Line does not intersect with any domains\n" );
	for( MexGrid2d::Cells::iterator i = cells.begin(); i != cells.end(); ++i )
		pDomains->push_back( pSurface->domainByXYIndex( (*i).xIndex(), (*i).yIndex() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
W4dDomain* MachLogPlanetDomains::pDomainAt( const MexPoint2d& point )
{
    return MachLogPlanet::instance().surface()->domainAt( MexPoint3d( point.x(), point.y(), 0) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
MATHEX_SCALAR MachLogPlanetDomains::terrainHeight( const MexPoint3d& point )
{
    return MachLogPlanet::instance().surface()->terrainHeight( point.x(), point.y() );
}

//static
void MachLogPlanetDomains::addIntersectRelations
(
    W4dEntity* pEntity, const MexPoint3d& startPoint, const MexPoint3d& endPoint
)
{
    //Get any current domain for the entity
    W4dDomain* pEntityDomain = NULL;
    pEntity->hasContainingDomain( &pEntityDomain );

    //Get the set of intersecting domains
	Domains intersectingDomains;
	MachLogPlanetDomains::intersecting( MexLine2d( startPoint, endPoint ), &intersectingDomains );

	for( MachLogPlanetDomains::Domains::iterator i = intersectingDomains.begin(); i != intersectingDomains.end(); ++i )
	{
        W4dDomain* pDomain = (*i);
        if( pDomain != pEntityDomain )
			pEntity->intersects( pDomain, true );
	}
}
/* End PLANDOMS.CPP *************************************************/
