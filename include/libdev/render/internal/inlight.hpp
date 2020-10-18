/*
 * I N L I G H T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_INLIGHT_HPP
#define _RENDER_INLIGHT_HPP

#include "base/base.hpp"
#include "render/internal/fixpt.hpp"
#include "render/colour.hpp"
#include "render/render.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "utility/property.hpp"
#include "base/persist.hpp"

class MexPoint3d;
class MexTransform3d;
class MexAlignedBox3d;
class RenIVertexData;
class RenIIlluminator;
class RenIFloatLightingBuffer;
class RenIFixPtLightingBuffer;
class RenLightColourTransform;

// Lights attributes are typically processed only once per frame.  So there
// are no efficiency concerns and the representations have been moved out of
// RenLight into these internal classes.
class RenILight
{
public:
	UtlProperty<RenColour>	colour;			// default: white
	UtlProperty<bool>		used;

	// By default, lights are on when created.
	bool on() const;
	void on(bool);

	// All light colours are multiplied by this before being used.
	// Defaults to white.
	static const RenLightColourTransform*& globalColourTransform();

	enum OutputAction { INITIALISE, ACCUMULATE };

	// The logic is simplified if we assume that this light is on before this
	// method is called: PRE(on());
	virtual void lightVertices(const RenIVertexData& in, RenIFloatLightingBuffer* out,
			const MexTransform3d& world, OutputAction sumtype, bool performRangeChecks,
			const MexAlignedBox3d* pVolume) const = 0;

	virtual void lightVerticesMMX(const RenIVertexData& in, RenIFixPtLightingBuffer* out,
			const MexTransform3d& world, OutputAction sumtype,
			const MexAlignedBox3d* pVolume) const = 0;

	virtual void accumulateGeneric(RenColour*) const = 0;
	virtual void print(ostream&) const = 0;

	// Called for every light at the start of each rendered frame.
	virtual void startFrame();

	virtual ~RenILight()	{}

	void illuminator(RenIIlluminator*);

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( RenILight );
    PER_FRIEND_READ_WRITE( RenILight );

protected:
	RenILight();

	// Apply the global transform, if any, to col.  PRE(col);
	void transformColour(RenColour* col, Ren::LightType t) const;

private:
	RenIIlluminator*	illuminator_;
	bool				on_;
};

PER_DECLARE_PERSISTENT( RenILight );

ostream& operator<<(ostream&, const RenILight*);
ostream& operator<<(ostream&, const RenILight&);

class RenIDirectionalLight : public RenILight
{
public:
	// Sets the direction along x-axis and the colour to white.
	RenIDirectionalLight();

	virtual void lightVerticesMMX(const RenIVertexData&, RenIFixPtLightingBuffer*, const MexTransform3d& world, OutputAction, const MexAlignedBox3d* pVolume) const;
	virtual void lightVertices(const RenIVertexData&, RenIFloatLightingBuffer*, const MexTransform3d& world, OutputAction, bool, const MexAlignedBox3d* pVolume) const;

	virtual void accumulateGeneric(RenColour*) const;
	virtual void print(ostream&) const;

	UtlProperty<MexVec3>	direction;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenIDirectionalLight );
    PER_FRIEND_READ_WRITE( RenIDirectionalLight );

};

PER_DECLARE_PERSISTENT( RenIDirectionalLight );

class RenIAttenuatedLight : public RenILight
{
public:
	void maxRange(MATHEX_SCALAR);
	MATHEX_SCALAR maxRange() const;

	UtlProperty<MexPoint3d>		position;
	UtlProperty<MATHEX_SCALAR>	constantAttenuation;
	UtlProperty<MATHEX_SCALAR>	linearAttenuation;
	UtlProperty<MATHEX_SCALAR>	quadraticAttenuation;

	static MATHEX_SCALAR& resultTollerance();

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( RenIAttenuatedLight );
    PER_FRIEND_READ_WRITE( RenIAttenuatedLight );

	// This does nothing.  It's difficult to imagine a sensible system for
	// point or uniform lights affecting TTFs.
	virtual void accumulateGeneric(RenColour*) const;
	virtual void startFrame();

protected:
	// Compute the attenuation as a function of squared range-to-vertex.
	// This uses the same maths as Direct3D's D3DLIGHT2.
	// PRE(rangeToVtxSquared >= 0);
	float attenuation(MATHEX_SCALAR rangeToVtx) const;
	RenIFixPtU0_8 attenuationFixPt(MATHEX_SCALAR rangeToVtx) const;

	// Calculate the attenuation given the position of a light and a vertex.
	float attenuation(MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, const MexPoint3d& pos,
					  MATHEX_SCALAR sqrRange) const;

private:
	float maxRange_, recipMaxRange_;
};

PER_DECLARE_PERSISTENT( RenIAttenuatedLight );

class RenIPointLight : public RenIAttenuatedLight
{
public:
	// Defaults the position to the origin.
	RenIPointLight(MATHEX_SCALAR range);		// PRE(r > 0);

	virtual void lightVerticesMMX(const RenIVertexData&, RenIFixPtLightingBuffer*, const MexTransform3d& world, OutputAction, const MexAlignedBox3d* pVolume) const;
	virtual void lightVertices(const RenIVertexData&, RenIFloatLightingBuffer*, const MexTransform3d& world, OutputAction, bool, const MexAlignedBox3d* pVolume) const;
	virtual void print(ostream&) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( RenIPointLight );
    PER_FRIEND_READ_WRITE( RenIPointLight );
};

PER_DECLARE_PERSISTENT( RenIPointLight );

// This is a rather odd light.  Vertices are illuminated as a function of
// *range only*, i.e., the vertex normals are ignored.  Put another way: it
// is an ambient source with a range of effect.  Although it has absolutely
// no basis in real physics, it may be useful for not-very-critical effects
// such as explosions.  It is, of course, a squillion times faster than a
// point light.
class RenIUniformLight : public RenIAttenuatedLight
{
public:
	// Defaults the position to the origin.
	RenIUniformLight(MATHEX_SCALAR range);		// PRE(r > 0);

	virtual void lightVerticesMMX(const RenIVertexData&, RenIFixPtLightingBuffer*, const MexTransform3d& world, OutputAction, const MexAlignedBox3d* pVolume) const;
	virtual void lightVertices(const RenIVertexData&, RenIFloatLightingBuffer*, const MexTransform3d& world, OutputAction, bool, const MexAlignedBox3d* pVolume) const;
	virtual void print(ostream&) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( RenIUniformLight );
    PER_FRIEND_READ_WRITE( RenIUniformLight );

private:
	bool boundingVolumeUniformlyLit(const MexAlignedBox3d* pVolume, const MexPoint3d& pos,
									MATHEX_SCALAR, float* pResult) const;
};

PER_DECLARE_PERSISTENT( RenIUniformLight );

// If there are no other lights, this is used by the illumination system as a
// dummy.  It fulfills the responsibilities of a light w.r.t. the lightVertices
// method but does no Lambertian calculation and sets all the output RGBs to
// zero.  The effect is the same as using any of the other light types with a
// colour of black.  The colour and on properties of the base class are ignored.
// There is no RenZeroLight in the public interface to the lib.
class RenIZeroLight : public RenILight
{
public:
	// Sets all the output RGBs to zero.
	virtual void lightVerticesMMX(const RenIVertexData&, RenIFixPtLightingBuffer*, const MexTransform3d& world, OutputAction, const MexAlignedBox3d* pVolume) const;
	virtual void lightVertices(const RenIVertexData&, RenIFloatLightingBuffer*, const MexTransform3d& world, OutputAction, bool, const MexAlignedBox3d* pVolume) const;
	virtual void accumulateGeneric(RenColour*) const;
	virtual void print(ostream&) const;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenIZeroLight );
    PER_FRIEND_READ_WRITE( RenIZeroLight );
};

PER_DECLARE_PERSISTENT( RenIZeroLight );

#ifdef _INLINE
    #include "internal/inlight.ipp"
#endif

#endif

/* End INLIGHT.HPP **************************************************/
