/*
 * I N L I G H T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/inlight.hpp"

#include "render/litxform.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/lightbuf.hpp"
#include "render/internal/illumine.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "ctl/vector.hpp"

#ifndef _INLINE
    #include "render/internal/inlight.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( RenILight );
PER_DEFINE_PERSISTENT_ABSTRACT( RenIAttenuatedLight );
PER_DEFINE_PERSISTENT( RenIZeroLight );
PER_DEFINE_PERSISTENT( RenIDirectionalLight );
PER_DEFINE_PERSISTENT( RenIPointLight );
PER_DEFINE_PERSISTENT( RenIUniformLight );

/////////////////////////////////////////////////////////////////////////////

RenILight::RenILight():
	on_(true),
	illuminator_(NULL)
{
	colour(RenColour::white());
	on(true);
}

// virtual
void RenILight::startFrame()
{
}

// static
const RenLightColourTransform*& RenILight::globalColourTransform()
{
	static const RenLightColourTransform* c = NULL;
	return c;
}

void RenILight::transformColour(RenColour* c, Ren::LightType t) const
{
	PRE(c);

	const RenLightColourTransform* xform = globalColourTransform();
	if (xform)
		xform->transform(*c, t, c);
}

void RenILight::on(bool b)
{
	const bool changed = (on_ && !b) || (!on_ && b);

	if (changed)
	{
		ASSERT(illuminator_, "No illuminator defined for RenILight.");
		on_ = b;

		if (on_)
			illuminator_->lightTurnedOn(this);
		else
			illuminator_->lightTurnedOff(this);
	}
}

ostream& operator<<(ostream& o, const RenILight* l)
{
	if (!l)
		o << "Null internal light pointer";
	else
		l->print(o);

	return o;
}

ostream& operator<<(ostream& o, const RenILight& l)
{
	l.print(o);
	return o;
}

void perWrite( PerOstream& ostr, const RenILight& light )
{
    ostr << light.colour;
    ostr << light.on_;
}

void perRead( PerIstream& istr, RenILight& light )
{
    istr >> light.colour;
    istr >> light.on_;
}

/////////////////////////////////////////////////////////////////////////////
inline void clampLow(float* pFloat, float limit)
{
	if (*pFloat < limit)
		*pFloat = limit;
}

inline void lightingRangeChecks
(
	bool performChecks,
	RenIFloatLightingBuffer*  out,
	float* pR, float* pG, float* pB,
	bool*& pUnderflow
)
{
	// If this is the last light and the sums have reached their final
	// values, then update the maximum values etc.
	if (performChecks)
	{
		out->checkMaxLambertian(*pR, *pG, *pB);

		clampLow(pR, 0);
		clampLow(pG, 0);
		clampLow(pB, 0);

		if (*pR == 0 && *pG == 0 && *pB == 0)
			*pUnderflow = true;

		++pUnderflow;
	}
}

inline void accumulateIllumination
(
	float* pR, float* pG, float* pB,
	RenILight::OutputAction sum,
	float intensity,
	const RenColour& colour
)
{
	const float r = (intensity * colour.r());
	const float g = (intensity * colour.g());
	const float b = (intensity * colour.b());

	switch (sum)
	{
		case RenILight::INITIALISE:
			*pR = r;
			*pG = g;
			*pB = b;
			break;
		case RenILight::ACCUMULATE:
			*pR += r;
			*pG += g;
			*pB += b;
			break;
	}
}

inline void accumulateIllumination
(
	float* pR, float* pG, float* pB,
	RenILight::OutputAction sum,
	const RenColour& colour
)
{
	switch (sum)
	{
		case RenILight::INITIALISE:
			*pR = colour.r();
			*pG = colour.g();
			*pB = colour.b();
			break;
		case RenILight::ACCUMULATE:
			*pR += colour.r();
			*pG += colour.g();
			*pB += colour.b();
			break;
	}
}

// TODO FixPtS11_4 is probably not used legacy code
/*FixPtS11_4 floatToFixPtS11_4(float f)
{
	return _STATIC_CAST(short, (f * 16 + 0.5));
}

inline accumulateIllumination
(
	MmxColourS11_4* output,
	RenILight::OutputAction sum,
	const RenIColourFixPtS15_16& colour
)
{
	static const FixPtS11_4 alpha = floatToFixPtS11_4(1);

	// Note: shifting a signed value is implementation dependent.  If it
	// works, do it because the alternative is a much slower division.
	switch (sum)
	{
		case RenILight::INITIALISE:
			output->r_ = _STATIC_CAST(FixPtS11_4, colour.r_ >> 12);
			output->g_ = _STATIC_CAST(FixPtS11_4, colour.g_ >> 12);
			output->b_ = _STATIC_CAST(FixPtS11_4, colour.b_ >> 12);
			output->a_ = alpha;
			break;
		case RenILight::ACCUMULATE:
			output->r_ += _STATIC_CAST(FixPtS11_4, colour.r_ >> 12);
			output->g_ += _STATIC_CAST(FixPtS11_4, colour.g_ >> 12);
			output->b_ += _STATIC_CAST(FixPtS11_4, colour.b_ >> 12);
			break;
	}
}

inline accumulateIllumination
(
	MmxColourS11_4* output,
	RenILight::OutputAction sum,
	RenIFixPtS15_16 intensity,
	const RenIColourFixPtS23_8& colour
)
{
	static const FixPtS11_4 alpha = floatToFixPtS11_4(1);

	// Note: shifting a signed value is implementation dependent.  If it
	// works, do it because the alternative is a much slower division.
	switch (sum)
	{
		case RenILight::INITIALISE:
			output->r_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.r_) >> 20);
			output->g_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.g_) >> 20);
			output->b_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.b_) >> 20);
			output->a_ = alpha;
			break;
		case RenILight::ACCUMULATE:
			output->r_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.r_) >> 20);
			output->g_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.g_) >> 20);
			output->b_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.b_) >> 20);
			break;
	}
}

inline accumulateIllumination
(
	MmxColourS11_4* output,
	RenILight::OutputAction sum,
	RenIFixPtU0_8 intensity,
	const RenIColourFixPtS23_8& colour
)
{
	static const FixPtS11_4 alpha = floatToFixPtS11_4(1);

	// Note: shifting a signed value is implementation dependent.  If it
	// works, do it because the alternative is a much slower division.
	switch (sum)
	{
		case RenILight::INITIALISE:
			output->r_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.r_) >> 12);
			output->g_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.g_) >> 12);
			output->b_ = _STATIC_CAST(FixPtS11_4, (intensity * colour.b_) >> 12);
			output->a_ = alpha;
			break;
		case RenILight::ACCUMULATE:
			output->r_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.r_) >> 12);
			output->g_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.g_) >> 12);
			output->b_ += _STATIC_CAST(FixPtS11_4, (intensity * colour.b_) >> 12);
			break;
	}
}
*/
static void resizeVector(ctl_vector<RenColour>& v, size_t newSize)
{
	PRE(newSize > v.size());

	v.reserve(newSize);
	const size_t extra = newSize - v.size();

	for (int i=extra; i!=0; --i)
		v.push_back(0);

	POST(v.size() == newSize);
}

