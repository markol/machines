/*
 * C S 2 D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dImpl

    Class used to store the implementation data for PhysConfigSpace2d.
    Avoids the need to include all the data item header files in the
    hpp file for that class.
*/

#ifndef _PHYS_CS2DATA_HPP
#define _PHYS_CS2DATA_HPP

#include "base/base.hpp"
#include "ctl/utility.hpp"
#include "ctl/pvector.hpp"
#include "ftl/serialid.hpp"
#include "ftl/seripmap.hpp"
#include "ftl/seriamap.hpp"
#include "ftl/reuidgen.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox2d.hpp"

#include "phys/absp2d.hpp"
#include "phys/cspace2.hpp"
#include "phys/internal/cs2domai.hpp"
#include "phys/internal/cs2porta.hpp"
#include "phys/internal/cs2dmarc.hpp"
#include "phys/internal/cs2dmvtx.hpp"
#include "phys/internal/cs2dmgra.hpp"
#include "phys/internal/cs2polyg.hpp"
#include "phys/internal/cs2findp.hpp"
#include "phys/internal/cs2mochu.hpp"

//Forward declarations
//class ostream;
class MexPoint2d;
class PhysCS2dVisibilityGraph;
class PhysCS2dExpansionSpace;
class PhysCS2dDomainFindPath;

//orthodox canonical (revoked)
class PhysCS2dImpl
{
public:
    void CLASS_INVARIANT;

    //Various typedefs
    typedef PhysConfigSpace2d::PolygonId PolygonId;
    typedef PhysConfigSpace2d::DomainId DomainId;
    typedef PhysConfigSpace2d::MotionChunkId MotionChunkId;
    typedef PhysConfigSpace2d::ObjectId ObjectId;
    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    typedef FtlSerialPMap< PhysCS2dDomain > DomainMap;
    typedef PhysAlignedBSPTree2d< PhysCS2dDomain* > DomainTree;

    typedef PhysConfigSpace2d::PortalId PortalId;
    typedef FtlSerialPMap< PhysCS2dPortal > PortalMap;

    typedef PhysConfigSpace2d::DomainVertexId DomainVertexId;
    typedef PhysConfigSpace2d::DomainArcId DomainArcId;

    typedef FtlSerialPMap< PhysCS2dPolygon > PolygonMap;
    typedef PhysAlignedBSPTree2d< PhysCS2dPolygon* > PolygonTree;

    typedef FtlSerialPMap< PhysCS2dMotionChunk > MotionChunkMap;
    typedef PhysAlignedBSPTree2d< PhysCS2dMotionChunk* > MotionChunkTree;

    typedef FtlSerialPMap< PhysCS2dFindPath > FindPathMap;
    typedef ctl_pvector< PhysCS2dFindPath > FindPaths;
    typedef FtlSerialPMap< PhysCS2dDomainFindPath > DomainFindPathMap;
    typedef ctl_pvector< PhysCS2dDomainFindPath > DomainFindPaths;

    /////////////////////////////////////////////////////
    //Access to the domain graph
    const PhysCS2dDomainGraph& domainGraph() const;

    //Add a DomainArc joining domainVertexId to each other
    //DomainVertex on domain domainId's portals, except the portal which
    //that vertex lies on.
    void addDomainArcs( const DomainVertexId& domainVertexId,
                        const DomainId& domainId );

    //Add a vertex to the domain graph at point, returning the vertex id.
    DomainVertexId addDomainVertex( const MexPoint2d& point, MATHEX_SCALAR clearance,
      ObstacleFlags flags );

    //Remove the domain vertex with id id from the domain graph.
    //Also removes any arcs connected to it.
    void removeDomainVertex( const DomainVertexId& id );

    //Get/set the domainGraph in use flag. This should be set when eg a sliced algorithm
    //wants to make use of the graph, and will have to restart if its version changes.
    //Clients should delay editing the graph when in use if possible.
    //The set method is incremental so if 2 clients flag as in use, they must both free
    //it, or it will be permanently set in use.
    bool domainGraphInUse() const;
    void domainGraphInUse( bool inUse );

    //Update the domain vertex graph with respect to any permanent polygons added
    void updateDomainRegion();

    //true if there is a need to update the domain graph due to addition/removal of obstacles
    bool domainUpdateRegionExists() const;

    /////////////////////////////////////////////////////

    //Create a visibility graph for the space
    void createVisibilityGraph( PhysConfigSpace2d* pOriginalSpace, PhysCS2dExpansionSpace* pExpansionSpace,
                                MATHEX_SCALAR clearance );

    //Remove the visibility graph
    void deleteVisibilityGraph();
    /////////////////////////////////////////////////////

