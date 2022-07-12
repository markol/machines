/*
 * C S P A C E 2 . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysConfigSpace2d

    An instance of this class represents a subset of 2d space.
*/

#ifndef _PHYS_CSPACE2_HPP
#define _PHYS_CSPACE2_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "ftl/serialid.hpp"
#include "phys/phys.hpp"
#include "ctl/utility.hpp"
#include <memory>

//Forward declarations
class MexPoint2d;
class MexPoint3d;
class MexPolygon2d;
class MexCircle2d;
class MexPolygon3d;
class MexAlignedBox2d;
class MexSausage2d;
//class ostream;
class PhysCS2dImpl;
class PhysCS2dExpansionSpace;
class PhysMotionChunk;
class PhysPathFindingQueueWriter;
class PhysCS2dDomain;
class MexDouble;
class PhysCS2dFindPath;
class PhysCS2dDomainFindPath;
template < class T > class ctl_pvector;
template < class T > class ctl_nb_vector;
//template < class T > class std::auto_ptr;
//template < class X, class Y > class pair;


// Warnings about foward references of template classes
#ifndef CB_NOWARNINGS // Are Charybdis warnings turned on?
#ifndef _MATHEX_POLY2D_HPP
#pragma message( __FILE__ "(39): Warning : std::auto_ptr< MexPolygon2d > foward referenced" )
#endif

#ifndef _MATHEX_POINT2D_HPP
#pragma message( __FILE__ "(43): Warning : ctl_nb_vector< MexPoint2d > foward referenced" )
#endif
#endif

//Orthodox canonical (revoked)
class PhysConfigSpace2d
{
public:

    typedef uint32  ObstacleFlags;

    //ctor. The space is bounded by minPoint and maxPoint.
    //If the mode is SUBTRACTIVE, then the whole of the initial rectangle is deemed
    //to be included in the space, and as polygons are registered, these are deemed
    //to be excluded from the space.
    //Vice verse for ADDITIVE.
    //domainClearance is the primary value to be used for clearance in pathfinding.
    // minPortalClearance and maxPortalClearance define the range of clearances
    // to be used when trying to place portal points. The system will try for
    // the biggest clearance first but reduce it gradually to the smaller clearance
    // if necessary.
    enum Mode { ADDITIVE, SUBTRACTIVE };
    PhysConfigSpace2d( const MexPoint2d& minPoint, const MexPoint2d& maxPoint, Mode mode,
                       MATHEX_SCALAR domainClearance, MATHEX_SCALAR minPortalClearance,
                       MATHEX_SCALAR maxPortalClearance, ObstacleFlags allObstacleFlags );
    //PRE( minPoint.x() < maxPoint.x() )
    //PRE( minPoint.y() < maxPoint.y() )

    //dtor.
    ~PhysConfigSpace2d();

    //Identifies various entites
    typedef FtlSerialId PolygonId; //A registered polygon
    typedef FtlSerialId FindPathId; //A findPath algorithm run
    typedef FtlSerialId DomainFindPathId; //A domainFindPath algorithm run
    typedef FtlSerialId DomainId; //A registered domain
    typedef FtlSerialId PortalId; //A registered portal
    typedef FtlSerialId DomainVertexId; //A DomainVertex - a graph node on a portal
    typedef FtlSerialId DomainArcId; //A DomainArc - an graph arc within a domain
    typedef FtlSerialId ObjectId; //Client defined id
    typedef FtlSerialId MotionChunkId; //A registered motion chunk

    //Polygons are classified by these types
    enum Longevity {PERMANENT, TEMPORARY};

    //Flag type for which sets of polygons to use in various operations
    enum UsePolygons {USE_TEMPORARY_ONLY, USE_PERMANENT_ONLY, USE_ALL};

    //Useful collections
    typedef ctl_pvector< const MexPolygon3d* > MexPolygon3ds;
    typedef ctl_nb_vector< MexPoint2d > Path;
    typedef ctl_nb_vector< PolygonId > PolygonIds;
    typedef std::pair< PortalId, MATHEX_SCALAR > PortalPoint;
    typedef ctl_nb_vector< PortalPoint > PortalPoints;

    //The space's mode
    Mode mode() const;

    //Export the space's boundary
    const MexAlignedBox2d& boundary() const;

