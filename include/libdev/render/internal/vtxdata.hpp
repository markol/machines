/*
 * V T X D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_VTXDATA_HPP
#define _RENDER_VTXDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/mmvector.hpp"
#include "ctl/countptr.hpp"
#include "render/internal/fixptvec.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point2d.hpp"

class MexPoint3d;
class MexPoint2d;
class RenIVertexIntensities;
class RenIVertexMaterials;
template <class T> class ctl_vector;

// NB: this is written to have the exact same layout as a D3DLVERTEX.  The
// normalIndex member occupies the same space as the vertex colour in the
// D3D struct.  This allows us to memcpy from RenIVertexData to a D3DLVERTEX
// array.  It *could* break under future versions of DirectX.
struct RenIVertex
{
	float x, y, z;
	//DWORD dwReserved;
	union
	{
        uint dwReserved;
        float w;
    };
	union
	{
		//DWORD normalIndex;
		uint normalIndex;
		//D3DCOLOR color;
		uint color;
	};
	//D3DCOLOR specular;
	uint specular;
	//D3DVALUE tu, tv;
	float tu, tv;
};

ostream& operator<<(ostream&, const RenIVertex&);
PerOstream& operator <<( PerOstream&, const RenIVertex& );
PerIstream& operator >>( PerIstream&, RenIVertex& );

//////////////////////////////////////////////////////////////////////////
// A list of all the vertices in a mesh.
class RenIVertexData : public ctl_min_memory_vector<RenIVertex>
{
public:
	typedef ctl_min_memory_vector<RenIVertex> Base;
	RenIVertexData(size_t nVertices = 20);
	~RenIVertexData();

	RenIVertexData(const RenIVertexData& copyMe);

	// PRE(nElements <= copyMe.size());
	RenIVertexData(const RenIVertexData& copyMe, size_t nElements);

	// This class screens the incomming data for unique normals, so you have
	// to add new vertices using this method.
	void addVertex(const MexPoint3d& pt, const MexVec3& normal, const MexPoint2d& uv);

	// If the caller knows that several vertices share the same normal, use
	// this method which is more efficient.
	// PRE(implies(nPts==0, pts.size() == uvs.size()));
	// PRE(implies(nPts!=0, pts.size() <= nPts && uvs.size() <= nPts));
	void addVertices(const ctl_vector<MexPoint3d>& pts, const MexVec3& normal,
					 const ctl_vector<MexPoint2d>& uvs, size_t nPts=0);

	// Const access to the vertices for rendering purposes.
	const ctl_min_memory_vector<RenIVertex>& asVector() const	{ return *this; }

	typedef ctl_min_memory_vector<RenIVec3FixPtS0_7> Normals;
	const Normals& normals() const		{ return normals_; }

	// Given an iterator pointing to a vertex, what is its normal?
	const RenIVec3FixPtS0_7& normal(Base::const_iterator it) const;
	const RenIVec3FixPtS0_7& normal(const RenIVertex& v) const;
	RenIVec3FixPtS0_7& normal(const RenIVertex& v);

	// Copy x, y, z, tu, and tv to the given array (which we must assume is
	// long enough).
	//void copyTo(D3DLVERTEX*);

	const RenIVertexIntensities* intensityMap() const	{ return (intensityMap_.isDefined())? &(*intensityMap_): NULL; }
	RenIVertexIntensities* intensityMap()				{ return (intensityMap_.isDefined())? &(*intensityMap_): NULL; }
	void createIntensityMap();							// POST(intensityMap());
	void resetAllIntensities();

	const RenIVertexMaterials* materialMap() const		{ return (materialMap_.isDefined())? &(*materialMap_): NULL; }
	RenIVertexMaterials* materialMap()					{ return (materialMap_.isDefined())? &(*materialMap_): NULL; }
	void createMaterialMap();							// POST(materialMap());
	void resetAllMaterials();

	Base::size;

	// Remove all the vertices, the normals and any maps.
	void eraseAll();

	// NB: this does not use the "DEFAULT" version of the persistence macro.
	// Calling the default ctor will reserve space for 20 vertices.  The vector
	// persistence read method handles the space allocation more efficiently.
    PER_MEMBER_PERSISTENT_VIRTUAL( RenIVertexData );
    PER_FRIEND_READ_WRITE( RenIVertexData );

private:
	CtlCountedPtr<RenIVertexIntensities>	intensityMap_;
	CtlCountedPtr<RenIVertexMaterials>		materialMap_;
	Normals									normals_;
};

PER_DECLARE_PERSISTENT( RenIVertexData );
ostream& operator <<( ostream&, const RenIVertexData& );

#endif

/* End VTXDATA.HPP **************************************************/
