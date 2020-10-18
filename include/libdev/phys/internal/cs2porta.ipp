/*
 * C S 2 P O R T A . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#include "ctl/algorith.hpp"

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysConfigSpace2d::DomainId PhysCS2dPortal::domainId1( void ) const
{
    return domainId1_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysConfigSpace2d::DomainId PhysCS2dPortal::domainId2( void ) const
{
    return domainId2_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MexPoint2d& PhysCS2dPortal::endPoint1( void ) const
{
    return endPoint1_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MexPoint2d& PhysCS2dPortal::endPoint2( void ) const
{
    return endPoint2_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR PhysCS2dPortal::length( void ) const
{
    return length_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t PhysCS2dPortal::nDomainVertexs( void ) const
{
    return domainVertexIds_.size();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysConfigSpace2d::DomainVertexId& PhysCS2dPortal::domainVertexId( size_t i ) const
{
    PRE_INFO( i );
    PRE_INFO( nDomainVertexs() );
    PRE( i < nDomainVertexs() );

    return domainVertexIds_[ i ];
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool PhysCS2dPortal::containsDomainVertex
(
    const PhysConfigSpace2d::DomainVertexId& domainVertexId
) const
{
    return ctl_contains( &domainVertexIds_, domainVertexId );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2PORTA.IPP *************************************************/
