/*
 * S H A D O W . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/shadow.hpp"

#include "world4d/light.hpp"
#include "world4d/generic.hpp"

#include "world4d/internal/complexi.hpp"

#include "render/hsv.hpp"
#include "render/device.hpp"
#include "render/matmap.hpp"
#include "render/colour.hpp"

#include "system/pathname.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"

const W4dDirectionalLight*	W4dShadow::light_ = NULL;
MexVec3			W4dShadow::globalLightDir_(0,0,-1);
MexRadians		W4dShadow::minElevation_ = MexDegrees(45);
MATHEX_SCALAR	W4dShadow::shadowInt_ = 0.5;
MATHEX_SCALAR	W4dShadow::minInt_    = 0.2;
MATHEX_SCALAR	W4dShadow::maxInt_    = 0.75;
MATHEX_SCALAR	W4dShadowProjected2d::shadowPlaneZ_ = 0;

PER_DEFINE_PERSISTENT_ABSTRACT( W4dShadow );
PER_DEFINE_PERSISTENT( W4dShadowProjected2d );
PER_DEFINE_PERSISTENT( W4dShadowFixed );

W4dShadow::W4dShadow(W4dEntity* pParent, const W4dTransform3d& localXform):
	W4dEntity(pParent, localXform, NOT_SOLID)
{
    TEST_INVARIANT;
	doNotLight(true);
}

W4dShadow::W4dShadow(const W4dShadow& copyMe, W4dEntity* pParent):
	W4dEntity(copyMe, pParent, copyMe.localTransform())
{
	doNotLight(true);
}

W4dShadow::~W4dShadow()
{
    TEST_INVARIANT;
}

// static
RenMaterial& W4dShadow::shadowMaterial()
{
	static RenMaterial m;
	static bool first = true;

	// This material is non-sharable by the manager, however, it is shared
	// between shadows by virtue of being applied to all shadows here.
	// Hence, by modifying it each frame we modify every shadow.
	if (first)
	{
		first = false;
		m.makeNonSharable();
		m.diffuse(RenColour(0,0,0, W4dShadow::shadowInt_));

		// Ensure that shadows will always be rendered last relative to
		// other alpha polygons.
		m.absoluteAlphaPriority(true);
		m.alphaPriority(-10000);

		// For hardware which supports zbias, also mark the shadows as
		// coplanar.  This sorts shadows against the opaque terrain.
		// The value of 17 is set to match the way in which the Machines
		// models are build.  (It shouldn't be hardcoded here.)
		m.interMeshCoplanar(true);
		m.coplanarPriority(17);
	}

	return m;
}

void W4dShadow::applyShadowMaterial(W4dEntity* entity)
{
	// Change all the materials in the given entity to be the shadow material.
    RenMaterialSet    materials;
    entity->addMaterials(&materials);

    RenMaterialMap  newMaterialMap;
    for( RenMaterialSet::iterator i = materials.begin(); i != materials.end(); ++i )
    {
		const RenMaterial newMat = shadowMaterial();
        newMaterialMap.insert(*i, newMat);
    }

    entity->materialMap(newMaterialMap);
}

// static
void W4dShadow::setLight(const W4dDirectionalLight* light)
{
	light_ = light;

	// Ensure that the light does not dip below min elevation.
	// Note the sense is *from* the light to the illuminated entity.
	globalLightDir_ = light_->direction();
	globalLightDir_.makeUnitVector();

	// Set the shadow strength to the light strength minus the ambient strength.
	const MATHEX_SCALAR contrastFudge = 1.0;
	const RenColourHSV lightCol(light->colour());
	const RenColourHSV amb(RenDevice::current()->ambient());
	shadowInt_ = contrastFudge * (lightCol.value() - amb.value());

	if (shadowInt_ < minInt_)
		shadowInt_ = minInt_;
	else if (shadowInt_ > maxInt_)
		shadowInt_ = maxInt_;

	const RenColour newCol(0,0,0, shadowInt_);
	shadowMaterial().diffuse(newCol);
}

/* Old version: computed diffuse reflection from a horizontal plane and
 * used that as the basis for shadow strength.
// static
void W4dShadow::setLight(const W4dDirectionalLight* light)
{
	light_ = light;

	// Ensure that the light does not dip below min elevation.
	// Note the sense is *from* the light to the illuminated entity.
	globalLightDir_ = light_->direction();
	globalLightDir_.makeUnitVector();

	// Calculate lambertian diffuse illumination of the given light on the
	// z=0 plane.  This should be close enough to the model used by DirectX.
	MexVec3 actualDir = globalLightDir_;
	actualDir *= -1;
	const RenColourHSV lightCol(light->colour());
	const MexVec3 up(0,0,1);

	// This equation ought to include the diffuse reflectivity of the material,
	// i.e., RenMaterial::diffuse(), but we don't have the surface available
	// to us (indeed there may be several differnt ones) so we just have assume
	// a reasonable value.
	const MATHEX_SCALAR diffuse = 1;
	const MATHEX_SCALAR lambertianInt = diffuse * lightCol.value() * actualDir.dotProduct(up);

	// Set the shadow intenstiy to the diffuse illumination minus the abmient,
	// times a fudge factor for slightly more contrast, plus the minimum
	// intensity.  This will give minInt_ when there is only ambient light.
	const MATHEX_SCALAR contrastFudge = 1.0;
	const RenColourHSV amb(RenDevice::current()->ambient());
	shadowInt_ = contrastFudge * (lightCol.value() - amb.value()) + minInt_;

	if (shadowInt_ < minInt_)
		shadowInt_ = minInt_;
	else if (shadowInt_ > maxInt_)
		shadowInt_ = maxInt_;

	const RenColour newCol(0,0,0, shadowInt_);
	shadowMaterial().diffuse(newCol);
}
*/

