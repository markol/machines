/*
 * N O D E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    Node

    A brief description of the class should go in here
*/

#ifndef _RENDER_NODE_HPP
#define _RENDER_NODE_HPP

class Node;

class Node : public ctl_vector< Node* >
{
public:
    ~Node();

    void CLASS_INVARIANT;

    const MexTransform3d& transform() const;
    RenMeshInstance* pMeshInstance() const;

    const std::string & instanceName() const;

    friend ostream& operator <<( ostream& o, const Node& t );

private:
    // Operation deliberately revoked
    Node( const Node& );

    // Operation deliberately revoked
    Node& operator =( const Node& );

    // Operation deliberately revoked
    bool operator ==( const Node& );

    friend class RenHierarchyBuilder;

    //  This is used to construct the root node - we don't know
    //  how many children the root can have, extra meshes can be
    //  introduced at any time
    Node(
        Node* pParent,
        const MexTransform3d& transform );

    Node(
        Node* pParent,
        const MexTransform3d& transform,
        size_t nChildren );
    Node(
        Node* pParent,
        const MexTransform3d& transform,
        size_t nChildren,
        const RenIMeshID& id );

    bool    allChildrenSupplied() const;
    Node*   pParent() const;

    //  This is the number of children that will
    //  eventually be supplied
    size_t  nChildren_;
    Node*   pParent_;
    bool    root_;

    MexTransform3d  transform_;

    SysPathName     pathName_;
    std::string           meshName_;
    std::string           instanceName_;
    double          scale_;
};


#endif

/* End NODE.HPP *****************************************************/