static void resizeVector(ctl_vector<RenIColourFixPtS15_16>& v, size_t newSize)
{
	PRE(newSize > v.size());

	static RenIColourFixPtS15_16 dummy;

	v.reserve(newSize);
	const size_t extra = newSize - v.size();

	for (int i=extra; i!=0; --i)
		v.push_back(dummy);

	POST(v.size() == newSize);
}

/////////////////////////////////////////////////////////////////////////////
// Non-MMX version.
// virtual
void RenIDirectionalLight::lightVertices
(
	const RenIVertexData& in,
	RenIFloatLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	bool performRangeChecks,
	const MexAlignedBox3d*
) const
{
	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIDirectionalLight*, this)->used(true);

	// Transform the light direction into the vertices' local co-ordinate system.
	MexVec3 dir = direction();
	world.transformInverse(&dir);
	dir *= -1;

	dir.makeUnitVector();
	ASSERT_INFO(dir);
	const RenIVec3FixPtS0_7 fixPtDir = dir;

	// Do the lambertian diffuse calculation for every normal.
	// Put the results into this vector.
	static ctl_vector<RenColour> intensities(200);
	const RenIVertexData::Normals& normals = in.normals();
	RenColour myColour = colour();
	transformColour(&myColour, Ren::DIRECTIONAL);

	// Make sure the intensities vector can hold an element for each normal.
	if (intensities.size() < normals.size())
		resizeVector(intensities, normals.size());

	RenIVertexData::Normals::const_iterator normIt = normals.begin();
	ctl_vector<RenColour>::iterator intIt = intensities.begin();
	while (normIt != normals.end())
	{
		const RenIVec3FixPtS0_7& normal = *normIt;
		const RenIFixPtU0_8 fixPtInt = fixPtDir.dotProductClampZero(normal);

		const float intensity = fixPtInt / 255.0;

		(*intIt).r(intensity * myColour.r());		// S15.16
		(*intIt).g(intensity * myColour.g());
		(*intIt).b(intensity * myColour.b());

		++normIt;
		++intIt;
	}

	// For each vertex apply the results of the corresponding normal calculation.
	float* pR = out->rLamberts();
	float* pG = out->gLamberts();
	float* pB = out->bLamberts();
	bool* pUnderflow = out->underflowFlags();

	// According to the profiler, the ctl_vector::operator[] is taking a bit of
	// time.  If this is the release version, use C-style array indexing instead.
	// This is a real cruft because nothing about the STL standard guarantees
	// that vector elements are in contiguous memory.
	#ifdef NDEBUG
	const RenColour* cruftyIntensities = &(intensities[0]);
	#else
	const ctl_vector<RenColour>& cruftyIntensities = intensities;
	#endif

	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
		ASSERT((*vtxIt).normalIndex < intensities.size(), "Normal indexing error");
		const RenColour& vtxColour = cruftyIntensities[(*vtxIt).normalIndex];

		// First time round the loop, set the RGB components.  On
		// subsequent iterations, accumulate the values.
		accumulateIllumination(pR, pG, pB, sumType, vtxColour);

		// If this is the last light and the sums have reached their
		// final values, then update the maximum values.
		lightingRangeChecks(performRangeChecks, out, pR, pG, pB, pUnderflow);

		++vtxIt;
		++pR;
		++pG;
		++pB;
	}
}