    //Add a domain to the space. Domains may be used as a convenience for gross navigation
    //around large spaces. A bounding box for the domain is supplied as box.
    //If boundaryAPtr is specified, it represents the actual boundary of the domain. Otherwise
    //box is assumed to be the boundary.
    DomainId addDomain( const MexAlignedBox2d& box );
    DomainId addDomain( const MexAlignedBox2d& box,
                        std::auto_ptr< MexPolygon2d >& boundaryAPtr );
    //PRE( mode() == SUBTRACTIVE )

    //True iff a domain with id id has been defined
    bool domainExists( const DomainId& id ) const;
    //PRE( mode() == SUBTRACTIVE )

    //Remove the domain with id id.
    void removeDomain( const DomainId& id );
    //PRE( mode() == SUBTRACTIVE )
    //PRE( domainExists( id ) )

    //True iff point is contained by a domain, in which case its id is return in pId.
    bool domain( const MexPoint2d& point, DomainId* pId ) const;
    //PRE( mode() == SUBTRACTIVE )

    //Adds a portal between domains id1 and id2 consisting of the line segment
    //between point1 and point2.
    PortalId addPortal( const DomainId& id1, const DomainId& id2,
                        const MexPoint2d& point1, const MexPoint2d& point2 );
    //PRE( mode() == SUBTRACTIVE )
    //PRE( domainExists( id1 ) )
    //PRE( domainExists( id2 ) )

    //True iff a portal with id id has been defined
    bool portalExists( const PortalId& id ) const;
    //PRE( mode() == SUBTRACTIVE )

    //Remove the portal with id id.
    void removePortal( const PortalId& id );
    //PRE( mode() == SUBTRACTIVE )
    //PRE( portalExists( id ) )

    //The length of specified portal
    MATHEX_SCALAR portalLength( const PortalId& id ) const;
    //PRE( mode() == SUBTRACTIVE )
    //PRE( portalExists( id ) )

    //A point on portal with id, distance along the portal from first point
    MexPoint2d portalPoint( const PortalId& id, MATHEX_SCALAR distance ) const;
    //PRE( mode() == SUBTRACTIVE )
    //PRE( portalExists( id ) )

    ///////////////////////////////////////////////////////////////
    //*polygonAPtr is added to the configuration space, with associated height.
    //The polygons are classified as either permanent or temporary. Temporary polygons
    //are ignored for the purposes of pathFinding. The flags associated with a polygon
    //allow it to be excluded for pathfinding purposes
    PolygonId add( std::auto_ptr< MexPolygon2d >& polygonAPtr,
      MATHEX_SCALAR height, ObstacleFlags flags, Longevity type );
    //The polygon must be convex

    //True if polygon(s) have been defined for id.
    bool exists( const PolygonId& id ) const;

    //gets/sets the type of polygon(s) registered with id id
    Longevity longevity( const PolygonId& id ) const;
    void longevity( const PolygonId& id, Longevity type );
    //PRE( exists( id ) )

    //Remove the polygon(s) registered with id.
    void remove( PolygonId id );
    //PRE( exists( id ) )

    //Indicates whether the portal points on the domain graph will be updated immediately
    //as permanent obstacles are added or removed, or buffered til the next pathfind.
    //By default updates are not buffered, and this is best when obstacles are added
    //sparsely. However, when large groups of polygons are added, such as during initial
    //setup, it will be much more efficient to have them buffered.
    void bufferPolygonPortalUpdates( bool doBuffer );
    bool bufferPolygonPortalUpdates();

    //The polygon with id polygonId
    const MexPolygon2d& polygon( const PolygonId& polygonId ) const;
    //PRE( exists( polygonId ) )

    //Enable/disable the specified polygon for containment and intersection tests etc
    void isPolygonEnabled( const PolygonId& polygonId, bool enable );
    //PRE( exists( polygonId ) )

    //Return enabled flag for specified polygon
    bool isPolygonEnabled( const PolygonId& polygonId ) const;
    //PRE( exists( polygonId ) )

    //Number of polygons registered in the space
    size_t nPolygons() const;
    ///////////////////////////////////////////////////////////////

    typedef ctl_nb_vector< ObjectId >  ObjectIds;

