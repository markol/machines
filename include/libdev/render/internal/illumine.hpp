/*
 * I L L U M I N E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_ILLUMINE_HPP
#define _RENDER_ILLUMINE_HPP

#include "base/base.hpp"
#include <memory>
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"
#include "render/colour.hpp"
#include "render/render.hpp"
#include "render/internal/internal.hpp"

class MexTransform3d;
class MexAlignedBox3d;
class RenLight;
class RenDevice;
class RenMaterial;
class RenMaterialTransform;
class RenILight;
class RenIMatBody;
class RenIVertexData;
class RenILightingBuffer;
class RenIVertexIntensities;
class RenIExpandedIntensityMap;
class RenIDeviceImpl;
class RenIVertex;
template <class T> class ctl_min_memory_vector;

// An object responsible for all of the lighting which effects a rendered
// scene.  Stores the list of lights and performing lighting calculations.
// This is an ABC; derived classes implement their calculations differently.
class RenIIlluminator
{
public:
    virtual ~RenIIlluminator();

	// Modify the list of objects which can potentially light the scene.
	// PRE(light);
	void    addLight(RenLight* light);
	void removeLight(RenLight* light);

	// The default value is a white light of 0.3 intensity.
	const RenColour& ambient() const;
	void ambient(const RenColour&);

	// This is a colour filter which is applied to every rendered colour.
	// See RenCamera for more details.  The value is copied from there.
	const RenColour& filter() const;
	void filter(const RenColour&);

	// This has the effect of turning all lights off.  Polygons will be drawn with
	// ambient and self-luminous lighting only.
	void disableLighting();
	void enableLighting();
	bool lightingEnabled() const;

	void startFrame();

	// Applys all the current lights to the given array of unlit vertices.
	// The caller may provide a bounding volume which should include all the
	// vertices.  If this is present, it can be used for optimised calculations.
	// PRE(in.size() <= RenMesh::maxVertices());
	// POST(validVertices_ == in.size()); POST(litData_ == &in);
	void lightVertices(const RenIVertexData& in, const MexAlignedBox3d* pVolume=NULL);

	// Optionally light only a sub-set of in.
	// PRE(nVertices <= RenMesh::maxVertices()); PRE(nVertices <= in.size());
	// POST(validVertices_ == nVertices); POST(litData_ == &in);
	void lightVertices(const RenIVertexData& in, size_t nVertices, const MexAlignedBox3d* pVolume=NULL);

	typedef ctl_min_memory_vector<Ren::VertexIdx> Indices;

	// Combines the given material with the reflected intensity at each vertex
	// referred to by the indices array.  Returns a pointer to an internal
	// array of lit vertices.  These will be overwritten by the next call to
	// applyMaterial.  The intention is that the result is used *immediately*
	// as an argument to a DrawPrimitive call.  Assumes that lightVertices has
	// previously been called with the same in arg:
	// PRE(litData_ == &in);
	// PRE(litData_->size() == in.size()); PRE(validVertices_ == in.size());
	//virtual const D3DLVERTEX* applyMaterial(const RenMaterial&, const RenIVertexData& in, const Indices&) =0;
	virtual const RenIVertex* applyMaterial(const RenMaterial&, const RenIVertexData& in, const Indices&) =0;

	// Yet again, the same function, but applied to a sub-set of the vertices,
	// starting at the beginning and applied to nVertices many.
	// PRE(litData_ == &in);				PRE(nVertices <= in.size());
	// PRE(nVertices <= litData_->size());	PRE(nVertices <= validVertices_);
	//const D3DLVERTEX* applyMaterial(const RenMaterial&, const RenIVertexData& in, size_t nVertices);
	const RenIVertex* applyMaterial(const RenMaterial&, const RenIVertexData& in, size_t nVertices);

	// Exactly the same as above, but applies to all the vertices.
	// Same pre-conditions, plus:
	// PRE(litData_ == &in);
	// PRE(in.size() == litData_->size()); PRE(in.size() == validVertices_);
	const RenIVertex* applyMaterial(const RenMaterial&, const RenIVertexData& in);

	// This is exactly the same as applyMaterial, except that it returns a
	// unique copy of the internal array.  This copy can be stored and used
	// for rendering delay-sorted alpha polygons.  (Same pre-conditions.)
	// PRE(nIndicesUsed <= in.size());	(additional pre-condition)
	RenI::LitVtxAPtr applyMaterialAndCopy(const RenMaterial&, const RenIVertexData& in, const Indices&, Ren::VertexIdx nIndicesUsed);

	// This is exactly the same as applyMaterial, except that it returns a
	// unique copy of the internal array.  This copy can be stored and used
	// for rendering delay-sorted alpha polygons.  (Same pre-conditions.)
	// PRE(nVertices <= in.size());	(additional pre-condition)
	RenI::LitVtxAPtr applyMaterialAndCopy(const RenMaterial&, const RenIVertexData& in, size_t nVertices);

	// Used for "lighting" TTF polygons.  The vertices are lit as if the lights in
	// the scene were applied to a vertical normal (0,0,1).  The intent is that the
	// result should represent the average lighting conditions in the scene.  This
	// may fail if there is one very strong directional light and little ambient.
	// Any colour filter and overflow effects are applied.
	// PRE(vertices);
	//void applyMaterialGenericLighting(const RenMaterial&, D3DTLVERTEX* vertices, size_t nVertices);
	void applyMaterialGenericLighting(const RenMaterial&, RenIVertex* vertices, size_t nVertices);

	void lightTurnedOn(RenILight*);
	void lightTurnedOff(RenILight*);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenIIlluminator& t );

protected:
    RenIIlluminator(RenIDeviceImpl*);

	const ctl_list<RenILight*>&		lights() const		   	{ return lights_; }
	const ctl_pvector<RenILight>&	lightsOn() const	   	{ return lightsOn_; }
	bool							disabled() const	   	{ return internallyDisabled_; }
	const RenIVertexIntensities*	expanded() const	   	{ return expanded_;}
	RenIExpandedIntensityMap*		expansion() const	   	{ return expansion_; }
	bool							perVertexMats() const  	{ return perVertexMats_; }
	const RenMaterialTransform*		globalMaterialTransform() const	{ return globalMaterialXform_; }

	void useLightingBuffer(RenILightingBuffer* l);		// PRE(l);

	// This is used to assert that lightVertices and applyMaterial are
	// applied to the same data.
	PRE_DATA(const RenIVertexData* litData_);

	// The illuminator is a friend of RenMaterial so that it may operate more
	// efficiently on constant material bodies.  Make this facility available to
	// all derived classes.
	const RenIMatBody* matBody(const RenMaterial& m);

private:
	virtual void computeLambertian(const RenIVertexData&, const MexTransform3d& world, const MexAlignedBox3d* pVolume) = 0;
	virtual void applyVertexMaterials(const RenIVertexData& in) = 0;
	virtual void doStartFrame();

	void computeFogAttenuation(const RenIVertexData&, const MexTransform3d& world);
	void copyCoords           (const RenIVertexData&);
	void computeGeneric();

	RenILightingBuffer*				lightingBuffer_;
	RenIDeviceImpl*					devImpl_;
	ctl_list<RenILight*>			lights_;
	ctl_pvector<RenILight>			lightsOn_;
	RenColour						ambient_;
	RenColour						filter_;
	bool							disabled_, internallyDisabled_;
	const RenIVertexIntensities*	expanded_;
	RenIExpandedIntensityMap*		expansion_;
	bool							perVertexMats_;
	RenColour						genericLambertian_;
	const RenMaterialTransform*		globalMaterialXform_;

    // Operation deliberately revoked
    RenIIlluminator( const RenIIlluminator& );
    RenIIlluminator& operator =( const RenIIlluminator& );
    bool operator ==( const RenIIlluminator& );
};

#ifdef _INLINE
    #include "internal/illumine.ipp"
#endif

#endif

/* End ILLUMINE.HPP *************************************************/
