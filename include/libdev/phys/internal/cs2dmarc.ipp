/*
 * C S 2 D M A R C . I P P 
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
PhysCS2dDomainArc::PhysCS2dDomainArc( void )
:   domainId_( 0 )
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainArc::PhysCS2dDomainArc( PhysConfigSpace2d::DomainId domainId )
:   domainId_( domainId )
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainArc::~PhysCS2dDomainArc()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool operator== ( const PhysCS2dDomainArc& lhs, const PhysCS2dDomainArc& rhs )
{
    return lhs.domainId_ == rhs.domainId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool operator< ( const PhysCS2dDomainArc& lhs, const PhysCS2dDomainArc& rhs )
{
    return lhs.domainId_ < rhs.domainId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysConfigSpace2d::DomainId& PhysCS2dDomainArc::domainId( void ) const
{
    return domainId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DMARC.IPP *************************************************/
