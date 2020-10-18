/*
 * R E N D E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_RENDER_HPP
#define _RENDER_RENDER_HPP

#include "base/base.hpp"

//class ostream;
class RenMesh;
class RenMaterialVec;
class RenUVTransform;
template <class T> class CtlCountedPtr;
template <class T> class CtlConstCountedPtr;

// Various widely used typedefs.  These could correctly be put in class
// scopes, e.g. RenTexManager::TexId.  However, putting them here helps
// reduce dependencies.
// Hence, this file is supposed to be **small & simple**.
struct Ren
{
    static const uint NullTexId = 0;
	// This returns RenDevice::current()->out for classes which don't need the
	// full definition of RenDevice.
	// PRE(RenDevice::current());
	static ostream& out();

	// This invokes the various render library Singletons in an order which
	// will guarantee correct destruction order.  This must be called before
	// any other render library Singletons are invoked.
	// Chose a co-ordinate system before starting the render lib.
	// PRE(MexCoordSystem::instance().isSet());
	static void initialise();
	static bool initialised();		// a pre-condition for many things

	typedef ushort VertexIdx;
	typedef ushort TriangleIdx;
	typedef uchar  MaterialIdx;
	typedef uint   TexId;
	// TODO init
	// static const uint NullTexId;

	// These are maximum values per mesh.
	enum { maxVertices = 65535, maxTriangles = 65535, maxMaterials = 255 };

	typedef CtlCountedPtr<RenMesh>			MeshPtr;
	typedef CtlConstCountedPtr<RenMesh>		ConstMeshPtr;

	typedef CtlCountedPtr< RenMaterialVec >	MaterialVecPtr;
	typedef CtlConstCountedPtr< RenMaterialVec > ConstMaterialVecPtr;

	typedef CtlCountedPtr< RenUVTransform >	UVTransformPtr;
	typedef CtlConstCountedPtr< RenUVTransform > ConstUVTransformPtr;

	struct Rect
	{
		Rect(int x=0, int y=0, int w=0, int h=0): originX(x), originY(y), width(w), height(h)	{}
		int originX, originY, width, height;
	};

	enum LightType { DIRECTIONAL, POINT, UNIFORM };
};

ostream& operator<<(ostream&, const Ren::Rect&);

#endif

/* End RENDER.HPP ***************************************************/
