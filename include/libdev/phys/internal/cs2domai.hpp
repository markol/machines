/*
 * C S 2 D O M A I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dDomain

    Represents a Domain added to a PhysConfigSpace2d.
*/

#ifndef _PHYS_CS2DOMAI_HPP
#define _PHYS_CS2DOMAI_HPP

#include "base/base.hpp"
#include "stdlib/memory.hpp"
#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"

//Forward refs
class MexPolygon2d;
class PhysCS2dDomainGraph;

//orthodox canonical( revoked )
class PhysCS2dDomain
{
public:
    typedef PhysConfigSpace2d::DomainId DomainId;

    //ctors.
    PhysCS2dDomain( const DomainId& id, const MexAlignedBox2d& boundary );

    PhysCS2dDomain( const DomainId& id, const MexAlignedBox2d& boundary,
                    std::auto_ptr< MexPolygon2d >& polygonAPtr );

    //dtor.
    ~PhysCS2dDomain( void );

    //Returns aligned bounding box in pBox
    void boundary( MexAlignedBox2d* pBox ) const;

    //The domain's id
    const PhysConfigSpace2d::DomainId& id( void ) const;

    //////////////////////////////
    //Maintain portal list

    void addPortal( const PhysConfigSpace2d::PortalId& id );
    //PRE( not containsPortal( id ) )

    void removePortal( const PhysConfigSpace2d::PortalId& id );
    //PRE( containsPortal( id ) )

    bool containsPortal( const PhysConfigSpace2d::PortalId& id ) const;

    size_t nPortals( void ) const;

    const ctl_vector< PhysConfigSpace2d::PortalId >& portals( void ) const;

    //////////////////////////////

    //True if contains point
    bool contains( const MexPoint2d& point ) const;

    //////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dDomain& t );

private:
    // Operation deliberately revoked
    PhysCS2dDomain( const PhysCS2dDomain& );
    PhysCS2dDomain& operator =( const PhysCS2dDomain& );
    bool operator ==( const PhysCS2dDomain& );

    //Data members
    DomainId id_; //This domain's id
    MexAlignedBox2d boundary_; //Aligned bounding box
    MexPolygon2d* pPolygon_; //Polygon defining actual boundary if different from boundary_
    ctl_vector< PhysConfigSpace2d::PortalId > portals_; //Collection of portal ids
};

#ifdef _INLINE
    #include "internal/cs2domai.ipp"
#endif


#endif

/* End CS2DOMAI.HPP *************************************************/