/////////////////////////////////////////////////////////////////////////////
// MMX version.
// virtual

void RenIDirectionalLight::lightVerticesMMX
(
	const RenIVertexData& in,
	RenIFixPtLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	const MexAlignedBox3d*
) const
{
/*	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIDirectionalLight*, this)->used(true);

	// Transform the light direction into the vertices' local co-ordinate system.
	MexVec3 dir = direction();
	world.transformInverse(&dir);
	dir *= -1;

	dir.makeUnitVector();
	ASSERT_INFO(dir);
	const RenIVec3FixPtS0_7 fixPtDir = dir;

	// Do the lambertian diffuse calculation for every normal.
	// Put the results into this vector.
	static ctl_vector<RenIColourFixPtS15_16> intensities(200);
	const RenIVertexData::Normals& normals = in.normals();
	RenColour myColour = colour();
	transformColour(&myColour, Ren::DIRECTIONAL);
	const RenIColourFixPtS23_8 myColourFixPt = myColour;

	// Make sure the intensities vector can hold an element for each normal.
	if (intensities.size() < normals.size())
		resizeVector(intensities, normals.size());

	RenIVertexData::Normals::const_iterator normIt = normals.begin();
	ctl_vector<RenIColourFixPtS15_16>::iterator intIt = intensities.begin();
	while (normIt != normals.end())
	{
		ASSERT_INFO(*normIt);
		const RenIVec3FixPtS0_7& normal = *normIt;
		const RenIFixPtU0_8 fixPtInt = fixPtDir.dotProductClampZero(normal);

		(*intIt).r_ = fixPtInt * myColourFixPt.r_;	// U0_8 * S23_8 = S15_16
		(*intIt).g_ = fixPtInt * myColourFixPt.g_;
		(*intIt).b_ = fixPtInt * myColourFixPt.b_;

		++normIt;
		++intIt;
	}

	// For each vertex apply the results of the corresponding normal calculation.
	MmxColourS11_4*	pLamb = out->lamberts();

	// According to the profiler, the ctl_vector::operator[] is taking a bit of
	// time.  If this is the release version, use C-style array indexing instead.
	// This is a real cruft because nothing about the STL standard guarantees
	// that vector elements are in contiguous memory.
	#ifdef NDEBUG
	const RenIColourFixPtS15_16* cruftyIntensities = &(intensities[0]);
	#else
	const ctl_vector<RenIColourFixPtS15_16>& cruftyIntensities = intensities;
	#endif

	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
		ASSERT((*vtxIt).normalIndex < intensities.size(), "Normal indexing error");
		const RenIColourFixPtS15_16& vtxColour = cruftyIntensities[(*vtxIt).normalIndex];

		// First time round the loop, set the RGB components.  On
		// subsequent iterations, accumulate the values.
		accumulateIllumination(pLamb, sumType, vtxColour);

		++vtxIt;
		++pLamb;
	}*/
}

