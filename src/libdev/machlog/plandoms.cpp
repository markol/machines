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
#include "mathex/abox3d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/grid2d.hpp"
#include "world4d/domain.hpp"
#include "world4d/composit.hpp"
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
	NETWORK_STREAM( "MachLogPlanetDomains::intersecting boundary " << boundary << " domains.size " << pDomains->size() << std::endl );
    //Get the planet surface, and hence its tile spacing
    MATHEX_SCALAR eps = MexEpsilon::instance();
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
    MATHEX_SCALAR tileXSpacing = pSurface->xTileSpacing();
    MATHEX_SCALAR tileYSpacing = pSurface->yTileSpacing();
    MATHEX_SCALAR xPlanetMax = tileXSpacing * pSurface->nXTiles() - eps;
    MATHEX_SCALAR yPlanetMax = tileYSpacing * pSurface->nYTiles() - eps;

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
        //To avoid including domains lying right on the border of the supplied area,
        //move the limits in by epsilon. But watch out for very narrow regions.

        MATHEX_SCALAR xStart = xMin + eps;
        MATHEX_SCALAR xEnd = xMax - eps;
        uint xDomains;
        MATHEX_SCALAR xDiff = xEnd - xStart;
        if( xDiff < 0.0 )
        {
			NETWORK_STREAM( "xDomains = 1;" << std::endl );
            xDomains = 1;
            xStart = 0.5 * (xMin + xMax);
        }
        else
		{
            xDomains = 1 + ((uint)( xEnd / tileXSpacing )) - ((uint)( xStart / tileXSpacing ));
			NETWORK_STREAM( "xDomains = " << xDomains << " xDiff " << xDiff << " tileXSpacing " << tileXSpacing << " " << (uint)( xDiff / tileXSpacing ) << std::endl );
		}

        MATHEX_SCALAR yStart = yMin + eps;
        MATHEX_SCALAR yEnd = yMax - eps;
        uint yDomains;
        MATHEX_SCALAR yDiff = yEnd - yStart;
        if( yDiff < 0.0 )
        {
			NETWORK_STREAM( "yDomains = 1;" << std::endl );
            yDomains = 1;	 
            yStart = 0.5 * (yMin + yMax);
        }
        else
		{
		    yDomains = 1 + ((uint)( yEnd / tileYSpacing )) - ((uint)( yStart / tileYSpacing ));
			NETWORK_STREAM( "yDomains = " << yDomains << " yDiff " << yDiff << " tileYSpacing " << tileYSpacing << " " << (uint)( yDiff / tileYSpacing ) << std::endl );
		}

        MATHEX_SCALAR dy = yStart;

		NETWORK_STREAM(" valid " << valid << " xDomains " << xDomains << " yDomains " << yDomains << " dy " << dy << std::endl );

        while( yDomains-- )
        {
            MATHEX_SCALAR dx = xStart;

            for( uint i = xDomains; i--; )
            {
				NETWORK_STREAM(" get domain at " << dx << " " << dy << std::endl );
                pDomains->push_back( pSurface->domainAt( MexPoint3d( dx, dy, 0 ) ) );
                dx += tileXSpacing;
            }

            dy += tileYSpacing;
        }
    }
	NETWORK_STREAM(" returning " << pDomains->size() << " domains.\n" );
}

//static
void MachLogPlanetDomains::intersecting( const MexLine2d& line, Domains* pDomains )
{
	HAL_STREAM("MachLogPlanetDomains::intersecting " << line << std::endl );
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
	{
		pDomains->push_back( pSurface->domainByXYIndex( (*i).xIndex(), (*i).yIndex() ) );
		HAL_STREAM(" intersect with domain at X " << (*i).xIndex() << " by Y " << (*i).yIndex() << std::endl );
	}
	HAL_STREAM(" intersecting domains size " << pDomains->size() << std::endl );
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

//static
void MachLogPlanetDomains::addIntersectRelations( W4dEntity* pEntity )
{
    //Get any current domain for the entity
    W4dDomain* pEntityDomain = NULL;
    pEntity->hasContainingDomain( &pEntityDomain );

    //Get the bounding box for the entity
    const MexAlignedBox3d& boundingBox = ( pEntity->isComposite() ? pEntity->asComposite().compositeBoundingVolume()
                                                                  : pEntity->boundingVolume() );
    //Convert to a world aligned 2d boundary
    MexAlignedBox2d boundary = convertBoundary( boundingBox, pEntity->globalTransform() );

    //Get the set of intersecting domains
	Domains intersectingDomains;
	MachLogPlanetDomains::intersecting( boundary, &intersectingDomains );

    //Attach th eintersect relations
	for( MachLogPlanetDomains::Domains::iterator i = intersectingDomains.begin(); i != intersectingDomains.end(); ++i )
	{
        W4dDomain* pDomain = (*i);
        if( pDomain != pEntityDomain )
			pEntity->intersects( pDomain, true );
	}
}

//static
MexAlignedBox2d MachLogPlanetDomains::convertBoundary( const MexAlignedBox3d& box3d, const MexTransform3d& globalTransform )
{
    const MexPoint3d& minCorner = box3d.minCorner();
    const MexPoint3d& maxCorner = box3d.maxCorner();

    //Get the 2d projection of the global mincorner
    MexPoint3d localCorner( minCorner.x(), minCorner.y(), minCorner.z() );
    MexPoint3d globalCorner( localCorner );
    globalTransform.transform( &globalCorner );
    MATHEX_SCALAR xMin = globalCorner.x();
    MATHEX_SCALAR xMax = xMin;
    MATHEX_SCALAR yMin = globalCorner.y();
    MATHEX_SCALAR yMax = yMin;

    //Convert the other corners and maintain the min/max boundary
    MATHEX_SCALAR x, y;


#define EXTEND                                         \
    globalCorner = localCorner;                        \
    globalTransform.transform( &globalCorner );        \
    x = globalCorner.x();                              \
    y = globalCorner.y();                              \
    if( x > xMax )                                     \
        xMax = x;                                      \
    else if( x < xMin )                                \
        xMin = x;                                      \
    if( y > yMax )                                     \
        yMax = y;                                      \
    else if( y < yMin )                                \
        yMin = y;

    localCorner.z( maxCorner.z() );
    EXTEND

    localCorner.y( maxCorner.y() );
    EXTEND

    localCorner.z( minCorner.z() );
    EXTEND

    localCorner.x( maxCorner.x() );
    EXTEND

    localCorner.y( minCorner.y() );
    EXTEND

    localCorner.z( maxCorner.z() );
    EXTEND

    localCorner.y( maxCorner.y() );
    EXTEND

    return MexAlignedBox2d( xMin, yMin, xMax, yMax );
}

//static
bool MachLogPlanetDomains::onPlanet(  const MexPoint2d& point )
{
    //Get the planet surface, and hence its tile spacing
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
    MATHEX_SCALAR x = point.x();
    MATHEX_SCALAR y = point.y();

    return (x >= pSurface->xMin()) and (x <= pSurface->xMax()) and
           (y >= pSurface->yMin()) and (y <= pSurface->yMax());
}
/* End PLANDOMS.CPP *************************************************/
