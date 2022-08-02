/*
 * M E S H . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_MESH_HPP
#define _REN_MESH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"

#include <memory>

#include "system/pathname.hpp"
#include "stdlib/memory.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"		// TBD: shouldn't be required.
#include "ctl/countptr.hpp"
#include "ctl/mmvector.hpp"
#include "render/render.hpp"
#include "render/matvec.hpp"
#include "render/texture.hpp"

class IDirect3DRMMeshBuilder;
class MexTransform3d;
class GXMesh;
namespace XFile { class Scene; class Mesh; };

class RenDevice;
class RenMaterial;
class RenNormalMap;
class RenColourMap;
class RenTriangleList;
class RenTriangle;
class RenTTFPolygon;
class RenSpinTFPolygon;
class RenUVTransform;
class RenScale;
class RenVertex;

class RenIVertexData;
class RenITriangleGroup;
class RenIDistinctGroup;
class StripGroup;
class FanGroup;
class RenILineGroup;

template <class T> class ctl_vector;

// Not implemented, yet:
//		other than material and scale, dynamic overriding of attributes whilst rendering
//		any of the mesh editing API
//		strips or fans
//		animated texture UV co-ordinates
class RenMesh
{
public:
	// Load a named mesh from a file.
	// RenDevice::useDevice() must be called before any of the creation methods.
	// PRE(Ren::initialised());
	// PRE(RenDevice::current());
	static Ren::ConstMeshPtr createShared(const SysPathName& path, const string& mesh, double scale = 1.0);
	static Ren::MeshPtr		 createUnique(const SysPathName& path, const string& mesh, double scale = 1.0);
	static Ren::MeshPtr		 createEmpty();

    virtual ~RenMesh();

	// The basic, most efficient way of drawing this mesh as it is.
	// RenDevice::useDevice() must be called before this method.
	void render(const MexTransform3d& world, const RenScale& scale) const;

	// Draw this mesh, dynamically overriding various attributes.
	void render(const MexTransform3d& world, const RenMaterialVec*,      const RenScale& scale) const;
	void render(const MexTransform3d& world, const RenUVTransform& anim, const RenScale& scale) const;
	void render(const MexTransform3d& world, const RenMaterialVec*, const RenUVTransform& anim, const RenScale& scale) const;

	int nVertices() const;

	// Use this non-const version to modify vertices belonging to this mesh.
	// It is assumed that the lifetime of the returned object will not outlast
	// the mesh, otherwise it will refer to a non-existant vertex.
	// PRE(idx < nVertices());
	// PRE(!RenDevice::current()->rendering()); for all non-const access
	RenVertex vertex(Ren::VertexIdx idx);

	// It is assumed that the lifetime of the returned object will not outlast
	// the mesh, otherwise it will refer to a non-existant vertex.
	// PRE(idx < nVertices());
	const RenVertex vertex(Ren::VertexIdx idx) const;

	// This version adds the point even if there is already an identical one.
	// Use this where two non-shared co-incident vertices are required.
	// PRE(!RenDevice::current()->rendering()); for all non-const access
	const RenVertex& addVertex(const MexPoint3d&);

	// This version searches for an existing point within epsillon of the
	// given one.  If an existing point is found, then it will be returned.
	// PRE(!RenDevice::current()->rendering()); for all non-const access
	const RenVertex& addVertex(const MexPoint3d&, MATHEX_SCALAR epsillon);

	// Set all vertex intensities to 1.0.  This is *much* quicker than using
	// the RenVertex interface repeatedly.
	void resetAllVertexIntensities();
	void resetAllVertexMaterials();

	// Access to the polygons which make up this mesh.
	int nTriangleLists() const;
	RenTriangleList& triangleList(Ren::TriangleIdx index);
	const RenTriangleList& triangleList(Ren::TriangleIdx index) const;

	// Read-only access to the entire list of polygons regardless of
	// whether they are distinct, strips or fans.
	int nTriangles() const;
	const RenTriangle& triangle(Ren::TriangleIdx index) const;

	// Read-only access to lines.
	// TBD: complete API access to lines.
	// PRE(!RenDevice::current()->rendering()); for all non-const access
	void addLine(const MexPoint3d&, const MexPoint3d&, const RenMaterial&);
	int nLines() const;

	// The ordering of the polygons will not be maintained if any
	// polygon is modified.
	// PRE(!RenDevice::current()->rendering()); for all non-const access
	void addTTFPolygon(const RenTTFPolygon&);
	void removeTTFPolygon(Ren::TriangleIdx idx);
	int nTTFPolygons() const;
	const RenTTFPolygon& TTFPolygon(Ren::TriangleIdx idx) const;

	// The modify method is replaced by the non-const TTFPolygon access method.
	// void modifyTTFPolygon(Ren::TriangleIdx idx, const RenTTFPolygon&);
	RenTTFPolygon& TTFPolygon(Ren::TriangleIdx idx);

	// PRE(!RenDevice::current()->rendering()); for all non-const access
	void addSpinTFPolygon(const RenSpinTFPolygon&);
	void removeSpinTFPolygon(Ren::TriangleIdx idx);
	void modifySpinTFPolygon(Ren::TriangleIdx idx, const RenSpinTFPolygon&);
	int nSpinTFPolygons() const;
	const RenSpinTFPolygon& spinTFPolygon(Ren::TriangleIdx idx) const;

	// The set of materials used by this mesh.
	size_t nMaterials() const;
	void materialVec(const RenMaterialVec*);
	std::auto_ptr<RenMaterialVec> materialVec() const;

	// Change the backface setting for all the polygons of this mesh.
	// Backface culling *is* applied by default.
	void backFaceAll(bool);

	// Change the backfacing flag for the first polygon group which uses the
	// given material.  (If there's more than one, you can't have finer
	// control; like it or lump it).
	void backFace(const RenMaterial&, bool);
	bool backFace(const RenMaterial&) const;

	// The contents of mesh files are cached when read, so that successive
	// reads from the same file do not require parsing it twice.  This fn.
	// should be called when the client has finished reading meshes, so that
	// the memory can be freed up.
	static void emptyCache();

    //Return the number of faces in this mesh. The number of vertices is returned
    //in nVertices. The vertex coordinates are pushed into vertices.
    //The face data is pushed into faceData. The data for each face consists of the
    //number of vertices, followed by an index into vertices for each vertex.
    size_t faces( size_t* pNVertices,
                ctl_vector<MexPoint3d>* vertices, ctl_vector<size_t>* faceData ) const;

    const MexAlignedBox3d& boundingVolume() const;

	// Returns the name of the file that this was loaded from.  For debugging.
	const SysPathName& pathName() const	{ return pathName_; }
	const string&	   meshName() const	{ return meshName_; }
	void meshName(const string&);

	// These are forwared to the internal class RenIMeshFactory.
	// These methods don't actually destroy the mesh instances, they just
	// release the counted pointers held by the factory.
	static void removeFromFactory(Ren::ConstMeshPtr);
	static void removeAllFromFactory();

	// PRE(Ren::initialised());
	static ulong totalMeshCount();

	// This is the maximum number of vertices stored in any mesh.  Used in
	// the lighting calculations.
	// PRE(Ren::initialised());
	static ulong maxVertices();

	static void startFrame();
	static ushort meshId();

	const RenIVertexData*   vertices()     const		{ return vertices_; }

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenMesh );
    PER_FRIEND_READ_WRITE( RenMesh );

    friend ostream& operator <<( ostream& o, const Ren::ConstMeshPtr& t );

    typedef ctl_vector< RenTexture > Textures;

private:
	// RenVertex is granted non-const access to the vertex representation so
	// that it can modify vertices.  Other internal clients only have const
	// access to the same vector.
	friend class RenVertex;
	RenIVertexData*   vertices()			{ return vertices_; }

	SysPathName			pathName_;
	string	   			meshName_;
	RenIVertexData*		vertices_;
	RenIVertexData*		uvAnimated_;
    MexAlignedBox3d     boundingVolume_;
    bool                isDirty_;		//Indicates the mesh data has changed

	ctl_min_memory_vector<RenITriangleGroup*>	triangles_;
	ctl_min_memory_vector<RenTTFPolygon*>		ttfs_;
	ctl_min_memory_vector<RenSpinTFPolygon*>	stfps_;
	ctl_min_memory_vector<RenILineGroup*>		lines_;

	// For debugging, this is the total number of mesh objects in existance.
	static ulong		meshCount_, maxVertices_;

	//typedef ctl_vector< RenTexture > Textures;
	Textures*			pVertexTexture_;

	bool read(const SysPathName& path, const string& mesh, double scale = 1.0);
	bool copyFromMeshBuilder(IDirect3DRMMeshBuilder*);
	Ren::VertexIdx addOrFindVertex(const MexPoint3d&, double epsilon);
	bool buildFromXMesh(XFile::Scene*, XFile::Mesh*);
	bool buildFromGXMesh(GXMesh*);
	void createTextures();

    //Called when the data is dirty (because the mesh has been edited)
    //to update any related info.
    void updateAllData();

    //Updates the cached bounding volume from all the polygon data
    void updateBoundingVolume();

    RenMesh();

	static ushort nextId();
	static ushort& privateMeshId();

    // Operations deliberately revoked
    RenMesh( const RenMesh& );
    RenMesh& operator =( const RenMesh& );
    bool operator ==( const RenMesh& );
};

PER_DECLARE_PERSISTENT( RenMesh );

#endif

/* End MESH.HPP *****************************************************/