// virtual
void RenIDirectionalLight::accumulateGeneric(RenColour* pColour) const
{
	PRE(pColour);

	// Use a fixed normal of (0,0,1).  If you work out the dot product of this
	// with the direction nx*dx + ny*dy + nz*dz	the result is always just dz.
	float intensity = -direction().z();

	// The dot product is negative for polygons facing away from the light.
	// The light should *never* substract from the accumulated effect.
	if (intensity < 0)
		intensity = 0;

	RenColour myColour = colour();
	transformColour(&myColour, Ren::DIRECTIONAL);
	myColour *= intensity;
	(*pColour) += myColour;
}

RenIDirectionalLight::RenIDirectionalLight()
{
	direction(MexVec3(1,0,0));
}

// virtual
void RenIDirectionalLight::print(ostream& o) const
{
	o << "Directional light, " << colour() << " dir=" << direction();
}

void perWrite( PerOstream& ostr, const RenIDirectionalLight& light )
{
    const RenILight& base = light;

    ostr << base;
    ostr << light.direction;
}

void perRead( PerIstream& istr, RenIDirectionalLight& light )
{
    RenILight& base = light;

    istr >> base;
    istr >> light.direction;
}

void RenIAttenuatedLight::maxRange(MATHEX_SCALAR r)
{
	PRE(r > 0);
	maxRange_ = r;
	recipMaxRange_ = 1.0 / r;
}

MATHEX_SCALAR RenIAttenuatedLight::maxRange() const
{
	return maxRange_;
}

// Lighting attenuation according to Direct3D's reference page for D3DLIGHT2.
float RenIAttenuatedLight::attenuation(MATHEX_SCALAR rangeToVtx) const
{
	PRE(rangeToVtx >= 0);
	PRE(rangeToVtx <= maxRange_);

	const float normalised = 1 - rangeToVtx * recipMaxRange_;
	ASSERT(normalised >= 0, "Attenuation calculation error.");
	ASSERT(normalised <= 1, "Attenuation calculation error.");

	// If the attenuations add up to 1.0 or less then this result must
	// also be in the range [0,1].  See startFrame method.
	float result = constantAttenuation() +
			 	   linearAttenuation()    * normalised +
			 	   quadraticAttenuation() * normalised * normalised;

	POST(result >= 0);
	POST(result <= 1);
	return result;
}

RenIFixPtU0_8 RenIAttenuatedLight::attenuationFixPt(MATHEX_SCALAR rangeToVtx) const
{
	PRE(rangeToVtx >= 0);
	PRE(rangeToVtx <= maxRange_);

	const float normalised = 1 - rangeToVtx * recipMaxRange_;
	ASSERT(normalised >= 0, "Attenuation calculation error.");
	ASSERT(normalised <= 1, "Attenuation calculation error.");

	// If the attenuations add up to 1.0 or less then this result must
	// also be in the range [0,1].  See startFrame method.
	float result = constantAttenuation() +
			 	   linearAttenuation()    * normalised +
			 	   quadraticAttenuation() * normalised * normalised;

	ASSERT(result >= 0, "Attenuation calculation error.");
	ASSERT(result <= 1, "Attenuation calculation error.");

	return _STATIC_CAST(RenIFixPtU0_8, 255 * result + 0.5);
}

// Calculate the attenuation given the position of a light and a vertex.
float RenIAttenuatedLight::attenuation
(
	MATHEX_SCALAR x,
	MATHEX_SCALAR y,
	MATHEX_SCALAR z,
	const MexPoint3d& pos,
	MATHEX_SCALAR sqrRange
) const
{
    MATHEX_SCALAR dx = x - pos.x();
    MATHEX_SCALAR dy = y - pos.y();
    MATHEX_SCALAR dz = z - pos.z();
	const MATHEX_SCALAR sqrRangeToVtx = dx*dx + dy*dy + dz*dz;

	// If the vertex is completely outside our range then just skip it
	// (unless we have been asked to initialise the output values, in
	// which case they must be reset to zero).
	if (sqrRangeToVtx >= sqrRange)
		return 0;
	else
		return attenuation(sqrt(sqrRangeToVtx));
}

