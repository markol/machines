/*
 * C S 2 D A T A . I P P 
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
const PhysConfigSpace2d::Mode& PhysCS2dImpl::mode( void ) const
{
    return mode_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlSerialPMap< PhysCS2dDomain >& PhysCS2dImpl::domains( void )
{
    return domains_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::DomainTree& PhysCS2dImpl::domainTree( void )
{
    return domainTree_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlSerialIdGenerator& PhysCS2dImpl::portalIdGenerator( void )
{
    return portalIdGenerator_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlSerialIdGenerator& PhysCS2dImpl::domainIdGenerator( void )
{
    return domainIdGenerator_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::domainVertexIdGenerator( void )
{
    return domainGraph_.domainVertexIdGenerator();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::domainArcIdGenerator( void )
{
    return domainGraph_.domainArcIdGenerator();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlSerialPMap< PhysCS2dPortal >& PhysCS2dImpl::portals( void )
{
    return portals_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainGraph* PhysCS2dImpl::pDomainGraph( void )
{
    return &domainGraph_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR PhysCS2dImpl::domainClearance( void )
{
    return domainClearance_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::polygonIdGenerator( void )
{
    return polygonIdGenerator_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::PolygonTree& PhysCS2dImpl::permanentPolygonTree( void )
{
    return permanentPolygonTree_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::PolygonTree& PhysCS2dImpl::temporaryPolygonTree( void )
{
    return temporaryPolygonTree_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::PolygonMap& PhysCS2dImpl::polygons( void )
{
    return polygons_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MexAlignedBox2d& PhysCS2dImpl::boundary( void )
{
    return boundary_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::FindPathMap& PhysCS2dImpl::findPaths( void )
{
    return findPaths_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl::DomainFindPathMap& PhysCS2dImpl::domainFindPaths( void )
{
    return domainFindPaths_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::findPathIdGenerator( void )
{
    return findPathIdGenerator_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::domainFindPathIdGenerator( void )
{
    return domainFindPathIdGenerator_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysCS2dDomainGraph& PhysCS2dImpl::domainGraph( void ) const
{
    return domainGraph_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool PhysCS2dImpl::domainGraphInUse( void ) const
{
    return nDomainGraphClients_ != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysCS2dImpl::domainGraphInUse( bool inUse )
{
    if( inUse )
        ++nDomainGraphClients_;
    else
    {
        PRE( nDomainGraphClients_ != 0 );
        --nDomainGraphClients_;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dVisibilityGraph* PhysCS2dImpl::pVisibilityGraph( void )
{
    return pVisibilityGraph_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t& PhysCS2dImpl::nPolygons( void )
{
    return nPolygons_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainFindPath* PhysCS2dImpl::pActiveDomainFindPath( void )
{
    return pActiveDomainFindPath_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dFindPath* PhysCS2dImpl::pActiveFindPath( void )
{
    return pActiveFindPath_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysCS2dImpl::activeFindPath( PhysCS2dFindPath* pFindPath )
{
    PRE( pFindPath == NULL or pActiveFindPath_ == NULL );
    pActiveFindPath_ = pFindPath;
}
//////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
FtlReusingIdGenerator& PhysCS2dImpl::motionChunkIdGenerator()
{
    return motionChunkIdGenerator_;
}

_CODE_INLINE
PhysCS2dImpl::MotionChunkTree& PhysCS2dImpl::motionChunkTree()
{
    return motionChunkTree_;
}

_CODE_INLINE
PhysCS2dImpl::MotionChunkMap& PhysCS2dImpl::motionChunks()
{
    return motionChunks_;
}

_CODE_INLINE
PhysCS2dImpl::FindPaths& PhysCS2dImpl::findPathQueue()
{
    return findPathQueue_;
}

_CODE_INLINE
PhysCS2dImpl::DomainFindPaths& PhysCS2dImpl::domainFindPathQueue()
{
    return domainFindPathQueue_;
}

_CODE_INLINE
PhysCS2dExpansionSpace& PhysCS2dImpl::expansionSpace()
{
    return *pExpansionSpace_;
}
/* End CS2DATA.IPP **************************************************/
