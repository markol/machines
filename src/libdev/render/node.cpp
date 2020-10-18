/*
 * N O D E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <string>
#include "render/hierbuil.hpp"
#include "render/meshinst.hpp"
#include "render/internal/meshid.hpp"
#include "utility/indent.hpp"

RenHierarchyBuilder::Node::Node(
    Node* pParent,
    const MexTransform3d& transform )
: pParent_( pParent ),
  transform_( transform ),
  root_( true ),
  scale_( 1.0 )
{
    reserve( 4 );
}

RenHierarchyBuilder::Node::Node(
    Node* pParent,
    const MexTransform3d& transform,
    size_t nChildren )
: pParent_( pParent ),
  transform_( transform ),
  nChildren_( nChildren ),
  root_( false ),
  scale_( 1.0 )
{
    reserve( nChildren );
}

RenHierarchyBuilder::Node::Node(
    Node* pParent,
    const MexTransform3d& transform,
    size_t nChildren,
    const RenIMeshID& id )
: pParent_( pParent ),
  transform_( transform ),
  nChildren_( nChildren ),
  pathName_( id.pathName() ),
  meshName_( id.combinedName() ),
  instanceName_( id.instanceName() ),
  scale_( id.scale() ),
  root_( false )
{
    reserve( nChildren );
}

bool    RenHierarchyBuilder::Node::allChildrenSupplied( void ) const
{
    //  We do not know how many children the root node will have in
    //  advance so we cannot tell if all children have been supplied
    if( root_ )
        return false;

    return nChildren_ == size();
}

RenHierarchyBuilder::Node*   RenHierarchyBuilder::Node::pParent( void ) const
{
    return pParent_;
}

RenHierarchyBuilder::Node::~Node()
{
    TEST_INVARIANT;

    for( iterator i = begin(); i != end(); ++i )
        _DELETE( *i );
}

const MexTransform3d& RenHierarchyBuilder::Node::transform( void ) const
{
    return transform_;
}

RenMeshInstance* RenHierarchyBuilder::Node::pMeshInstance( void ) const
{
    RenMeshInstance* result;

    if( pathName_.set() )
    {
//        BOB_STREAM( "Create shared mesh " << pathName_ << " " << meshName_ << " " << scale_ << std::endl );
        result = RenMeshInstance::createShared( pathName_, meshName_, scale_ );
    }
    else
    {
        result = NULL;
    }

    return result;
}

const std::string& RenHierarchyBuilder::Node::instanceName( void ) const
{
    return instanceName_;
}

void RenHierarchyBuilder::Node::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenHierarchyBuilder::Node& t )
{
    o << "Node " << t.pathName_ << " " << t.meshName_ << std::endl;
    o << t.transform_;

    UtlIndentOstream    ostr( o, "  " );

    for( RenHierarchyBuilder::Node::const_iterator i = t.begin(); i != t.end(); ++i )
        ostr << *(*i);

    return o;
}

/* End NODE.CPP *****************************************************/