// virtual
void RenIAttenuatedLight::startFrame()
{
	// It would be best to check these when they're set.  However, with an
	// interface which allows each to be set independently, you can't perform
	// the check until all three are set.  TBD: maybe change the interface?
	ASSERT_INFO(maxRange_);
	ASSERT_INFO(constantAttenuation());
	ASSERT_INFO(linearAttenuation());
	ASSERT_INFO(quadraticAttenuation());
	ASSERT_DATA(MATHEX_SCALAR sum = constantAttenuation() + linearAttenuation() + quadraticAttenuation());
	ASSERT(sum >= 0, "Light attenuations must add up to greater than 0.");
	ASSERT(sum <= 1, "Light attenuations must add up to less than 1.");
}

// virtual
void RenIAttenuatedLight::accumulateGeneric(RenColour*) const
{
	// Intentionally has no effect.
}

// static
MATHEX_SCALAR& RenIAttenuatedLight::resultTollerance()
{
	static MATHEX_SCALAR t = 1.0 / 15.0;
	return t;
}

void perWrite( PerOstream& ostr, const RenIAttenuatedLight& light )
{
    const RenILight& base = light;

    ostr << base;
    ostr << light.position;
    ostr << light.constantAttenuation;
    ostr << light.linearAttenuation;
    ostr << light.quadraticAttenuation;
    ostr << light.maxRange_;
    ostr << light.recipMaxRange_;
}

void perRead( PerIstream& istr, RenIAttenuatedLight& light )
{
    RenILight& base = light;

    istr >> base;
    istr >> light.position;
    istr >> light.constantAttenuation;
    istr >> light.linearAttenuation;
    istr >> light.quadraticAttenuation;
    istr >> light.maxRange_;
    istr >> light.recipMaxRange_;
}

/* Lighting attenuation according to Foley & van Dam.  Althought this *does*
   work correctly, it is hard to get a feel for how the parameters affect the
   light.  Thus it is difficult to create a light which does what you want.
   Direct3D's interpretation of the parameters is preferred.
float RenIAttenuatedLight::attenuation(MATHEX_SCALAR rangeToVtxSquared) const
{
	PRE(rangeToVtxSquared >= 0);

	const float rangeToVtx = sqrt(rangeToVtxSquared);
	static const float verySmall = 0.00001;
	float denom = constantAttenuation() +
				  linearAttenuation()    * rangeToVtx +
				  quadraticAttenuation() * rangeToVtxSquared;

	float result;
	if (denom < verySmall)		// Avoid divide by zero.
		result = 1;
	else
		result = 1 / denom;

	if (result > 1)
		result = 1;
	else if (result < 0)
		result = 0;

	result *= multiplier(rangeToVtxSquared);
	return result;
}
*/

/////////////////////////////////////////////////////////////////////////////
RenIPointLight::RenIPointLight(MATHEX_SCALAR r)
{
	PRE(r > 0);
	maxRange(r);
}

// Non-MMX version.
// virtual
void RenIPointLight::lightVertices
(
	const RenIVertexData& in,
	RenIFloatLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	bool performRangeChecks,
	const MexAlignedBox3d*
) const
{
	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIPointLight*, this)->used(true);

	// Transform the light position into the vertices' local co-ordinate system.
	MexPoint3d pos = position();
	world.transformInverse(&pos);

	const MATHEX_SCALAR sqrRange = maxRange() * maxRange();

	float* pR = out->rLamberts();
	float* pG = out->gLamberts();
	float* pB = out->bLamberts();
	bool* pUnderflow = out->underflowFlags();

	RenColour myColour = colour();
	transformColour(&myColour, Ren::POINT);

	// Note: it is not worth optimising this for unique vertex normals because
	// the light direction is unique for each vertex, so the dot product result
	// will also be unique and shared normals don't make any difference.
	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
        MATHEX_SCALAR dx = pos.x() - (*vtxIt).x;
        MATHEX_SCALAR dy = pos.y() - (*vtxIt).y;
        MATHEX_SCALAR dz = pos.z() - (*vtxIt).z;
		const MATHEX_SCALAR sqrRangeToVtx = dx*dx + dy*dy + dz*dz;

		// If the vertex is completely outside our range then just skip it
		// (unless we have been asked to initialise the output values, in
		// which case they must be reset to zero).
		if (sqrRangeToVtx >= sqrRange)
		{
			if (sumType == INITIALISE)
			{
				*pR = 0;
				*pG = 0;
				*pB = 0;
			}
		}
		else
		{
			// Rather than call dir.makeUnitVector(), avoid a squrare root and
			// divide by the range to the vertex because we need that separately
			// for the attenuation calculation.
			const float rangeToVtx = sqrt(sqrRangeToVtx);
			const float recipRange = 1 / rangeToVtx;
			const RenIVec3FixPtS0_7 fixPtDir(recipRange * dx, recipRange * dy, recipRange * dz);

			const RenIVec3FixPtS0_7& normal = in.normal(vtxIt);
			const RenIFixPtU0_8 fixPtInt = fixPtDir.dotProductClampZero(normal);

			// TBD: convert the attenuation calculations to fixed-point?
			const float intensity = fixPtInt / 255.0;

			const float att = attenuation(rangeToVtx);
			accumulateIllumination(pR, pG, pB, sumType, att * intensity, myColour);
		}

		// If this is the last light and the sums have reached their
		// final values, then update the maximum values.
		lightingRangeChecks(performRangeChecks, out, pR, pG, pB, pUnderflow);

		++vtxIt;
		++pR;
		++pG;
		++pB;
	}
}