const W4dDirectionalLight* W4dShadow::light()
{
	return light_;
}

const MexVec3& W4dShadow::globalLightDirection()
{
	return globalLightDir_;
}

MATHEX_SCALAR W4dShadow::shadowIntensity()
{
	return shadowInt_;
}

void W4dShadow::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

///////////////////////////////////////////////////////////////////////////////
W4dShadowProjected2d::W4dShadowProjected2d(W4dEntity* pParent, const W4dTransform3d& localXform):
	W4dShadow(pParent, localXform),
	shadowEntity_(_NEW(W4dGeneric(this, MexTransform3d(), NOT_SOLID)))
{
}

W4dShadowProjected2d::W4dShadowProjected2d(const W4dShadowProjected2d& copyMe, W4dEntity* pParent):
	W4dShadow(copyMe, pParent),
	shadowEntity_(_NEW(W4dGeneric(*(copyMe.shadowEntity_), this, copyMe.shadowEntity_->localTransform())))
{
}

W4dShadowProjected2d::~W4dShadowProjected2d()
{
}

// virtual
void W4dShadowProjected2d::copy(W4dEntity* parent)
{
	_NEW(W4dShadowProjected2d(*this, parent));
}

// virtual
void W4dShadowProjected2d::cast(MATHEX_SCALAR zs)
{
	PRE(light());		// W4dShadow::setLight must have been called.

	// If the shadow intenstity is zero or less, i.e., if the model is
	// lit from below, then no shadow is needed.
	shadowEntity_->visible(shadowIntensity() > 0);

	if (!shadowEntity_->visible())
		return;

	// Project the light direction into this object's co-ordinate system.
	MexVec3 lightDir = globalLightDirection();
	MexTransform3d invGlob;
	globalTransform().invert(&invGlob);
	invGlob.transform(&lightDir);

	// What height is the shadow above the given plane?  Assume that the shadow
	// is in the z=0 plane of it's local co-ordinate system!!
	const MATHEX_SCALAR zoffset = localTransform().position().z() - zs;

	// If the shadow mesh is below the projection plane it cannot cast a shadow
	// so make this mesh invisible.
	shadowEntity_->visible(shadowEntity_->visible() & (zoffset >= 0));

	if (!shadowEntity_->visible())
		return;

	// We can't project the shadow if the light direction is horizontal.
	// If so, just don't bother reprojecting the shadow and keep the old
	// version.  It may look wrong, but probably only for a few frames.
	if (fabs(lightDir.z()) > MexEpsilon::instance())
	{
		const MATHEX_SCALAR scale = zoffset / lightDir.z();
		lightDir *= -scale;

		// This is a bit of a hack.  The maximum value is really related to the
		// size of the objects in the world.  This should work for Machines.
		// TBD: give clients control over the maximum.
		static const MATHEX_SCALAR offsetMax = 2;
		const MATHEX_SCALAR offsetSize = lightDir.modulus();
		if (offsetSize > offsetMax)
			lightDir *= (offsetMax / offsetSize);

		const MexPoint3d offset(lightDir.x(), lightDir.y(), -zoffset);
		shadowEntity_->localTransform(offset);
	}
}

