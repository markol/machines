/*
 * L I N K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/link.hpp"
#include "world4d/composit.hpp"

#ifndef _INLINE
#include "world4d/link.ipp"
#endif


PER_DEFINE_PERSISTENT( W4dLink );

W4dLink::W4dLink( W4dComposite* pOwningComposite, const W4dTransform3d& newLocalTransform,
                  Solidity solid, const W4dLink::Id& id )
: W4dEntity( pOwningComposite, newLocalTransform, solid, DONT_ADD_TO_CHILD_LIST ),
  pComposite_( pOwningComposite ),
  id_( id )
{
    TEST_INVARIANT;
}

W4dLink::W4dLink( W4dLink* pOwningLink, const W4dTransform3d& newLocalTransform,
                  Solidity solid, const W4dLink::Id& id )
: W4dEntity( pOwningLink, newLocalTransform, solid ),
  pComposite_( pOwningLink->pComposite() ),
  id_( id )
{
    TEST_INVARIANT;
}

W4dLink::W4dLink(
    const W4dLink& copyMe,
    W4dComposite* pComposite,
    W4dEntity* pOwner,
    const W4dTransform3d& localTransform,
    const W4dLink::Id& id )
: W4dEntity( copyMe, pOwner, localTransform ),
  pComposite_( pComposite ),
  id_( id )
{
}

W4dLink::~W4dLink()
{
    TEST_INVARIANT;

}

// virtual
void W4dLink::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
    ostr << "Start W4dLink " << (void*)this << std::endl;
    ostr << "Name " << name() << std::endl;
    ostr << "Id " << id() << std::endl;
    ostr << "End   W4dLink " << (void*)this << std::endl;
}

void W4dLink::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool W4dLink::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                          Accuracy accuracy ) const
{
    //Just check against bounding volume
    return defaultIntersectsLine( line, pDistance, accuracy );
}

void perWrite( PerOstream& ostr, const W4dLink& link )
{
    const W4dEntity& base = link;

    ostr << base;
    ostr << link.pComposite_;
    ostr << link.id_;
}

void perRead( PerIstream& istr, W4dLink& link )
{
    W4dEntity& base = link;

    istr >> base;
    istr >> link.pComposite_;
    istr >> link.id_;
}

W4dLink::W4dLink( PerConstructor con )
: W4dEntity( con )
{
}


/* End LINK.CPP *****************************************************/