    //Ensure an expansion space is created with specified clearance.
    //If number of polygons in existing spaces exceeds a threshold value,
    //the existing spaces will be deleted first. This prevents buildup of
    //excessively large expansion spaces
    void expansionSpaceOpen( MATHEX_SCALAR expansionDistance );
    //PRE( not expansionSpaceIsOpen() );

    //Indicate finished using specifed expansion space. However, contents may
    //be retained for future use.
    void expansionSpaceClose( MATHEX_SCALAR expansionDistance );
    //PRE( expansionSpaceIsOpen( expansionDistance ) );

    //True if a space with specified distance is open
    bool expansionSpaceIsOpen( MATHEX_SCALAR expansionDistance ) const;

    //True if a space with any expansion distance is open
    bool expansionSpaceIsOpen() const;

    //The PhysConfigSpace maintained by the expansion space with specified distance
    PhysConfigSpace2d* expansionSpace( MATHEX_SCALAR expansionDistance );
    //PRE( expansionSpaceIsOpen( expansionDistance ) );

    //Add the polygon to the expansion space expanded by the specified clearance
    void expansionSpaceAddPolygon( MATHEX_SCALAR expansionDistance, PolygonId id );
    //PRE( expansionSpaceIsOpen( expansionDistance ) );
    //PRE( not expansionSpacePolygonExists( expansionDistance, id ) );

    //True iff the expansion space contains an expanded version of this space's
    //polygon with id id.
    bool expansionSpacePolygonExists( MATHEX_SCALAR expansionDistance, PolygonId id ) const;
    //PRE( expansionSpaceIsOpen( expansionDistance ) );
    /////////////////////////////////////////////////////

private:
    friend class PhysConfigSpace2d;
    friend class PhysCS2dFindPath;
    friend class PhysCS2dDomainFindPath;
    friend class PhysCS2dVisibilityGraph;
    friend void  PhysDummyFunction2();
    friend ostream& operator <<( ostream& o, const PhysPathFindingQueueWriter& t );

    //ctor.
    PhysCS2dImpl( PhysConfigSpace2d* pConfigSpace,
                  const MexAlignedBox2d& boundary,
                  const PhysConfigSpace2d::Mode& newMode,
                  MATHEX_SCALAR domainMinResolution,
                  size_t nDomainMaxItems,
                  MATHEX_SCALAR domainLipSize,
                  MATHEX_SCALAR polygonMinResolution,
                  size_t nPolygonMaxItems,
                  MATHEX_SCALAR polygonLipSize,
                  MATHEX_SCALAR domainClearance,
                  MATHEX_SCALAR minPortalClearance,
                  MATHEX_SCALAR maxPortalClearance,
                  ObstacleFlags allObstacleFlags );

    //dtor
    ~PhysCS2dImpl();

    // Operation deliberately revoked
    PhysCS2dImpl( const PhysCS2dImpl& );
    PhysCS2dImpl& operator =( const PhysCS2dImpl& );
    bool operator ==( const PhysCS2dImpl& );

    //Private constants etc
    enum
    {
        EXPANSION_SPACE_THRESHOLD = 5 //Polygon count at which expansion space is cleared
                                      //on opening.
    };

    //Adds domain to the collection of domains
    void add( PhysCS2dDomain* domain );

    //typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    //True iff testPoint is contained by one or more of the polygons referenced
    //in polygonTree, returning the id of the first found in pId if so.
    bool contains( const MexPoint2d& testPoint, PhysConfigSpace2d::PolygonId* pId,
                   const PolygonTree& polygonTree, ObstacleFlags flags ) const;

    //True iff the line joining p1 to p2 intersects any of the polygons referenced
    //in polygonTree, in which case the id of the first such polygon is returned in pId.
    //The line is deemed to be at height lineHeight. Polygons with height less than this
    //are not considered to intersect the line.
    //rule determines whether penetration is required to constitute intersection.
    bool intersects( const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR lineHeight,
                     PhysConfigSpace2d::PolygonId* pId,
                     const PolygonTree& polygonTree,
                     Mathex::TouchingRule rule,
                     ObstacleFlags flags ) const;

    //True iff polygon (which has bounding box polygonBoundary) intersects any of
    //the polygons referenced in polygonTree, in which case the ids of the first maxHits such
    //polygons are returned in pIds.
    bool intersects( const MexPolygon2d& polygon, const MexAlignedBox2d& polygonBoundary,
                     PhysConfigSpace2d::PolygonIds* pIds, size_t maxHits,
                     const PolygonTree& polygonTree,
                     Mathex::TouchingRule rule,
                     ObstacleFlags flags ) const;

    //True iff sausage (which has bounding box sausageBoundary) intersects any of
    //the polygons referenced in polygonTree, in which case the ids of the first maxHits such
    //polygons are returned in pIds.
    bool intersects( const MexSausage2d& sausage, const MexAlignedBox2d& sausageBoundary,
                     PhysConfigSpace2d::PolygonIds* pIds, size_t maxHits,
                     const PolygonTree& polygonTree,
                     Mathex::TouchingRule rule,
                     ObstacleFlags flagse ) const;