    //Attempt to add motionChunk to the space, for object with client defined id obectId.
    //True if successful, in which case pId returns the motion chunk id.
    //False if potential collision with some other motion chunk (except any with
    //object ids in the ignoreIds vector). In this case the time of the earliest
    //potential collision is returned
    //in pCollideTime, with the id of the obstructing object in pCollisionObjectId.
    //flags defines those obstacles which can be ignored when adding the chunk
    bool add(
        const PhysMotionChunk& motionChunk,
        const ObjectId& objectId,
        const ObjectIds& ignoreIds,
        MotionChunkId *pId,
        ObjectId* pCollisionObjectId,
        PhysAbsoluteTime* pCollideTime );

    //  As above but only ignore the given object id.
    bool add(
        const PhysMotionChunk& motionChunk,
        const ObjectId& objectId,
        MotionChunkId *pId,
        ObjectId* pCollisionObjectId,
        PhysAbsoluteTime* pCollideTime );

    CTL_PAIR( ChunkIntersectionDataNoTime,
        ObjectId, collisionObjectId,
        MotionChunkId, collisionChunkId );

    typedef ctl_nb_vector< ChunkIntersectionDataNoTime >	ChunkIntersectionsNoTime;

    //As the previous add method, except that this will only check for collision
    //with those motion chunks defined in checkChunks. This allows some
    //optimisations - if we are trying the same motion chunk but at different
    //times or speeds we can use a cached set of chunks to check against and
    //save recalculating it.
    bool add(
        const PhysMotionChunk& motionChunk,
        const ObjectId& objectId,
        const ObjectIds& ignoreIds,
        const ChunkIntersectionsNoTime& checkChunks,
        MotionChunkId *pId,
        ObjectId* pCollisionObjectId,
        PhysAbsoluteTime* pCollideTime );

    //Add the motion chunk associated with object with specified id.
    //Does not check for intersections with existing motion chunks.
    //returns id of registered chunk.
    MotionChunkId add( const PhysMotionChunk& motionChunk, const ObjectId& objectId );

    //True if motion chunk has been defined for id.
    bool motionChunkExists( const MotionChunkId& id ) const;

    //Remove the motion chunk registered with id.
    void removeMotionChunk( const MotionChunkId& id );
    //PRE( motionChunkExists( id ) )

    //The motion chunk with id
    const PhysMotionChunk& motionChunk( const MotionChunkId& id ) const;
    //PRE( motionChunkExists( id ) )

    //The client object id of the registered motion chunk with id
    const ObjectId& motionChunkObjectId( const MotionChunkId& id ) const;
    //PRE( motionChunkExists( id ) )
    ///////////////////////////////////////////////////////////////

    class ChunkIntersectionData
    {
    public:
        ChunkIntersectionData()
        {}
        ChunkIntersectionData( const ChunkIntersectionData& copyMe )
        : collisionObjectId_( copyMe.collisionObjectId_ ),
          collisionChunkId_( copyMe.collisionChunkId_ ),
          collisionTime_( copyMe.collisionTime_ )
        {}
        ChunkIntersectionData( const ObjectId& id, const MotionChunkId& chunkId, const PhysAbsoluteTime& time )
        : collisionObjectId_( id ), collisionChunkId_( chunkId ), collisionTime_( time )
        {}
        ~ChunkIntersectionData()
        {}

        friend bool operator< ( const ChunkIntersectionData& lhs, const ChunkIntersectionData& rhs )
        {
            return lhs.collisionTime_ < rhs.collisionTime_;
        };

	    ObjectId 		    collisionObjectId_;
	    MotionChunkId       collisionChunkId_;
    	PhysAbsoluteTime	collisionTime_;
    };

    typedef ctl_nb_vector< ChunkIntersectionData >	ChunkIntersections;

    // Get any potential collisions between the given motion chunk and existing motion chunks in this config space.
    // Intersections with chunks having object id objectId are ignored.
    // Ids of chunks which might collide are returned in pChunkIntersections
    void findIntersections( const PhysMotionChunk& motionChunk, const ObjectId& objectId,
              ChunkIntersections* pChunkIntersections ) const;

    // Get any potential collisions between the given box and existing motion chunks in this config space.
    // Ids of chunks which might collide are returned in pChunkIntersections
    // Note that this function does not return intersection times
    void findIntersections( const MexAlignedBox2d& box,
              ChunkIntersectionsNoTime* pChunkIntersections ) const;