bool W4dShadowProjected2d::loadLODFile(const SysPathName& filename)
{
	bool retval = shadowEntity_->loadLODFile(filename);

	if (retval)
		applyShadowMaterial(shadowEntity_);

	return retval;
}

bool W4dShadowProjected2d::loadSingleMesh(const SysPathName& filename, const string& meshName, double scale)
{
	bool retval = shadowEntity_->loadSingleMesh(filename, meshName, scale);

	if (retval)
		applyShadowMaterial(shadowEntity_);

	return retval;
}

// virtual
W4dEntity::RenderType W4dShadowProjected2d::render
(
	const W4dCamera&  cam,
	W4dComplexity& comp
)
{
	cast(shadowPlaneZ_);
	return W4dEntity::render(cam, comp);
}

// static
void W4dShadowProjected2d::shadowPlaneZ(MATHEX_SCALAR zs)
{
	shadowPlaneZ_ = zs;
}

///////////////////////////////////////////////////////////////////////////////
W4dShadowFixed::W4dShadowFixed(W4dEntity* pParent, const W4dTransform3d& localXform):
	W4dShadow(pParent, localXform)
{
}

W4dShadowFixed::W4dShadowFixed(const W4dShadowFixed& copyMe, W4dEntity* pParent):
	W4dShadow(copyMe, pParent)
{
}

W4dShadowFixed::~W4dShadowFixed()
{
}

// virtual
void W4dShadowFixed::copy(W4dEntity* parent)
{
	_NEW(W4dShadowFixed(*this, parent));
}

// virtual
void W4dShadowFixed::cast(MATHEX_SCALAR)
{
}

bool W4dShadowFixed::loadLODFile(const SysPathName& filename)
{
	readLODFile(filename);

    //  Use the last part of the name without any extension
    SysPathName withoutExt( filename );
    withoutExt.extension( "" );

	name(withoutExt.components().back());

	applyShadowMaterial(this);
	return true;
}

// virtual
bool W4dShadow::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const
{
    //Just check against bounding volume
    return defaultIntersectsLine( line, pDistance, accuracy );
}

W4dShadow::W4dShadow( PerConstructor con )
: W4dEntity( con )
{
}

void perWrite( PerOstream& ostr, const W4dShadow& shadow )
{
    const W4dEntity& base = shadow;

    ostr << base;
}

void perRead( PerIstream& istr, W4dShadow& shadow )
{
    W4dEntity& base = shadow;

    istr >> base;
}

W4dShadowProjected2d::W4dShadowProjected2d( PerConstructor con )
:	W4dShadow( con ),
	shadowEntity_(NULL)
{
}

void perWrite( PerOstream& ostr, const W4dShadowProjected2d& shadow )
{
    const W4dShadow& base = shadow;

    ostr << base;
}


void perRead( PerIstream& istr, W4dShadowProjected2d& shadow )
{
    W4dShadow& base = shadow;

    istr >> base;

    PER_READ_CONST_OBJECT( istr, W4dGeneric*, shadow.shadowEntity_ );
}

W4dShadowFixed::W4dShadowFixed( PerConstructor con )
: W4dShadow( con )
{
}

void perWrite( PerOstream& ostr, const W4dShadowFixed& shadow )
{
    const W4dShadow& base = shadow;

    ostr << base;
}

void perRead( PerIstream& istr, W4dShadowFixed& shadow )
{
    W4dShadow& base = shadow;

    istr >> base;
}

/* End SHADOW.CPP ***************************************************/
