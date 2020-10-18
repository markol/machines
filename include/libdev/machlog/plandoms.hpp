/*
 * P L A N D O M S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogPlanetDomains

    Provides static helper functions related to domains on the machines plant
*/

#ifndef _MACHLOG_PLANDOMS_HPP
#define _MACHLOG_PLANDOMS_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

//Forward declarations
class W4dDomain;
class MexAlignedBox2d;
class MexPoint2d;
class MexPoint3d;
class MexLine2d;
class MexTransform3d;
class MexRadians;
class W4dEntity;

template < class X > class ctl_vector;

//static methods
class MachLogPlanetDomains
{
public:
    typedef ctl_vector< W4dDomain* > Domains;

    //To position an entity with origin at globalLocation, rotated zAngle about the z axis,
    //it could be attached to returned domain, with position relative to the domain
    //returned in pLocalTransform
    static
    W4dDomain* pDomainPosition( const MexPoint3d& globalLocation, const MexRadians& zAngle,
                                MexTransform3d* pLocalTransform );
    //PRE( pLocalTransform != NULL )

    //Push pointers to all domains intersecting boundary into pDomains
    static void intersecting( const MexAlignedBox2d& boundary, Domains* pDomains );
    //PRE( pDomains != NULL )

    //Push pointers to all domains intersecting with line into pDomains
	//uses MexGrid2d for calculations
    static void intersecting( const MexLine2d& line, Domains* pDomains );
    //PRE( pDomains != NULL )

    //The domain in which point lies
    static W4dDomain* pDomainAt( const MexPoint2d& point );

    //The terrain height at point's x/y coords
    static MATHEX_SCALAR terrainHeight( const MexPoint3d& point );

    //True if the coordinate lies inside the planet boundary
    static bool onPlanet(  const MexPoint2d& point );

    //Add intersect relations to pEntity for all the domains traversed in a line from startPoint to endPoint
    static void addIntersectRelations( W4dEntity* pEntity, const MexPoint3d& startPoint, const MexPoint3d& endPoint );

    //Add intersect relations to pEntity for all the domains encompassed by its bounding volume,
    //except its owning domain. If pEntity is a composite, uses the composite volume.
    static void addIntersectRelations( W4dEntity* pEntity );

    //returns a world aligned 2d boundary which is the projection of box3d transformed by globalTransform
    static MexAlignedBox2d convertBoundary( const MexAlignedBox3d& box3d, const MexTransform3d& globalTransform );

private:
    // Operation deliberately revoked
    MachLogPlanetDomains( const MachLogPlanetDomains& );
    MachLogPlanetDomains& operator =( const MachLogPlanetDomains& );
    bool operator ==( const MachLogPlanetDomains& );

    //No instantiation allowed - all methods static
    MachLogPlanetDomains( void );
    ~MachLogPlanetDomains( void );
};


#endif

/* End PLANDOMS.HPP *************************************************/
