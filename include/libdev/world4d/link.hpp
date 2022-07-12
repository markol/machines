/*
 * L I N K . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    W4dLink

    Represents an internal link in a W4dComposite entity
*/

#ifndef _LINK_HPP
#define _LINK_HPP

#include "stdlib/string.hpp"
#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/world4d.hpp"
#include "world4d/entity.hpp"

class W4dComposite;
class W4dMesh;

class W4dLink : public W4dEntity
{
public:

    ~W4dLink();

    void CLASS_INVARIANT;

    W4dComposite* pComposite() const;

    typedef W4dLinkId   Id;

    const Id& id() const
        { return id_; };

    void add( RenMeshInstance* pMeshInstance, W4dLOD LODid = 0 )
        { W4dEntity::add( pMeshInstance, LODid ); };

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dLink );
    PER_FRIEND_READ_WRITE( W4dLink );

protected:
    virtual void doDebugOutput( ostream& ostr );
    // Write out the details of 'this' for debugging purposes.

private:
    // Operation deliberately revoked
    W4dLink( const W4dLink& );

    // Operation deliberately revoked
    W4dLink& operator =( const W4dLink& );

    // Operation deliberately revoked
    bool operator ==( const W4dLink& );

    friend class W4dComposite;

    //  The id is assumed to be the index into the
    //  W4dComposite's vector of W4dLinks

    W4dLink( W4dComposite* pOwningComposite, const W4dTransform3d& localTransform,
             Solidity solid, const W4dLink::Id& id );

    W4dLink( W4dLink* pOwningLink, const W4dTransform3d& localTransform,
             Solidity solid, const W4dLink::Id& id );

    W4dLink( const W4dLink& copyMe, W4dComposite* pComposite,
             W4dEntity* pOwner, const W4dTransform3d& localTransform,
             const W4dLink::Id& id );

    //Data members
    W4dComposite*   pComposite_; //Owner
    Id              id_; //Unique id allocated by the composite
};

PER_DECLARE_PERSISTENT( W4dLink );

#ifdef _INLINE
#include "world4d\link.ipp"
#endif

#endif

/* End LINK.HPP *****************************************************/