// MMX version.
// virtual

void RenIPointLight::lightVerticesMMX
(
	const RenIVertexData& in,
	RenIFixPtLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	const MexAlignedBox3d*
) const
{
    /*
	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIPointLight*, this)->used(true);

	// Transform the light position into the vertices' local co-ordinate system.
	MexPoint3d pos = position();
	world.transformInverse(&pos);

	RenColour myColour = colour();
	transformColour(&myColour, Ren::POINT);
	const RenIColourFixPtS23_8 myColourFixPt = myColour;

	const MATHEX_SCALAR sqrRange = maxRange() * maxRange();
	MmxColourS11_4*	pLamb = out->lamberts();

	// Note: it is not worth optimising this for unique vertex normals because
	// the light direction is unique for each vertex, so the dot product result
	// will also be unique and shared normals don't make any difference.
	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
        MATHEX_SCALAR dx = pos.x() - (*vtxIt).x;
        MATHEX_SCALAR dy = pos.y() - (*vtxIt).y;
        MATHEX_SCALAR dz = pos.z() - (*vtxIt).z;
		const MATHEX_SCALAR sqrRangeToVtx = dx*dx + dy*dy + dz*dz;

		// If the vertex is completely outside our range then just skip it
		// (unless we have been asked to initialise the output values, in
		// which case they must be reset to zero).
		if (sqrRangeToVtx >= sqrRange)
		{
			if (sumType == INITIALISE)
			{
				static const MmxColourS11_4 zero = { 0,0,0,1 };
				*pLamb = zero;
			}
		}
		else
		{
			// Rather than call dir.makeUnitVector(), avoid a squrare root and
			// divide by the range to the vertex because we need that separately
			// for the attenuation calculation.
			const float rangeToVtx = sqrt(sqrRangeToVtx);
			const float recipRange = 1 / rangeToVtx;
			const RenIVec3FixPtS0_7 fixPtDir(recipRange * dx, recipRange * dy, recipRange * dz);

			const RenIVec3FixPtS0_7& normal = in.normal(vtxIt);
			const RenIFixPtU0_8 fixPtInt = fixPtDir.dotProductClampZero(normal);

			// NB: implicit integral promotion should ensure enough bits of
			// precision for the result.
			const RenIFixPtU0_8 fixPtAtt = attenuationFixPt(rangeToVtx);
			const RenIFixPtS15_16 attenuatedInt = fixPtInt * fixPtAtt;

			accumulateIllumination(pLamb, sumType, attenuatedInt, myColourFixPt);
		}

		++vtxIt;
		++pLamb;
	}*/
}

// virtual
void RenIPointLight::print(ostream& o) const
{
	o << "Point light, " << colour() << " pos=" << position();
	o << " atten: (c=" << constantAttenuation();
	o << ",l=" << linearAttenuation();
	o << ",q=" << quadraticAttenuation();
	o << ") range=" << maxRange();
}

RenIPointLight::RenIPointLight( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const RenIPointLight& light )
{
    const RenIAttenuatedLight& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, RenIPointLight& light )
{
    RenIAttenuatedLight& base = light;

    istr >> base;
}

