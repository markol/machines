/*
 * M E S H I N S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_MESHINST_HPP
#define _REN_MESHINST_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "mathex/abox3d.hpp"
#include "render/render.hpp"
#include "render/fwmatmap.hpp"
#include "render/material.hpp"

//template <class T> class basic_string;
//typedef basic_string< char > string;

//class ostream;
class MexTransform3d;
class SysPathName;
class RenMesh;
class RenScale;
class RenMaterialVec;
class RenUVTransform;

// An instance of a RenMesh plus a collection of overridable attributes, such
// as material.
class RenMeshInstance
{
public:
	// PRE(mesh.isDefined());
    RenMeshInstance(const Ren::ConstMeshPtr& mesh);
    RenMeshInstance(const RenMeshInstance& copyMe);

	// As a convenience, this is simply _NEW(RenMeshInstance(RenMesh::createShared(...)))
	// It helps to reduce dependencies for clients (W4dEntity) who only read
	// and render instances.  The caller must delete the returned object.
	static RenMeshInstance* createShared(const SysPathName& path, const std::string & mesh, double scale = 1.0);

	// TBD: in the presence of multiple devices, we should check to see that
	// none of the devices is currently rendering (or something).
	// PRE(!RenDevice::current().rendering());
    ~RenMeshInstance();

	// Note: the render library is free to defer the actual drawing of this
	// object until the end of the frame.  Hence, the owner *cannot* delete it
	// until RenDevice::endFrame is called.  See precondition for dtor.
    void render( const MexTransform3d& transform ) const;

    //Same as above but applies an additional scale oeration defined by extraScale
    void render( const MexTransform3d& transform, const RenScale& extraScale ) const;

	// Note: non-const access is not possible through this interface.  If a
	// client needs to modify a mesh which is held by an instance object, that
	// client must itself retain a pointer to a non-const mesh.
    Ren::ConstMeshPtr mesh() const;

    //  Add the materials used by this instance to the given set
    void    addMaterials( RenMaterialSet* pSet ) const;

    //  Remap the materials that are being used.
    void    materialMap( const RenMaterialMap& map );

    // A mesh instance holds a material vector override for the mesh.
    void materialVec( const Ren::MaterialVecPtr& materialVecPtr );
    const Ren::MaterialVecPtr& materialVec() const;

	// Override the scale for this instance.  Makes a copy of the given scale.
    void  scale(const RenScale&);
    const RenScale& scale() const;

	// For backwards compatibiltiy, this creates a RenUniformScale.
    void  scale(MATHEX_SCALAR uniform);

	// Animate *all* the UV coords in this mesh.
	void	animateTextureCoords(const Ren::UVTransformPtr&);

    //  Return the unscaled bounding volume of the mesh
    const MexAlignedBox3d& boundingVolume() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenMeshInstance& t );

    PER_MEMBER_PERSISTENT( RenMeshInstance );
    PER_FRIEND_READ_WRITE( RenMeshInstance );

private:
    Ren::ConstMeshPtr	pMesh_;

    //  This is the material set used for overriding the mesh's own set.
    Ren::MaterialVecPtr materialVecPtr_;

	// A texture coordinate animation may be present.
	Ren::UVTransformPtr	texAnimationPtr_;

	RenScale*		scale_;

	// This is the default value for scale_, if a client doesn't set it.
	static RenScale* unityScale();

    // Operations deliberately revoked
    //RenMeshInstance( const RenMeshInstance& );
    RenMeshInstance& operator =( const RenMeshInstance& );
    bool operator ==( const RenMeshInstance& );
};

PER_DECLARE_PERSISTENT( RenMeshInstance );

#endif

/* End MESHINST.HPP *************************************************/
