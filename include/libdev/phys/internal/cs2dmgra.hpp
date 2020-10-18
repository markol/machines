/*
 * C S 2 D M G R A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dDomainGraph

    The graph used to perform gross pathfinding via domains in a PhysConfigSpace2d.
    The nodes are PhysCS2dDomainVertexs, with arcs PhysCS2dDomainArc.
*/

#ifndef _PHYS_CS2DMGRA_HPP
#define _PHYS_CS2DMGRA_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "ftl/seriamap.hpp"
#include "ftl/reuidgen.hpp"
#include "graph/graph.hpp"
#include "graph/vertex.hpp"
#include "graph/arc.hpp"

#include "phys/cspace2.hpp"
#include "phys/internal/cs2dmarc.hpp"
#include "phys/internal/cs2dmvtx.hpp"

//Vertex/arc typedefs
typedef GraVertex< PhysCS2dDomainVertex, PhysConfigSpace2d::DomainArcId
                 > PhysCS2dDomainGraphVertex;
typedef FtlSerialMap< PhysCS2dDomainGraphVertex > PhysCS2dDomainVertexMap;

typedef GraArc< PhysCS2dDomainArc, PhysConfigSpace2d::DomainVertexId, MATHEX_SCALAR
              > PhysCS2dDomainGraphArc;
typedef FtlSerialMap< PhysCS2dDomainGraphArc > PhysCS2dDomainArcMap;

//memberwise canonical
class PhysCS2dDomainGraph : public GraGraph< PhysCS2dDomainVertexMap, PhysCS2dDomainArcMap >
{
public:
    //PhysCS2dDomainGraph( void );
    //~PhysCS2dDomainGraph( void );

    FtlReusingIdGenerator& domainVertexIdGenerator()
    {
        return domainVertexIdGenerator_;
    };

    FtlReusingIdGenerator& domainArcIdGenerator()
    {
        return domainArcIdGenerator_;
    };

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dDomainGraph& t );

protected:
    //Callbacks when vertex/arc deleted. Used to free the id
	virtual void doRemoveVertex( const VertexId& id );
	virtual void doRemoveArc( const ArcId& id );

private:
    // Operation deliberately revoked
    //PhysCS2dDomainGraph( const PhysCS2dDomainGraph& );
    //PhysCS2dDomainGraph& operator =( const PhysCS2dDomainGraph& );
    //bool operator ==( const PhysCS2dDomainGraph& );

    //data members
    FtlReusingIdGenerator domainVertexIdGenerator_; //Generates domainVertex ids
    FtlReusingIdGenerator domainArcIdGenerator_; //Generates domainArc ids
};


#endif

/* End CS2DMGRA.HPP *************************************************/
