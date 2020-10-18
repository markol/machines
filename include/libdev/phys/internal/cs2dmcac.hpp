/*
 * C S 2 D M C A C . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    PhysCS2dDomainFindPathCache

    A brief description of the class should go in here
*/

#ifndef _PHYS_CS2DMCAC_HPP
#define _PHYS_CS2DMCAC_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "ctl/utility.hpp"
#include "ctl/vector.hpp"
#include "ctl/nbvector.hpp"
#include "phys/cspace2.hpp"

class PhysCS2dDomainFindPathCache
// Canonical form revoked
{
public:
    //  Singleton class
    static PhysCS2dDomainFindPathCache& instance( void );
    ~PhysCS2dDomainFindPathCache( void );

    typedef PhysConfigSpace2d::PortalPoints PortalPoints;

    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    bool findPath(
        PhysConfigSpace2d::DomainId startDomainId,
        const MexPoint2d& startPoint, 
        PhysConfigSpace2d::DomainId endDomainId,
        const MexPoint2d& endPoint,
        MATHEX_SCALAR clearance,
        ObstacleFlags flags,
        PortalPoints* pPath ) const;
    
    void addPath(
        PhysConfigSpace2d::DomainId startDomainId,
        const MexPoint2d& startPoint, 
        PhysConfigSpace2d::DomainId endDomainId,
        const MexPoint2d& endPoint,
        MATHEX_SCALAR clearance,
        ObstacleFlags flags,
        const PortalPoints& path );
    
    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PhysCS2dDomainFindPathCache& t );

    PhysCS2dDomainFindPathCache( const PhysCS2dDomainFindPathCache& );
    PhysCS2dDomainFindPathCache& operator =( const PhysCS2dDomainFindPathCache& );

    PhysCS2dDomainFindPathCache( void );

    //  To allow template instantiation
    friend void PhysDummyFunction2();

    CTL_PAIR( Point, 
        PhysConfigSpace2d::DomainId, domainId, 
        MexPoint2d, point );    

    CTL_PAIR( SearchData,
        MATHEX_SCALAR, clearance,
        ObstacleFlags, flags );
    
    CTL_QUAD( PathData, 
        Point, startPoint, 
        Point, endPoint, 
        SearchData, searchData,
        PortalPoints, path );
    
    typedef ctl_vector< PathData >  Cache;
    
    Cache   cache_;
    uint    oldestValue_;
    const uint maxCacheValues_;

    uint    succeed_;
    uint    fail_;
};


#endif

/* End CS2DMCAC.HPP *************************************************/