/////////////////////////////////////////////////////////////////////////////
RenIUniformLight::RenIUniformLight(MATHEX_SCALAR r)
{
	PRE(r > 0);
	maxRange(r);
}

// Although this actually works, it's not worth using.  It saves a fraction of
// a percent of the CPU time, at the expense of some visual quality.  Despite
// it not being used, the framework of bounding volumes is still in place in
// case this, or any other bounding volume optimisations, could be useful.
bool RenIUniformLight::boundingVolumeUniformlyLit
(
	const MexAlignedBox3d* pVolume,
	const MexPoint3d& pos,
	MATHEX_SCALAR sqrRange,
	float* pResult
) const
{
	PRE(pVolume);
	PRE(pResult);

	// What is a reasonable approximation of a uniformly lit bounding volume?
	// If we only test the two corners, then the result could be wrong if the
	// light is bang in the middle of the volume.  Hence, this tests the mid-
	// point too.  Testing these three points could still be wrong in
	// pathological cases (because they're colinear).
	const MexPoint3d& c1 = pVolume->minCorner();
	const MexPoint3d& c2 = pVolume->maxCorner();

	MATHEX_SCALAR midX = 0.5 * (c1.x() + c2.x());
	MATHEX_SCALAR midY = 0.5 * (c1.y() + c2.y());
	MATHEX_SCALAR midZ = 0.5 * (c1.z() + c2.z());

	const MATHEX_SCALAR int1 = attenuation(midX,   midY,   midZ,   pos, sqrRange);
	const MATHEX_SCALAR int2 = attenuation(c1.x(), c1.y(), c1.z(), pos, sqrRange);
	const MATHEX_SCALAR int3 = attenuation(c2.x(), c2.y(), c2.z(), pos, sqrRange);

	// Use the mid-point's calculation to represent the whole.
	*pResult = int1;
	return fabs(int1 - int2) < resultTollerance() && fabs(int2 - int3) < resultTollerance();
}

// virtual
void RenIUniformLight::lightVertices
(
	const RenIVertexData& in,
	RenIFloatLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	bool performRangeChecks,
	const MexAlignedBox3d*
) const
{
	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIUniformLight*, this)->used(true);

	// Transform the light position into the vertices' local co-ordinate system.
	MexPoint3d pos = position();
	world.transformInverse(&pos);

	RenColour myColour = colour();
	transformColour(&myColour, Ren::UNIFORM);

	const MATHEX_SCALAR sqrRange = maxRange() * maxRange();

	float* pR = out->rLamberts();
	float* pG = out->gLamberts();
	float* pB = out->bLamberts();
	bool* pUnderflow = out->underflowFlags();

	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
        MATHEX_SCALAR dx = (*vtxIt).x - pos.x();
        MATHEX_SCALAR dy = (*vtxIt).y - pos.y();
        MATHEX_SCALAR dz = (*vtxIt).z - pos.z();
		const MATHEX_SCALAR sqrRangeToVtx = dx*dx + dy*dy + dz*dz;

		// If the vertex is completely outside our range then just skip it
		// (unless we have been asked to initialise the output values, in
		// which case they must be reset to zero).
		if (sqrRangeToVtx >= sqrRange)
		{
			if (sumType == INITIALISE)
			{
				*pR = 0;
				*pG = 0;
				*pB = 0;
			}
		}
		else
		{
			const float intensity = attenuation(sqrt(sqrRangeToVtx));
			accumulateIllumination(pR, pG, pB, sumType, intensity, myColour);
		}

		// If this is the last light and the sums have reached their
		// final values, then update the maximum values.
		lightingRangeChecks(performRangeChecks, out, pR, pG, pB, pUnderflow);

		++vtxIt;
		++pR;
		++pG;
		++pB;
	}
}