    // Get any potential collisions between the given sausage and existing motion chunks in this config space.
    // Intersections with chunks having is in ignoreObjectIds are ignored.
    // Ids of chunks which might collide are returned in pChunkIntersections
    // Note that this function does not return intersection times
    void findIntersections( const MexSausage2d& sausage, const ObjectIds& ignoreObjectIds,
              ChunkIntersectionsNoTime* pChunkIntersections ) const;

    // Get any potential collisions between the given sausage and existing motion chunks in this config space.
    // Intersections with chunks having object ids in the vector ignoreObjectIds are ignored.
    // Object ids of chunks which might collide are returned in pIntersectionIds
    void findIntersections(
        const MexSausage2d& sausage,
        const MexDouble& minHeight,
        const MexDouble& maxHeight,
        PhysAbsoluteTime startTime,
        PhysAbsoluteTime endTime,
        const ObjectIds& ignoreObjectIds,
        ObjectIds* pIntersectionIds ) const;

    //  Returns true iff the given box intersects with any motion chunks
    //  in the config space. If all you want is a simple yes / no answer
    //  this is quicker than findIntersections.
    bool intersectsWithMotionChunk( const MexAlignedBox2d& box ) const;

    ///////////////////////////////////////////////////////////////

    class IntersectionData
    {
    public:
        IntersectionData()
        {}
        IntersectionData( const IntersectionData& copyMe )
        : collisionObjectId_( copyMe.collisionObjectId_ ), collisionTime_( copyMe.collisionTime_ )
        {}
        IntersectionData( const ObjectId& id, const PhysAbsoluteTime& time )
        : collisionObjectId_( id ), collisionTime_( time )
        {}
        ~IntersectionData()
        {}

	    ObjectId 		    collisionObjectId_;
    	PhysAbsoluteTime	collisionTime_;
    };

    typedef ctl_nb_vector< IntersectionData >	Intersections;

    // Get any intersections between the given motion chunk and objects
    // currently in the configuration space, other than those with object id objectId.
    void findIntersections( const PhysMotionChunk& motionChunk, const ObjectId& objectId,
              Intersections* pIntersections ) const;

    ///////////////////////////////////////////////////////////////

    //True iff the polygon is completely contained in the non-excluded portion
    //of space, with reference to the set of polygons specified by whichPolygons.
    //If result is false, pId is updated to contain the id of a registered polygon which
    //intersects the test polygon.
    //flags defines those obstacles which can be ignored
    bool contains( const MexPolygon2d& polygon, ObstacleFlags flags, PolygonId* pId,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY) const;
    //PRE( mode() == SUBTRACTIVE )

    //True iff the polygons are completely contained in the non-excluded portion
    //of space, with reference to the set of polygons specified by whichPolygons.
    //If result is false, the ids of all registered polygons which
    //intersects the test polygon are appended to pIds.
    //flags defines those obstacles which can be ignored
    bool contains( const MexPolygon2d& polygon, ObstacleFlags flags, PolygonIds* pIds,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY) const;
    //PRE( mode() == SUBTRACTIVE )

    //True iff the sausage is completely contained in the non-excluded portion
    //of space, with reference to the set of polygons specified by whichPolygons.
    //If result is false, the ids of all registered polygons which
    //intersects the test sausage are appended to pIds.
    //flags defines those obstacles which can be ignored
    bool contains( const MexSausage2d& sausage, ObstacleFlags flags, PolygonIds* pIds,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY) const;
    //PRE( mode() == SUBTRACTIVE )

    //True iff the circle is completely contained in the non-excluded portion
    //of space, with reference to the set of polygons specified by whichPolygons.
    //If result is false, pId is updated to contain the id of a registered polygon which
    //intersects the test circle.
    //flags defines those obstacles which can be ignored
    bool contains( const MexCircle2d& circle, ObstacleFlags flags, PolygonId* pId,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY) const;
    //PRE( mode() == SUBTRACTIVE )

    //True iff the line from p1 to p2 does not intersect any polygons of the type(s)
    //specified by whichPolygons.
    //Account is taken of the 2.5d info for excluding polygons, in conjunction with
    //the z coordinate of the 2 points.
    //Hence, an excluding polygon does not intersect the line if its
    //max z coordinate is less than the test point z coordinates.
    //If result is false, pId is updated to contain the id of a registered polygon which
    //intersects the line.
    //flags defines those obstacles which can be ignored
    bool contains( const MexPoint3d& p1, const MexPoint3d& p2, ObstacleFlags flags, PolygonId* pId,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY ) const;
    //PRE( mode() == SUBTRACTIVE )

