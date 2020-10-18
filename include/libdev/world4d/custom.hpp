/*
 * C U S T O M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dCustom

    A concrete subclass of W4dEntity which is constructed with an empty mesh.
    A non-const reference to the mesh is exported, so that clients can modify
    the mesh, for example to add wire frame lines.
*/

#ifndef _WORLD4D_CUSTOM_HPP
#define _WORLD4D_CUSTOM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"
#include "world4d/entity.hpp"

//forward refs
class RenMesh;

//orthodox canonical (revoked)
class W4dCustom : public W4dEntity
{
public:
    //ctor. Creates empty mesh. Attaches to pParent using position localTransform.
    //Solidity flag is solid.
    //The default empty mesh is visible up to a distance of firstLODDistance.
    W4dCustom( W4dEntity* pParent, const W4dTransform3d& localTransform, Solidity solid,
               W4dDistance firstLODDistance );

    //Copying ctor.
    W4dCustom( const W4dCustom& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform );

    //Add another lod mesh visible up to distance.
    //LODid must be next in sequence (the default one has id 0).
    void addMesh( W4dDistance distance, W4dLOD LODid );

    //Reference to the mesh at specified lod.
    //Can be customized.
    RenMesh& mesh( W4dLOD LODid = 0 );

	// Delete the mesh at specifies lod and replace with an empty mesh.
	void emptyMesh( W4dDistance distance, W4dLOD LODid = 0 );

	// Load a unique, modifiable mesh at the specified LOD.
    bool loadUniqueMesh(const SysPathName&, const string& meshName, W4dDistance, W4dLOD = 0);

    virtual ~W4dCustom();

    //True if this entity intersects line.
    //If so returns the distance from line.end1() to the intersection point in pDistance.
    //This method is virtual so that the derived class can implement variant behaviour,
    //using, for example, known special features of the geometry of the object.
    //The accuracy of the test is indicated by accuracy.
    //Appropriate default behaviour might be to call defaultIntersects().
    //NB could not call this method "intersects" because of overload conflict in subclasses
    //with the intersects( domain ) method.
    //Default returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCustom& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dCustom );
    PER_FRIEND_READ_WRITE( W4dCustom );

private:
    // Operations deliberately revoked
    W4dCustom( const W4dCustom& );
    W4dCustom& operator =( const W4dCustom& );
    bool operator ==( const W4dCustom& );

    //data members
    ctl_pvector< RenMesh > meshes_;
};

PER_DECLARE_PERSISTENT( W4dCustom );

#ifdef _INLINE
    #include "world4d/custom.ipp"
#endif


#endif

/* End CUSTOM.HPP ***************************************************/
