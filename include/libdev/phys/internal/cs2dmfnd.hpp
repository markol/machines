/*
 * C S 2 D M F N D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dDomainFindPath

    Implements the domain path finding algorithm in a PhysConfigSpace2d class.
    This finds a conceptual route between a start point and an end point via
    zero or more portals connecting domains.
    This is an implementation class for PhysConfigSpace2d.
*/

#ifndef _PHYS_CS2DMFND_HPP
#define _PHYS_CS2DMFND_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"
#include "graph/astarvtx.hpp"
#include "graph/astaralg.hpp"

#include "phys/cspace2.hpp"
#include "phys/internal/cs2dmgra.hpp"
#include "phys/internal/cs2dmvtx.hpp"

//Forward declarations

//Orthodox canonical (revoked)
class PhysCS2dDomainFindPath
{
public:
    //Default ctor for collections etc.
    PhysCS2dDomainFindPath();

    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    //ctor. Finds a path contained in pSpace from startPoint to endPoint
    //flags indicates those obstacles which can be ignored
    PhysCS2dDomainFindPath( PhysConfigSpace2d* pSpace, const MexPoint2d& startPoint,
                      const MexPoint2d& endPoint, MATHEX_SCALAR clearance,
                      ObstacleFlags flags,
                      PhysPathFindingPriority );

    //dtor
    ~PhysCS2dDomainFindPath();

    //Useful types
    typedef PhysConfigSpace2d::Path Path;
    typedef PhysConfigSpace2d::PortalId PortalId;
    typedef PhysConfigSpace2d::PortalPoint PortalPoint;
    typedef PhysConfigSpace2d::PortalPoints PortalPoints;

    //True if the algorithm is finished
    bool isFinished() const;

    //Run the algorithm (for no longer than maxTime)
    void update( const PhysRelativeTime& maxTime );
    //PRE( state_ != UNDEFINED )

    //True if a path was found. In this case, the portal points are returned in
    //pPortalPoints. If the start and end point both lie in the same domain,
    //the path will be empty, as it does not include the start and end points.
    bool output( PortalPoints* pPortalPoints ) const;
    //PRE( state_ == FINISHED )

    MATHEX_SCALAR clearance() const;

    ObstacleFlags   flags() const;

    PhysPathFindingPriority priority() const;
    void increasePriority();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dDomainFindPath& t );

private:
    // Operations deliberately revoked
    PhysCS2dDomainFindPath( const PhysCS2dDomainFindPath& );
    PhysCS2dDomainFindPath& operator =( const PhysCS2dDomainFindPath& );
    bool operator ==( const PhysCS2dDomainFindPath& );

    //Private types etc
    enum State
    {
        UNDEFINED, //No search data
        NOT_STARTED, //Haven't got going yet
        PENDING_MACRO, //Waiting for the domain graph to be free
        MACRO, //Doing macro search in the domain graph
        FINISHED
    };

    //Stuff for the A* algorithm on the domain graph
    typedef GraAStarVertex< PhysCS2dDomainGraph > AStarDomainVertex;
    typedef FtlSerialMap< AStarDomainVertex > AStarDomainVertexMap;

    class DomainAStarAlg : public GraAStarAlg< PhysCS2dDomainGraph, AStarDomainVertexMap >
    {
    public:
        //  Specify the minimum clearance a vertex must have in order to be available
        //  Also specify the obstacle flags this machine can move over
        DomainAStarAlg( MATHEX_SCALAR minClearance, ObstacleFlags flags );
    private:
        virtual MATHEX_SCALAR estimatedCost( const PhysCS2dDomainVertex& from,
                                             const PhysCS2dDomainVertex& to ) const;
        virtual bool vertexAvailable( const PhysCS2dDomainVertex& vertex ) const;

        MATHEX_SCALAR minClearance_;
        ObstacleFlags flags_;
    };

    //Updates at various stages
    void start();

    //////////////////////////////////////////////////
    //True if start and end points in different domains.
    //If so, need to do a macro search using the domain graph.
    bool needMacroSearch();

    //If the domain graph is available, lock it. Then add a domain vertex
    //for the start and endpoint to the domain graph, with arcs joining each
    //existing domain vertex in the respective domains. Set up the A* search
    //algorithm on the graph. Enter MACRO state.
    //If the domain graph is not avaliable, do nothing.
    void startMacroSearch();
    //PRE( state_ == PENDING_MACRO )

    //Advance the macro search algorithm (for no longer than maxTime)
    void updateMacroSearch( const PhysRelativeTime& maxTime );
    //PRE( state_ == MACRO )

    //True iff the domain graph search has completed.
    bool isMacroSearchFinished() const;
    //PRE( state_ == MACRO )

    //Get the results of the domain graph pathFind, and tidy up
    void endMacroSearch();
    //////////////////////////////////////////////////

    //Data members
    State state_; // Indicates where at
    MexPoint2d startPoint_;// Start position
    MexPoint2d endPoint_;// End position
    PhysConfigSpace2d* pConfigSpace_;//The config space through which we try to find the path
    PortalPoints domainPath_; // Path for navigating between domains
    PhysCS2dDomainGraph::Version domainGraphVersion_; // Version of domain graph used as basis
                                                      // for search
    PhysConfigSpace2d::DomainId startDomainId_; //Id of domain containing start point
    PhysConfigSpace2d::DomainId endDomainId_; //Id of domain containing end point
    PhysConfigSpace2d::DomainVertexId startDomainVertexId_; //Vertex add to domain graph for
                                                            //start point
    PhysConfigSpace2d::DomainVertexId endDomainVertexId_; //Vertex add to domain graph for
                                                          //end point
    DomainAStarAlg* pDomainAlg_; //The domain A* algorithm, if running
    bool foundPath_; //True if succeeded in finding a (possibly empty) path
    MATHEX_SCALAR   clearance_;
    ObstacleFlags   flags_;
    PhysPathFindingPriority priority_;
};

#ifdef _INLINE
    #include "internal/cs2dmfnd.ipp"
#endif


#endif

/* End CS2DMFND.HPP *************************************************/
