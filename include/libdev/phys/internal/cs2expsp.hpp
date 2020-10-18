/*
 * C S 2 E X P S P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dExpansionSpace

    A PhysConfigSpace2d may be requested to open a an expansion space.
    An expansion space is a PhysConfigSpace2d containing some of the
    polygons of the owning space, but expanded by a nominated expansion
    distance. This object HAS-A PhysConfigSpace2d, and other data required
    by the interafce between the owning space and that space.
*/

#ifndef _PHYS_CS2EXPSP_HPP
#define _PHYS_CS2EXPSP_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "ftl/seriamap.hpp"

#include "phys/cspace2.hpp"

//orthodox canonical( revoked )
class PhysCS2dExpansionSpace
{
public:
    //Types
    typedef PhysConfigSpace2d::PolygonId PolygonId;
    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    //ctor. The expansion space will have specified boundary, be SUBTRACTIVE mode,
    //and have arbitrary domainClearance. The distance by which polygons will be
    //expanded is expansionDistance.
    PhysCS2dExpansionSpace( const MexAlignedBox2d& boundary, MATHEX_SCALAR expansionDistance );

    //dtor.
    ~PhysCS2dExpansionSpace( void );

    //////////////////////////////////////////////////////////

    //Provides access to the expnasion config space. Note that exporting a non-const
    //pointer to the space allows clients to make unexpected modifications to it.
    //However, this is an implementation class of PhysConfigSpace2d, and no
    //clients of the original space can get at an expansion space. The only clients
    //are those which are also implementation classes of PhysConfigSpace2d.
    const PhysConfigSpace2d& configSpace( void ) const;
    PhysConfigSpace2d* pConfigSpace( void );

    //Maintain count of clients
    size_t nClients( void ) const;

    void addClient( void );

    void removeClient( void );
    //PRE( nClients() > 0 )

    //Accessors
    MATHEX_SCALAR expansionDistance( void ) const;

    //////////////////////////////////////////////////////////
    //Adds an expanded polygon to the space derived from oldPolygon, which in the master
    //space has id oldId. The id of the expanded polygon is returned as result.
    PolygonId addPolygon( const MexPolygon2d& oldPolygon,
        const PolygonId& oldId,
        ObstacleFlags flags );
    //PRE( not polygonExists( oldId ) )

    //True if the expansion space contains an expanded version of the original
    //polygon with id oldId.
    bool polygonExists( const PolygonId& oldId ) const;

    //Removes the polygon derived by expanding master polygon with id oldId
    void removePolygon( const PolygonId& oldId );
    //PRE( polygonExists( oldId ) )

    //////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dExpansionSpace& t );

private:
    // Operations deliberately revoked
    PhysCS2dExpansionSpace( const PhysCS2dExpansionSpace& );
    PhysCS2dExpansionSpace& operator =( const PhysCS2dExpansionSpace& );
    bool operator ==( const PhysCS2dExpansionSpace& );

    //Implementation types
    typedef FtlSerialMap< PolygonId > PolygonIdMap;

    //Data members
    PhysConfigSpace2d configSpace_; //The space containing the expanded polygons
    MATHEX_SCALAR expansionDistance_; //The distance by which polygons to be expanded
    PolygonIdMap idMap_; //Maps parent config space polygon id to id of corresponding
                         //expanded polygon.
    size_t nClients_; //Count of current users of this space
};

#ifdef _INLINE
    #include "internal/cs2expsp.ipp"
#endif


#endif

/* End CS2EXPSP.HPP *************************************************/