    //true iff testPoint is not contained by any registered polygons for a SUBTRACTIVE
    //space, and vice versa for ADDITIVE. The polygons tested are those of the type(s)
    //specified by whichPolygons.
    //If result is false, pId is updated to contain the id of a registered polygon which
    //intersects the test point.
    //flags defines those obstacles which can be ignored
    bool contains( const MexPoint2d& testPoint, ObstacleFlags flags, PolygonId* pId,
                   UsePolygons whichPolygons = USE_PERMANENT_ONLY ) const;

    /////////////////////////////////////////////////////////
    //Set up to find a short path from startPoint to endPoint, avoiding all the
    //registered polygons of type PERMANENT. The path is to get no closer than
    //clearance to the polygons. Note that use of many different values
    //for clearance will greatly decrease the efficiency of the space.
    //Returned id is used to pursue the pathfinding algorithm.
    //flags defines those obstacles which can be ignored
    FindPathId addFindPath( const MexPoint2d& startPoint, const MexPoint2d& endPoint,
                            MATHEX_SCALAR clearance, ObstacleFlags flags,
                            PhysPathFindingPriority priority );
    //PRE( mode() == SUBTRACTIVE )
    //PRE( contains( startPoint ) )
    //PRE( contains( endPoint ) )
    //PRE( clearance > 0 )

    //Ignore the polygons with ids listed in polygons for pathfind havind id id.
    //This is not cumulative (ie in successive calls only the latest is used).
    //The client must ensure that the polygons collection persists as long as
    //the pathFind exists.
    void findPathIgnorePolygons( const FindPathId& id, const PolygonIds& polygons );
    //PRE( findPathExists( id ) )

    //True iff a findPath with id id is still current
    bool findPathExists( const FindPathId& id ) const;

    //Perform another iteration towards finding the path requested with id.
    //True iff finished searching. Don't spend longer than maxTime doing it.
    bool updateFindPath( const FindPathId& id, const PhysRelativeTime& maxTime );
    //PRE( findPathExists( id ) )

    //true iff the pathFind with id has completed
    bool isFindPathDone( const FindPathId& id ) const;
    //PRE( findPathExists( id ) )

    //True if a path was found between the points for findPath run with id id.
    //If so returns the sequence of points in pPath, including the start
    //and end points.
    //Then removes the findPath.
    bool findPath( const FindPathId& id, Path* pPath );
    //PRE( findPathExists( id ) )
    //PRE( isFindPathDone( id ) )

    //Removes id from the space without completing it.
    void removeFindPath( const FindPathId& id );
    //PRE( findPathExists( id ) )

    //Debugging output for current state
    void traceFindPath( const FindPathId& id, ostream& o );
    //PRE( findPathExists( id ) )

    /////////////////////////////////////////////////////////
    //Set up to find a route from startPoint to endPoint, navigating via domain portals.
    //Returns the id of this findpath.
    DomainFindPathId addDomainFindPath( const MexPoint2d& startPoint,
      const MexPoint2d& endPoint, MATHEX_SCALAR clearance,
      ObstacleFlags flags, PhysPathFindingPriority priority );
    //PRE( mode() == SUBTRACTIVE )

    //True iff a domainFindPath with id id is still current
    bool domainFindPathExists( const DomainFindPathId& id ) const;

    //Perform another iteration towards finding the domain path requested with id.
    //True iff finished searching. Don't spend longer than maxTime doing it.
    bool updateDomainFindPath( const DomainFindPathId& id, const PhysRelativeTime& maxTime );
    //PRE( domainFindPathExists( id ) )

    //true iff the domainFathFind with id has completed
    bool isDomainFindPathDone( const DomainFindPathId& id ) const;
    //PRE( domainFindPathExists( id ) )

    //True if a route was found between the points for domainFindPath run with id id.
    //If so returns the sequence of intervening portal points in pPortalPoints,
    //excluding the start and end points.
    //Then removes the domainFindPath.
    bool domainFindPath( const DomainFindPathId& id, PortalPoints* pPortalPoints );
    //PRE( domainFindPathExists( id ) )
    //PRE( isDomainFindPathDone( id ) )

