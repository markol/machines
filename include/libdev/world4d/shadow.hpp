/*
 * S H A D O W . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _WORLD4D_SHADOW_HPP
#define _WORLD4D_SHADOW_HPP

#include "base/persist.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/angle.hpp"
#include "world4d/entity.hpp"

class W4dDirectionalLight;
class W4dGeneric;

// Abstract base class for all types of dynamically projected shadow.  Fixed
// shadows do not require a class because they are represented using normal,
// predefined meshes.  Note that this class does not determine *when* the
// shadow is projected.  Hence, a shadow could be projected once at start-
// up and then left in the same position.  Alternatively, it could be
// projected every frame to track a fast moving light source.
class W4dShadow : public W4dEntity
{
public:
    W4dShadow(W4dEntity* pParent, const W4dTransform3d& localXform);
    W4dShadow(const W4dShadow& copyMe, W4dEntity* pParent);
    ~W4dShadow();

	// Cast this shadow onto the z=zs plane.
	// PRE(light_);		a light source must be set
	virtual void cast(MATHEX_SCALAR zs) = 0;

	// This represents an optimisation, all shadows share a common
	// light source.
	static void setLight(const W4dDirectionalLight*);

	virtual void copy(W4dEntity* parent) = 0;

	// Lights with low elevations produce very long shadows which are not
	// desirable.  (Indeed, elevations of exactly zero would produce infinitely
	// long shadows, or more likely, an exception.)  Thus, this value sets a
	// minimum elevation.  If a light source is lower than this, the shadows
	// will be projected as if they were at this elevation.  Default = 45deg.
	static void minLightElevation(MexRadians);

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

	static MATHEX_SCALAR shadowIntensity();
	static void minIntensity(MATHEX_SCALAR);	// default = 0.1
	static void maxIntensity(MATHEX_SCALAR);	// default = 0.6

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dShadow );
    PER_FRIEND_READ_WRITE( W4dShadow );

protected:
	static const W4dDirectionalLight* light();
	static const MexVec3& globalLightDirection();

	// Apply the shadow material to the given entity.
	void applyShadowMaterial(W4dEntity*);
	static RenMaterial& shadowMaterial();

private:
	static const W4dDirectionalLight* light_;
	static MexVec3			globalLightDir_;
	static MexRadians		minElevation_;
	static MATHEX_SCALAR	shadowInt_, minInt_, maxInt_;

    // Operations deliberately revoked
    W4dShadow( const W4dShadow& );
    W4dShadow& operator =( const W4dShadow& );
    bool operator ==( const W4dShadow& );
};

PER_DECLARE_PERSISTENT( W4dShadow );

// A shadow which has a flat, 2D mesh built in some plane z=z1.  When rendered,
// the shadow is projected along the light direction into another plane z=z2.
class W4dShadowProjected2d : public W4dShadow
{
public:
    W4dShadowProjected2d(W4dEntity* pParent, const W4dTransform3d& localXform);
    W4dShadowProjected2d(const W4dShadowProjected2d& copyMe, W4dEntity* pParent);
    ~W4dShadowProjected2d();

	virtual void cast(MATHEX_SCALAR zs);
	virtual void copy(W4dEntity* parent);
    bool loadLODFile(const SysPathName& filename);
    bool loadSingleMesh(const SysPathName& filename, const string& meshName, double scale = 1.0);

	// Set the projection plane for all shadows to be z=zs.  Default=0.
	static void shadowPlaneZ(MATHEX_SCALAR zs);

    PER_MEMBER_PERSISTENT( W4dShadowProjected2d );
    PER_FRIEND_READ_WRITE( W4dShadowProjected2d );

private:
	// This W4dEntity method is overriden in order to cast the shadow whenever
	// it is drawn.
    virtual RenderType render( const W4dCamera&, W4dComplexity& );

	W4dGeneric* const		shadowEntity_;
	static MATHEX_SCALAR	shadowPlaneZ_;
};

PER_DECLARE_PERSISTENT( W4dShadowProjected2d );

// A shadow whose geometry is loaded from a file and left unchanged.  The
// shadow material is overriden to use the computed shadow intensity.
class W4dShadowFixed : public W4dShadow
{
public:
    W4dShadowFixed(W4dEntity* pParent, const W4dTransform3d& localXform);
    W4dShadowFixed(const W4dShadowFixed& copyMe, W4dEntity* pParent);
    ~W4dShadowFixed();

	virtual void cast(MATHEX_SCALAR zs);
	virtual void copy(W4dEntity* parent);
    bool loadLODFile(const SysPathName& filename);

    PER_MEMBER_PERSISTENT( W4dShadowFixed );
    PER_FRIEND_READ_WRITE( W4dShadowFixed );
};

PER_DECLARE_PERSISTENT( W4dShadowFixed );

#endif

/* End SHADOW.HPP ***************************************************/