    //True iff sausage (with specifed boundary) intersects any enabled polygons in polygonTree,
    //which have already been expanded into the open expansion space using given expansion distance.
    bool intersectsAnyExpanded( const MexSausage2d& sausage,
                               const MexAlignedBox2d& sausageBoundary,
                               const PolygonTree& polygonTree,
                               Mathex::TouchingRule rule,
                               ObstacleFlags flags,
                               MATHEX_SCALAR expansionDistance ) const;

    // Return a list of the polygons which intersect the supplied box.
    // This list can be cached and used in some intersects functions
    // if it is necessary to do a lot of intersection tests in the
    // same area ( e.g. when calculating portal points )
    void intersectingPolygons(
        const MexAlignedBox2d& box,
        const PolygonTree& polygonTree,
        PolygonTree::PItems *pPolygons ) const;

    // Return a list of the polygons which intersect the supplied sausage.
    // This list can be cached and used in some intersects functions
    // if it is necessary to do a lot of intersection tests in the
    // same area ( e.g. when calculating portal points )
    void intersectingPolygons(
        const MexSausage2d& sausage,
        const PolygonTree& polygonTree,
        PolygonTree::PItems *pPolygons ) const;

    //True iff circle intersects any of
    //the polygons referenced in polygons, in which case the ids of the first maxHits such
    //polygons are returned in pIds.
    bool intersects(
        const MexCircle2d& circle,
        const PolygonTree::PItems& polygons,
        PhysConfigSpace2d::PolygonIds* pIds,
        size_t maxHits,
        Mathex::TouchingRule rule,
        ObstacleFlags flags ) const;

    //True iff circle (which has bounding box circleBoundary) intersects any of
    //the polygons referenced in polygonTree, in which case the ids of the first maxHits such
    //polygons are returned in pIds.
    bool intersects( const MexCircle2d& circle, const MexAlignedBox2d& circleBoundary,
                     PhysConfigSpace2d::PolygonIds* pIds, size_t maxHits,
                     const PolygonTree& polygonTree,
                     Mathex::TouchingRule rule,
                     ObstacleFlags flags ) const;

    // Add the standard domain vertices to the domain graph for portalId
    // This will also run through all the obstacle flags given
    // in allObstacleFlags and if it can generate different portal
    // points by ignoring different obstacles it will do so.
    void addPortalDomainVertexs( const PortalId& portalId );

    enum CheckAtEnd { CHECK_END, DONT_CHECK_END };

    void calculatePortalPoints( const PortalId& portalId,
      CheckAtEnd checkEnd1, CheckAtEnd checkEnd2,
      bool* pFoundAtEnd1, bool* pFoundAtEnd2,
      ObstacleFlags flags, const PolygonTree::PItems& polygons );

    //  Look for a single portal point between the given points
    //  and ranging between the distances shown. Return the distance
    //  the point is along the line, the clearance that was found,
    //  and also a flag to show if the point was found at the end
    //  of the line and using the maximum clearance allowed. This
    //  flag might save us from having to do further calculations
    //  with the flags set to ignore various obstacles. If we can
    //  find a max. clearance portal point at the end of the line
    //  which doesn't intersect with any obstacles we don't need
    //  to try and find a better one that ignores (say) the water
    //  obstacles.

    bool calculatePortalPoint(
      const MexPoint2d& end1, const MexPoint2d& end2,
      MATHEX_SCALAR startDistance, MATHEX_SCALAR endDistance,
      MATHEX_SCALAR* pPointDistance, MATHEX_SCALAR* pClearance,
      bool* pFoundAtEnd, ObstacleFlags flags,
      const PolygonTree::PItems& polygons );

    void addDomainVertex( const PortalId& portalId, PhysCS2dPortal* pPortal,
      MATHEX_SCALAR distance, MATHEX_SCALAR clearance, ObstacleFlags flags );

    void listArcs( ostream& ostr ) const;