    //Removes id from the space without completing it.
    void removeDomainFindPath( const DomainFindPathId& id );
    //PRE( domainFindPathExists( id ) )

    //Debugging output for current state
    void traceDomainFindPath( const DomainFindPathId& id, ostream& o );
    //PRE( domainFindPathExists( id ) )
    /////////////////////////////////////////////////////////

    //True if can find a point at which to place a polygon with sides
    //clearance * root(2), a distance radius from targetPoint.
    //If successful the answer is returned in pResult.
    //The first point that is tried will lie on the line joining
    //startPoint to targetPoint.
    //flags defines those obstacles which can be ignored
    bool findSpace( const MexPoint2d& startPoint, const MexPoint2d& targetPoint,
                    MATHEX_SCALAR clearance, MATHEX_SCALAR radius,
                    ObstacleFlags flags, MexPoint2d* pResult );

    //  Identical to the other findSpace method except that this method
    //  will try points at random rather than needing an explicit start
    //  point.
    bool findSpace( const MexPoint2d& targetPoint,
                    MATHEX_SCALAR clearance, MATHEX_SCALAR radius,
                    ObstacleFlags flags, MexPoint2d* pResult );

    /////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysConfigSpace2d& t );

    ostream& traceObstacles
    (
        ostream& o, UsePolygons whichPolygons,
        MATHEX_SCALAR xStart, MATHEX_SCALAR xEnd, MATHEX_SCALAR xIncr,
        MATHEX_SCALAR yStart, MATHEX_SCALAR yEnd, MATHEX_SCALAR yIncr
    );

    //  Trace the entire config space
    ostream& traceObstacles
    (
        ostream& o
    );

    //  Write out any points which are not in an obstacle and not in a domain
    void traceIllegalPoints( ostream& o ) const;

private:
    friend class PhysCS2dFindPath;
    friend class PhysCS2dDomainFindPath;
    friend class PhysCS2dVisibilityGraph;
    friend ostream& operator <<( ostream& o, const PhysPathFindingQueueWriter& t );

    // Operations deliberately revoked
    PhysConfigSpace2d( const PhysConfigSpace2d& );
    PhysConfigSpace2d& operator =( const PhysConfigSpace2d& );
    bool operator ==( const PhysConfigSpace2d& );

    //Export the implementation to friends
    const PhysCS2dImpl& impl() const;
    PhysCS2dImpl* pImpl();

    //Set/get option to maintain visibility graph between vertices of permanent polygons.
    void isMaintainingVisibilityGraph( bool doMaintain, PhysConfigSpace2d* pOriginalSpace,
                                       PhysCS2dExpansionSpace* pExpansionSpace, MATHEX_SCALAR clearance );
    bool isMaintainingVisibilityGraph() const;

    PhysCS2dDomainFindPath* pCurrentDomainFindPath();
    // PRE( impl_.domainFindPathQueue().size() > 0 );
    // POST( result == NULL );

    void removeCurrentDomainFindPathFromQueue();
    // PRE( pImpl_->hasCurrentDomainFindPath_ );
    // POST( not pImpl_->hasCurrentDomainFindPath_ );

    PhysCS2dFindPath* pCurrentFindPath();
    // PRE( impl_.findPathQueue().size() > 0 );
    // POST( result == NULL );

    void removeCurrentFindPathFromQueue();
    // PRE( pImpl_->hasCurrentFindPath_ );
    // POST( not pImpl_->hasCurrentFindPath_ );

    //  Returns true iff the given point is in a domain
    //  or in an obstacle
    bool pointValid(
        const MexPoint2d& location,
        PhysCS2dDomain** ppCachedDomain ) const;

    //  Adjust the limits of the ascii picture of the config space so that
    //  each character has dimensions that are a nice whole number of metres.
    void adjustLimits(
        MATHEX_SCALAR* pXStart, MATHEX_SCALAR* pXEnd,
        MATHEX_SCALAR* pYStart, MATHEX_SCALAR* pYEnd,
        uint* pNColumns );

    //Data members
    PhysCS2dImpl& impl_;
};

#ifdef _INLINE
    #include "phys/cspace2.ipp"
#endif


#endif

/* End CSPACE2.HPP **************************************************/
