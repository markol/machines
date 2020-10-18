/*
 * H I E R B U I L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <string>
#include "render/hierbuil.hpp"

RenHierarchyBuilder::RenHierarchyBuilder()
: pRoot_( _NEW( Node( NULL, MexTransform3d() ) ) )
{
    pCurrentNode_ = pRoot_;

    TEST_INVARIANT;
}

RenHierarchyBuilder::~RenHierarchyBuilder()
{
    TEST_INVARIANT;

    _DELETE( pRoot_ );
}

const RenHierarchyBuilder::Node* RenHierarchyBuilder::pRoot( void ) const
{
    return pRoot_;
}

void RenHierarchyBuilder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void    RenHierarchyBuilder::addFrame( const MexTransform3d& transform, size_t nChildren )
{
    addNode( _NEW( Node( pCurrentNode_, transform, nChildren ) ) );
}

void    RenHierarchyBuilder::addFrame( const MexTransform3d& transform, size_t nChildren, const RenIMeshID& meshId )
{
    addNode( _NEW( Node( pCurrentNode_, transform, nChildren, meshId ) ) );
}

void    RenHierarchyBuilder::addNode( Node* pNode )
{

//     if( pCurrentNode_ == NULL )
//     {
//         BOB_STREAM( "Node is root" << std::endl );
//
//         pRoot_ = pNode;
//         pCurrentNode_ = pNode;
//     }
//     else
    {
        pCurrentNode_->push_back( pNode );
        pCurrentNode_ = pNode;
    }

    while( pCurrentNode_ and pCurrentNode_->allChildrenSupplied() )
        pCurrentNode_ = pCurrentNode_->pParent();
}

ostream& operator <<( ostream& o, const RenHierarchyBuilder& t )
{

    o << "RenHierarchyBuilder " << (void*)&t << " start" << std::endl;
    o << "RenHierarchyBuilder " << (void*)&t << " end" << std::endl;

    return o;
}

/* End HIERBUIL.CPP *************************************************/
