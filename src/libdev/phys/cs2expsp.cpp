/*
 * C S 2 E X P S P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/abox2d.hpp"
#include "mathex/cvexpgon.hpp"

#include "phys/cspace2.hpp"
#include "phys/internal/cs2expsp.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2expsp.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dExpansionSpace::PhysCS2dExpansionSpace
(
    const MexAlignedBox2d& boundary, MATHEX_SCALAR expansionDistance
)
:   configSpace_( boundary.minCorner(),
                  boundary.maxCorner(),
                  PhysConfigSpace2d::SUBTRACTIVE,
                  0, 1.0, 8.0, 0 ),
    expansionDistance_( expansionDistance ),
    nClients_( 0 )
{

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dExpansionSpace::~PhysCS2dExpansionSpace()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dExpansionSpace::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dExpansionSpace& t )
{

    o << "PhysCS2dExpansionSpace " << (void*)&t << " start" << std::endl;
    o << "PhysCS2dExpansionSpace " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::PolygonId PhysCS2dExpansionSpace::addPolygon
(
    const MexPolygon2d& oldPolygon, const PolygonId& oldId, ObstacleFlags flags
)
{
    PRE( not polygonExists( oldId ) )

    CS2VGRA_STREAM( "Adding polygon to expansion space" << std::endl );
    CS2VGRA_STREAM( "Unexpanded polygon " << oldPolygon << std::endl );

    //Construct a new expanded polygon
    MexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( oldPolygon, expansionDistance_ ) );
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

    CS2VGRA_STREAM( "Expanded polygon   " << *pPolygon << std::endl );

    //Add to the space
    PolygonId id = configSpace_.add( polygonAPtr, 1, flags, PhysConfigSpace2d::PERMANENT );

    //Add the polygon to the map
    idMap_.add( FtlPair<PolygonId, PolygonId>( oldId, id ) );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dExpansionSpace::polygonExists( const PolygonId& oldId ) const
{
    return idMap_.contains( oldId ) and configSpace_.exists( idMap_[ oldId ] );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dExpansionSpace::removePolygon( const PolygonId& oldId )
{
    PRE( polygonExists( oldId ) );

    //Get the expanded polygon id and remove it from the space
    PolygonId expandedId = idMap_[ oldId ];
    configSpace_.remove( expandedId );

    //Remove the entry from the map
    idMap_.remove( oldId );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2EXPSP.CPP *************************************************/
