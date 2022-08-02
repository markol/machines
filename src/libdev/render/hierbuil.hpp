/*
 * H I E R B U I L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    RenHierarchyBuilder

    A brief description of the class should go in here
*/

#ifndef _RENDER_HIERBUIL_HPP
#define _RENDER_HIERBUIL_HPP

#include "base/base.hpp"

#include "ctl/vector.hpp"
#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"

class RenMeshInstance;
class RenIMeshID;

class RenHierarchyBuilder
{
public:
    RenHierarchyBuilder();
    ~RenHierarchyBuilder();

    #include "render/node.hpp"
    
    const Node*   pRoot() const;
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenHierarchyBuilder& t );

protected:
    //  Add a frame to the hierarchy. It is the responsibility of the
    //  hierarchy to delete the mesh instance. The children of this frame
    //  will be added in order and recursively (i.e. child 1 will be added,
    //  then all of its children added, then child 2 etc. ). If there is no
    //  mesh associated with this frame pMeshInstance will be NULL.
    //  The derived class should provide an implementation of this function
    //  to produce the hierarchy.
    
//    virtual void    addFrame( const MexTransform3d&, size_t nChildren, RenMeshInstance* pMeshInstance ) = 0;

private:
    // Operation deliberately revoked
    RenHierarchyBuilder( const RenHierarchyBuilder& );

    // Operation deliberately revoked
    RenHierarchyBuilder& operator =( const RenHierarchyBuilder& );

    // Operation deliberately revoked
    bool operator ==( const RenHierarchyBuilder& );

    friend class RenID3DMeshLoader;
    friend class RenIGXMeshLoader;

    //  Add a frame without a mesh to the hierarchy
    void    addFrame( const MexTransform3d&, size_t nChildren );

    //  Add a frame with a mesh to the hierarchy
    void    addFrame( const MexTransform3d&, size_t nChildren, const RenIMeshID& meshId );

    void    addNode( Node* pNode );

    Node*   pCurrentNode_;
    Node*   pRoot_;
};


#endif

/* End HIERBUIL.HPP *************************************************/
