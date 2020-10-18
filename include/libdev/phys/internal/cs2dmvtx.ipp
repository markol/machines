/*
 * C S 2 D M V T X . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainVertex::PhysCS2dDomainVertex( void )
:   portalId_( PhysConfigSpace2d::PortalId::invalidId() )
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainVertex::PhysCS2dDomainVertex( const MexPoint2d& point,
  MATHEX_SCALAR clearance, PhysConfigSpace2d::ObstacleFlags flags,
  PhysConfigSpace2d::PortalId portalId )
:   point_( point ),
    portalId_( portalId ),
    clearance_( clearance ),
    flags_( flags )
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainVertex::~PhysCS2dDomainVertex()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool operator== ( const PhysCS2dDomainVertex& lhs, const PhysCS2dDomainVertex& rhs )
{
    return lhs.portalId_ == rhs.portalId_ and
           lhs.point_ == rhs.point_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool operator< ( const PhysCS2dDomainVertex& lhs, const PhysCS2dDomainVertex& rhs )
{
    return lhs.portalId_ < rhs.portalId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysConfigSpace2d::PortalId& PhysCS2dDomainVertex::portalId( void ) const
{
    return portalId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MexPoint2d& PhysCS2dDomainVertex::point( void ) const
{
    return point_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR   PhysCS2dDomainVertex::clearance() const
{
    return clearance_;
}

_CODE_INLINE
PhysConfigSpace2d::ObstacleFlags    PhysCS2dDomainVertex::flags() const
{
    return flags_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DMVTX.IPP *************************************************/
