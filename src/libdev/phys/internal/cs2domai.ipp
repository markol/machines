/*
 * C S 2 D O M A I . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include <algorithm>
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysCS2dDomain::boundary( MexAlignedBox2d* pBox ) const
{
    *pBox = boundary_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysCS2dDomain::addPortal( const PhysConfigSpace2d::PortalId& id )
{
    PRE( not containsPortal( id ) );
    portals_.push_back( id );
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysCS2dDomain::removePortal( const PhysConfigSpace2d::PortalId& id )
{
    PRE( containsPortal( id ) );

    //Find and erase the item from the list
    ctl_vector< PhysConfigSpace2d::PortalId >::iterator i =
        find( portals_.begin(), portals_.end(), id );
    POST( i != portals_.end() );

    portals_.erase( i );
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool PhysCS2dDomain::containsPortal( const PhysConfigSpace2d::PortalId& id ) const
{
    return find( portals_.begin(), portals_.end(), id ) != portals_.end();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t PhysCS2dDomain::nPortals( void ) const
{
    return portals_.size();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const ctl_vector< PhysConfigSpace2d::PortalId >& PhysCS2dDomain::portals( void ) const
{
    return portals_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysConfigSpace2d::DomainId& PhysCS2dDomain::id( void ) const
{
    return id_;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DOMAI.IPP *************************************************/