// virtual
void RenIUniformLight::lightVerticesMMX
(
	const RenIVertexData& in,
	RenIFixPtLightingBuffer*  out,
	const MexTransform3d& world,
	OutputAction sumType,
	const MexAlignedBox3d*
) const
{
    /*
	PRE(on());

	// TBD: could be removed from the production version.
	_CONST_CAST(RenIUniformLight*, this)->used(true);

	// Transform the light position into the vertices' local co-ordinate system.
	MexPoint3d pos = position();
	world.transformInverse(&pos);

	RenColour myColour = colour();
	transformColour(&myColour, Ren::UNIFORM);
	const RenIColourFixPtS23_8 myColourFixPt = myColour;

	const MATHEX_SCALAR sqrRange = maxRange() * maxRange();
	MmxColourS11_4*	pLamb = out->lamberts();

	RenIVertexData::const_iterator vtxIt = in.begin();
	while (vtxIt != in.end())
	{
        MATHEX_SCALAR dx = (*vtxIt).x - pos.x();
        MATHEX_SCALAR dy = (*vtxIt).y - pos.y();
        MATHEX_SCALAR dz = (*vtxIt).z - pos.z();
		const MATHEX_SCALAR sqrRangeToVtx = dx*dx + dy*dy + dz*dz;

		// If the vertex is completely outside our range then just skip it
		// (unless we have been asked to initialise the output values, in
		// which case they must be reset to zero).
		if (sqrRangeToVtx >= sqrRange)
		{
			if (sumType == INITIALISE)
			{
				static const MmxColourS11_4 zero = { 0,0,0,1 };
				*pLamb = zero;
			}
		}
		else
		{
			const RenIFixPtU0_8 fixPtAtt = attenuationFixPt(sqrt(sqrRangeToVtx));
			accumulateIllumination(pLamb, sumType, fixPtAtt, myColourFixPt);
		}

		++vtxIt;
		++pLamb;
	}
	*/
};

// virtual
void RenIUniformLight::print(ostream& o) const
{
	o << "Uniform light, " << colour() << " pos=" << position();
	o << " atten: (c=" << constantAttenuation();
	o << ",l=" << linearAttenuation();
	o << ",q=" << quadraticAttenuation();
	o << ") range=" << maxRange();
}

RenIUniformLight::RenIUniformLight( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const RenIUniformLight& light )
{
    const RenIAttenuatedLight& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, RenIUniformLight& light )
{
    RenIAttenuatedLight& base = light;

    istr >> base;
}

/////////////////////////////////////////////////////////////////////////////
// Non-MMX version.
// virtual
void RenIZeroLight::lightVertices
(
	const RenIVertexData& in,
	RenIFloatLightingBuffer*  out,
	const MexTransform3d&,
	OutputAction sumType,
	bool performRangeChecks,
	const MexAlignedBox3d*
) const
{
	// TBD: could be removed from the production version.
	_CONST_CAST(RenIZeroLight*, this)->used(true);

	// If sumType is accumulate, then we don't have to do anything.
	if (sumType == INITIALISE)
	{
		float* pR = out->rLamberts();
		float* pG = out->gLamberts();
		float* pB = out->bLamberts();
		bool* pUnderflow = out->underflowFlags();

		for (int i=in.size(); i!=0; --i)
		{
			*pR = 0;
			*pG = 0;
			*pB = 0;

			// If this is the last light and the sums have reached their
			// final values, then update the maximum values.
			lightingRangeChecks(performRangeChecks, out, pR, pG, pB, pUnderflow);

			++pR;
			++pG;
			++pB;
		}
	}
}

// MMX version.
// virtual

void RenIZeroLight::lightVerticesMMX
(
	const RenIVertexData& in,
	RenIFixPtLightingBuffer*  out,
	const MexTransform3d&,
	OutputAction sumType,
	const MexAlignedBox3d*
) const
{
    /*
	// TBD: could be removed from the production version.
	_CONST_CAST(RenIZeroLight*, this)->used(true);

	// If sumType is accumulate, then we don't have to do anything.
	if (sumType == INITIALISE)
	{
		MmxColourS11_4*	pLamb = out->lamberts();
		static const FixPtS11_4 alpha = floatToFixPtS11_4(1);
		static const FixPtS11_4 zero  = floatToFixPtS11_4(0);

		for (int i=in.size(); i!=0; --i)
		{
			pLamb->r_ = zero;
			pLamb->g_ = zero;
			pLamb->b_ = zero;
			pLamb->a_ = alpha;
			++pLamb;
		}
	}*/
}

// virtual
void RenIZeroLight::accumulateGeneric(RenColour*) const
{
	// Intentionally has no effect.
}

// virtual
void RenIZeroLight::print(ostream& o) const
{
	o << "Zero light";
}

void perWrite( PerOstream& ostr, const RenIZeroLight& light )
{
    const RenILight& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, RenIZeroLight& light )
{
    RenILight& base = light;

    istr >> base;
}

/* End INLIGHT.CPP **************************************************/
