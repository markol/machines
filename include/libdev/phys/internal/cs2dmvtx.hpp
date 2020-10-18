/*
 * C S 2 D M V T X . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dDomainVertex

    Represents a vertex on a portal between 2 domains in a PhysConfigSpace2d.
    Forms vertices of the domain graph for gross navigation between domains.
*/

#ifndef _PHYS_CS2DMVTX_HPP
#define _PHYS_CS2DMVTX_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"
#include "phys/cspace2.hpp"

//Memberwise canonical
class PhysCS2dDomainVertex
{
public:
    //ctor.
    PhysCS2dDomainVertex( void ); //Default for ctl
    PhysCS2dDomainVertex( const MexPoint2d& point, MATHEX_SCALAR clearance,
      PhysConfigSpace2d::ObstacleFlags flags,
      PhysConfigSpace2d::PortalId portalId = PhysConfigSpace2d::PortalId::invalidId() );

    //dtor.
    ~PhysCS2dDomainVertex( void );

    //Access
    const PhysConfigSpace2d::PortalId& portalId( void ) const;
    const MexPoint2d& point( void ) const;
    MATHEX_SCALAR   clearance() const;
    PhysConfigSpace2d::ObstacleFlags    flags() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dDomainVertex& t );
    friend bool operator== ( const PhysCS2dDomainVertex& lhs, const PhysCS2dDomainVertex& rhs );
    friend bool operator< ( const PhysCS2dDomainVertex& lhs, const PhysCS2dDomainVertex& rhs );

private:
    //data members
    PhysConfigSpace2d::PortalId         portalId_; //Id of the portal on which the vertex sits, if any
    MexPoint2d                          point_; //The point coordinates
    MATHEX_SCALAR                       clearance_;
    PhysConfigSpace2d::ObstacleFlags    flags_;
};

#ifdef _INLINE
    #include "internal/cs2dmvtx.ipp"
#endif


#endif

/* End CS2DMVTX.HPP *************************************************/