    //Data access methods
    const PhysConfigSpace2d::Mode& mode() const;
    DomainMap& domains();
    DomainTree& domainTree();
    FtlSerialIdGenerator& domainIdGenerator();
    FtlSerialIdGenerator& portalIdGenerator();
    PortalMap& portals();
    PhysCS2dDomainGraph* pDomainGraph();
    FtlReusingIdGenerator& domainVertexIdGenerator();
    FtlReusingIdGenerator& domainArcIdGenerator();
    FtlReusingIdGenerator& polygonIdGenerator();
    MATHEX_SCALAR domainClearance();
    PolygonTree& permanentPolygonTree();
    PolygonTree& temporaryPolygonTree();
    PolygonMap& polygons();
    MexAlignedBox2d& boundary();
    FtlReusingIdGenerator& findPathIdGenerator();
    FtlReusingIdGenerator& domainFindPathIdGenerator();
    FindPathMap& findPaths();
    FindPaths& findPathQueue();
    DomainFindPathMap& domainFindPaths();
    DomainFindPaths& domainFindPathQueue();
    PhysCS2dExpansionSpace& expansionSpace();
    PhysCS2dVisibilityGraph* pVisibilityGraph();
    size_t& nPolygons();
    PhysCS2dDomainFindPath* pActiveDomainFindPath();
    void activeDomainFindPath( PhysCS2dDomainFindPath* pFindPath );
    PhysCS2dFindPath* pActiveFindPath();
    void activeFindPath( PhysCS2dFindPath* pFindPath );
    FtlReusingIdGenerator& motionChunkIdGenerator();
    MotionChunkTree& motionChunkTree();
    MotionChunkMap& motionChunks();

    //Debug tracing
    friend ostream& operator <<( ostream& o, const PhysCS2dImpl& t );
    void trace( ostream& o, const PhysCS2dDomainGraph& domainGraph ) const;

    //Data members
    PhysConfigSpace2d& configSpace_; //The owning space
    MexAlignedBox2d boundary_;
    PhysConfigSpace2d::Mode mode_;
    FtlSerialIdGenerator domainIdGenerator_; //Generates domain ids
    DomainMap domains_; //Collection of domain pointers
    DomainTree domainTree_; //BSP tree of the domains
    FtlSerialIdGenerator portalIdGenerator_; //Generates portal ids
    PortalMap portals_;//Collection of portal pointers
    PhysCS2dDomainGraph domainGraph_; //Graph in which portals are vertices,
                              //and arcs are between points on the portals
    MATHEX_SCALAR domainClearance_; //Clearance distance used for domain pathfinding.
                                    //This is the distance DomainVertexs are located
                                    //in from the sides of Portals.
    FtlReusingIdGenerator polygonIdGenerator_; //Generates polygon ids
    PolygonTree permanentPolygonTree_; //BSP tree of permanent polygons
    PolygonTree temporaryPolygonTree_; //BSP tree of permanent polygons
    PolygonMap polygons_; //id map for all polygons
    size_t nPolygons_; //Number of polygons in the space
    FtlReusingIdGenerator findPathIdGenerator_; //Generates findPath ids
    FindPathMap findPaths_; //The collection of pointers to current PhysCS2dFindPaths
    FtlReusingIdGenerator domainFindPathIdGenerator_; //Generates domain findPath ids
    DomainFindPathMap domainFindPaths_; //The collection of pointers to current PhysCS2dDomainFindPaths
    size_t nDomainGraphClients_; //Count of clients that have registered using the domain
                                 //graph for an extended time, eg for a sliced algorithm
    PhysCS2dVisibilityGraph* pVisibilityGraph_;//The visibility graph if being maintained
    PhysCS2dExpansionSpace* pExpansionSpace_; //Only 1 maintained for now
    PhysCS2dDomainFindPath* pActiveDomainFindPath_;//Pointer to current findpath searching the
                                            //domain graph
    PhysCS2dFindPath* pActiveFindPath_;//Pointer to current findpath using the
                                       //expansion space and visibility graph
    FtlReusingIdGenerator motionChunkIdGenerator_; //Generates motion chunk ids
    MotionChunkTree motionChunkTree_; //BSP tree of motion chunks
    MotionChunkMap motionChunks_; //id map for all motion chunks
    FindPaths findPathQueue_;// Vector of the current findpaths.
    DomainFindPaths domainFindPathQueue_;// Vector of the current findpaths.
    bool domainRegionOutOfDate_; //True if any permanent polygons have been added to the space,
                            //since the domain vertex graph was last updated. If so
                            //domainUpdateRegion is the area to be updated
    MexAlignedBox2d domainUpdateRegion_;

    MATHEX_SCALAR   minPortalClearance_;
    MATHEX_SCALAR   maxPortalClearance_;

    //  The collection of all possible object flags
    const ObstacleFlags allFlags_;

    //  These allow us to keep track of the highest priority domain path finds
    bool                    hasCurrentDomainFindPath_;
    PhysCS2dDomainFindPath* pCurrentDomainFindPath_;

    //  These allow us to keep track of the highest priority path finds
    bool                hasCurrentFindPath_;
    PhysCS2dFindPath*   pCurrentFindPath_;

    bool bufferPolygonPortalUpdates_; //When false, portals will be updated wrt to obstacles after
                                      //every polygon is added/removed.
                                      //When true they'll only be updated when a pathfind is done.
};

#ifdef _INLINE
    #include "phys/internal/cs2impl.ipp"
#endif


#endif

/* End CS2DATA.HPP **************************************************/
