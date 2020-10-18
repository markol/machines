/*
 * C S 2 V I G R A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dVisibilityGraph

    Implements a visibility graph for a PhysConfigSpace2d.
    This is a private implementation class of PhysConfigSpace2d.
    The graph has vertices at the vertices of all polygons in the space.
    Arcs are created between all vertices which do not intersect polygons in the
    space. The A* findpath algorithm maybe used to find the shortest route through
    the graph between 2 vertices.
*/

#ifndef _PHYS_CS2VIGRA_HPP
#define _PHYS_CS2VIGRA_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/point2d.hpp"
#include "ftl/serialid.hpp"
#include "ftl/seriamap.hpp"
#include "graph/vertex.hpp"
#include "graph/arc.hpp"
#include "graph/graph.hpp"
#include "graph/astarvtx.hpp"
#include "graph/astaralg.hpp"

#include "phys/cspace2.hpp"

//Forward declarations
class MexPoint2d;
class PhysCS2dExpansionSpace;

//orthodox canonical (revoked)
class PhysCS2dVisibilityGraph
{
public:
    //Useful types
    typedef PhysConfigSpace2d::PolygonId PolygonId;
    typedef PhysConfigSpace2d::Path Path;
    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    //ctor. Defined for configSpace containing the expanded versions of the polygons in
    //pOriginalConfigSpace, with pExpansionSpace holding the mapping between the 2.
    //Path to be found for obejct with clearance radius.
    PhysCS2dVisibilityGraph( PhysConfigSpace2d* pConfigSpace, PhysConfigSpace2d* pOriginalConfigSpace,
                             PhysCS2dExpansionSpace* pExpansionSpace, MATHEX_SCALAR clearance );

    //dtor.
    ~PhysCS2dVisibilityGraph( void );

    /////////////////////////////////////////////////////////
    //Add the config space's polygon to the graph
    void addPolygon( const PolygonId& polygonId );

    //Remove the config space's polygon from the graph
    void removePolygon( const PolygonId& polygonId );

    //True if graph is constructed for all registered polygons
    bool isUpToDate( void ) const;

    //Perform some updating of the visibility graph, without taking too long.
    //True iff becomes up to date.
    //Take no longer than maxTime.
    bool updateGraph( const PhysRelativeTime& maxTime, ObstacleFlags flags );

    /////////////////////////////////////////////////////////

    //Set the current findPath search to be from startPoint to endPoint.
    //These points are added temporarily as vertices to the visibility graph,
    //and will be removed from the graph only by clearFindPath() or a new
    //call to setFindPath() using different points.
    void setFindPath( const MexPoint2d& startPoint, const MexPoint2d& endPoint );

    //Update the findPath search, returning true iff complete.
    //Take no longer than maxTime.
    bool updateFindPath( const PhysRelativeTime& maxTime, ObstacleFlags flags );

    //True iff the current findPath search is complete
    bool isFindPathFinished( void ) const;

    //Returns the results of the current findPath search. If result is empty,
    //no path could be found. Otherwise the first and last points returned will
    //be the start and end points.
    Path findPath( void ) const;
    //PRE( isFindPathFinished() )

    //Clears the current findPath search, and removes the start and end points from
    //the graph.
    void clearFindPath( void );

    /////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dVisibilityGraph& t );

private:
    // Operations deliberately revoked
    PhysCS2dVisibilityGraph( const PhysCS2dVisibilityGraph& );
    PhysCS2dVisibilityGraph& operator =( const PhysCS2dVisibilityGraph& );
    bool operator ==( const PhysCS2dVisibilityGraph& );

    //Vertex class
    class VertexData
    {
    public:
        VertexData();
        VertexData( const MexPoint2d& point, const PolygonId& polygonId );
        friend bool operator== ( const VertexData& lhs, const VertexData& rhs );
        friend bool operator< ( const VertexData& lhs, const VertexData& rhs );

        const MexPoint2d* pPoint_; //The vertex point
        PolygonId polygonId_; //Id of the polygon on which vertex lies.
                              //Maybe invalid for the start/end points for findpath
        bool startArcChecked_; //true if the arc to findpath start has been checked and added if ok
        bool endArcChecked_; //true if the arc to findpath end has been checked and added if ok

        friend ostream& operator<<( ostream&, const VertexData& );
    };

    friend ostream& operator<<( ostream&, const VertexData& );
    friend bool operator== ( const VertexData& lhs, const VertexData& rhs );
    friend bool operator< ( const VertexData& lhs, const VertexData& rhs );

    //Arc class
    class ArcData
    {
    public:
        friend bool operator== ( const ArcData& lhs, const ArcData& rhs );
        friend bool operator< ( const ArcData& lhs, const ArcData& rhs );
    };

    friend bool operator== ( const ArcData& lhs, const ArcData& rhs );
    friend bool operator< ( const ArcData& lhs, const ArcData& rhs );

    //Types needed for the graph class
    typedef FtlSerialId VertexId;
    typedef FtlSerialId ArcId;

    typedef GraVertex< VertexData, ArcId > GraphVertex;
    typedef FtlSerialMap< GraphVertex > VertexMap;

    typedef GraArc< ArcData, VertexId, MATHEX_SCALAR > GraphArc;
    typedef FtlSerialMap< GraphArc > ArcMap;

