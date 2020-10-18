/*
 * C S 2 D M C A C . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2dmcac.hpp"

// static
PhysCS2dDomainFindPathCache& PhysCS2dDomainFindPathCache::instance()
{
    static PhysCS2dDomainFindPathCache instance_;
    return instance_;
}

PhysCS2dDomainFindPathCache::PhysCS2dDomainFindPathCache()
: oldestValue_( 0 ),
  maxCacheValues_( 10 ),
  succeed_( 0 ),
  fail_( 0 )
{

    TEST_INVARIANT;
}

bool PhysCS2dDomainFindPathCache::findPath(
    PhysConfigSpace2d::DomainId startDomainId,
    const MexPoint2d& startPoint,
    PhysConfigSpace2d::DomainId endDomainId,
    const MexPoint2d& endPoint,
    MATHEX_SCALAR clearance,
    ObstacleFlags flags,
    PortalPoints* pPath ) const
{
    bool result = false;

    for( size_t i = 0; i < cache_.size() and not result; ++i )
    {
        const MATHEX_SCALAR sqrMaxDistance = 15.0 * 15.0;
        const PathData& data = cache_[ i ];

        if( startDomainId == data.startPoint().domainId() and
            endDomainId == data.endPoint().domainId() and
            clearance <= data.searchData().clearance() and
            flags == data.searchData().flags() and
            startPoint.sqrEuclidianDistance( data.startPoint().point() ) < sqrMaxDistance and
            endPoint.sqrEuclidianDistance( data.endPoint().point() ) < sqrMaxDistance )
        {
            result = true;
            *pPath = data.path();
        }
    }

    PhysCS2dDomainFindPathCache *nonConstThis = _CONST_CAST( PhysCS2dDomainFindPathCache*, this );

    if( result )
        ++nonConstThis->succeed_;
    else
        ++nonConstThis->fail_;

    return result;
}

void PhysCS2dDomainFindPathCache::addPath(
    PhysConfigSpace2d::DomainId startDomainId,
    const MexPoint2d& startPoint,
    PhysConfigSpace2d::DomainId endDomainId,
    const MexPoint2d& endPoint,
    MATHEX_SCALAR clearance,
    ObstacleFlags flags,
    const PortalPoints& path )
{
    PathData pathData(
      Point( startDomainId, startPoint ),
      Point( endDomainId, endPoint ),
      SearchData( clearance, flags ),
      path );

    if( cache_.size() < maxCacheValues_ )
    {
        cache_.push_back( pathData );
    }
    else
    {
        cache_[ oldestValue_ ] = pathData;
        oldestValue_ = ( oldestValue_ + 1 ) % maxCacheValues_;
    }
}

PhysCS2dDomainFindPathCache::~PhysCS2dDomainFindPathCache()
{
    TEST_INVARIANT;

}

void PhysCS2dDomainFindPathCache::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysCS2dDomainFindPathCache& t )
{

    o << "PhysCS2dDomainFindPathCache " << (void*)&t << " start" << std::endl;
    o << "PhysCS2dDomainFindPathCache " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CS2DMCAC.CPP *************************************************/
