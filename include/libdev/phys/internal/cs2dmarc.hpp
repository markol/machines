/*
 * C S 2 D M A R C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dDomainArc

    Represents an arc item in the domain/portal graph within the
    PhysConfigSpace2d class.
*/

#ifndef _PHYS_CS2DMARC_HPP
#define _PHYS_CS2DMARC_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

#include "phys/cspace2.hpp"

//memberwise canonical
class PhysCS2dDomainArc
{
public:
    //ctor
    PhysCS2dDomainArc( void );
    PhysCS2dDomainArc( PhysConfigSpace2d::DomainId domainId );

    //dtor
    ~PhysCS2dDomainArc( void );

    //Data access
    const PhysConfigSpace2d::DomainId& domainId( void ) const;

    friend bool operator== ( const PhysCS2dDomainArc& lhs, const PhysCS2dDomainArc& rhs );
    friend bool operator< ( const PhysCS2dDomainArc& lhs, const PhysCS2dDomainArc& rhs );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dDomainArc& t );

private:
    //Data members
    PhysConfigSpace2d::DomainId domainId_; //The domain containing the arc
};

#ifdef _INLINE
    #include "internal/cs2dmarc.ipp"
#endif


#endif

/* End CS2DMARC.HPP *************************************************/