    typedef GraGraph< VertexMap, ArcMap > Graph;

    //Stuff for the A* algorithm on the visibility graph
    typedef GraAStarVertex< Graph > AStarVertex;
    typedef FtlSerialMap< AStarVertex > AStarVertexMap;

    class FindPathAlg : public GraAStarAlg< Graph, AStarVertexMap >
    {
        virtual MATHEX_SCALAR estimatedCost( const VertexData& from,
                                             const VertexData& to ) const;
        virtual bool vertexAvailable( const VertexData& vertex ) const;
    };

    //Other private types
    typedef ctl_vector< PolygonId > PolygonIds;
    typedef ctl_vector< VertexId > VertexIds;

    enum ProcessingState { NONE, POLYGON, START_VERTEX, END_VERTEX };

    //Select a new polygon to process, if any.
    void startProcessingPolygon( ObstacleFlags flags );

    //Process some intersections between the polygon currently being processed
    //and existing arcs in the graph. Remove any that intersect the polygon.
    //True iff at least one arc actually tested for intersection.
    bool processOldArcs( void );

    //Check current vertex of the processing polygon against an existing graph vertex
    //and add an arc between them if no intersections with the config space polygons.
    //True if some significant processing performed.
    bool processNewVertexWithOldVertex( ObstacleFlags flags );

    //Add an arc for the current processing vertex of the processing polygon to its
    //adjacent preceding point, unless that edge intersects some other polygon in the
    //config space
    //True if some significant processing performed.
    bool processNewVertexWithNewVertex( ObstacleFlags flags );

    //Complete processing of the current polygon
    void endProcessingPolygon( void );

    //Set up the processing for the start/end vertex, depending on processingState_;
    void startProcessingFindPathVertex( void );

    //Check current processing findPath vertex against an existing graph vertex
    //and add an arc between them if no intersections with the config space polygons.
    //True if some significant processing performed.
    bool processFindPathVertexWithOldVertex( ObstacleFlags flags );

    //Run through all thr graph vertices setting the start/end arc checked flags to false
    //if resetStart/EndArcFlags are true.
    void updateStartEndArcFlags( bool resetStartArcFlag, bool resetEndArcFlag );

    //Invalidates any current findPath search or stored results, and deletes items
    //no longer useful.
    void invalidateFindPath( void );

    bool originalSpaceContainsSausage
    (
        const MexPoint2d& startPoint, const MexPoint2d& endPoint, ObstacleFlags flags
    );

    //  Returns a vertex id that will never be assigned to any valid vertex
    static VertexId dummyVertexId();

    //Data members
    PhysConfigSpace2d* pConfigSpace_; //The config space for which the graph is defined (ie contains expanded polygons)
    PhysConfigSpace2d* pOriginalConfigSpace_; //the original space containing the unexpanded polygons
    PhysCS2dExpansionSpace* pExpansionSpace_; //the link between the 2 spaces
    MATHEX_SCALAR clearance_; //The clearance to be used
    Graph graph_; //The visibility graph
    PolygonIds unprocessedPolygons_;//Ids of polygons waiting to be added to graph
    ProcessingState processingState_; //Defines what object currently being processed -
                                      //ie having vertices/arcs added to graph
    PolygonId processingPolygonId_; //Id of polygon currently being processed.
    VertexIds processingPolygonVertexIds_; //Ids generated for the vertices of the polygon being
                                           //added to the graph
    size_t processingPolygonVertex_;//Index of the vertex being processed
    VertexId processingGraphVertexId_; //Id of the graph vertex being processed, either for
                                       //adding a polygon, or during processing a search vertex
    ArcId processingGraphArcId_; //Id of graph arc processing when processingOldArcs_ is true
    FtlSerialIdGenerator vertexIdGenerator_; // Generates graph vertex ids
    FtlSerialIdGenerator arcIdGenerator_; // Generates graph arc ids
    VertexId startVertexId_; //Start vertex for the findPath search.
                             //Valid iff findPathIsDefined_
    VertexId endVertexId_; //End vertex for the findPath search.
                           //Valid iff findPathIsDefined_
    MexPoint2d endPoints_[2]; //Start/End point coords for findPath search
    size_t startPointIndex_; //Index in endPoints_ of findPath start point.
                             //T'other one is the endpoint
    FindPathAlg* pFindPathAlg_; // Pointer to current instance of the A* algorithm for
                                //finding the path from the start vertex to the end vertex.
    VertexIds findPathOutput_; //Used to cache the results of running the findPath algorithm
                               //on the visibility graph
    bool upToDate_ :1; //True iff graph is constructed for all registered polygons
    bool processingOldArcs_ :1; //True iff current polygon is initial phase of checking
                                //for intersection with existing arcs
    bool findPathIsDefined_ :1; //true if a findPath search has been registered
    bool startVertexProcessed_ :1; //True iff the start vertex has been connected to all other
                                   //potential vertices
    bool endVertexProcessed_ :1; //True iff the end vertex has been connected to all other
                                 //potential vertices
    bool findPathCompleted_ :1; //True iff the current find path search has completed.
                                //Results will be in findPathOutput_.
};

#ifdef _INLINE
    #include "internal/cs2vigra.ipp"
#endif


#endif

/* End CS2VIGRA.HPP *************************************************/
