/*
 * L I G H T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _W4D_LIGHT_HPP
#define _W4D_LIGHT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"
#include "phys/phys.hpp"
#include "utility/property.hpp"

class RenLight;
class RenDirectionalLight;
class RenPointLight;
class RenUniformLight;
class RenColour;
class MexPoint3d;
class MexVec3;
class MexSphere3d;
class W4dLightImpl;
class PhysScalarPlan;
template <class T> class ctl_pvector;
template <class T> class CtlCountedPtr;
typedef CtlCountedPtr< PhysScalarPlan > PhysScalarPlanPtr;

// A W4dEntity which represents a light source, illuminating the scene.
class W4dLight : public W4dEntity
{
public:
    virtual ~W4dLight();

	enum Scope : unsigned char {LOCAL, GLOBAL, DYNAMIC};
	UtlProperty<Scope> scope;

	bool isLocal() const		{ return scope() == LOCAL;   }
	bool isGlobal() const		{ return scope() == GLOBAL;  }
	bool isDynamic() const		{ return scope() == DYNAMIC; }

	void colour(const RenColour&);		// PRE(!RenDevice::current()->rendering());
	const RenColour& colour() const;

	// On vs. enabled: on is a physical "is this light switched on" state which
	// clients can change to modify the lighting conditions.  The enabled state
	// is used internally for implementing local light behaviour.
	void turnOn();
	void turnOff();
	bool isOn() const;

	// Remove/add an entity illuminated by this light.  Clear the list of
	// illumination targets by calling illuminateNone.
	void illuminate(W4dEntity* pEntityItem);		// PRE(!isGlobal());
	void dontIlluminate(W4dEntity* pEntityItem);	// PRE(!isGlobal());
	void illuminateNone();							// PRE(!isGlobal());

	// W4dEntity::visible only returns the value for the entity itself.  For
	// lights we often need to check the visibility of it's parents.  This goes
	// up thru the tree checking all the flags until it comes to the root.
	bool hierarchicallyVisible() const;

	// Not all lights have bounding spheres.  The scene manager must be able
	// to tell the difference.  Ugh!
	bool hasBoundingSphere() const;
	const MexSphere3d& boundingSphere() const;		// PRE(hasBoundingSphere()); PRE(isOn());

    // Inherited from W4dEntity -- always returns false for lights.
    virtual bool intersectsLine(const MexLine3d&, MATHEX_SCALAR*, Accuracy) const;

    void intensityPlan( const PhysScalarPlanPtr& planPtr, const PhysAbsoluteTime& startTime, uint nRepetations = 0);
    void saveUnintensifiedColour();
    bool unintensifiedColourSaved() const;
    bool hasIntensityPlan() const;

	// Will return true if this light actually lit some vertices.
	bool wasUsedThisFrame() const;

	void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dLight );
    PER_FRIEND_READ_WRITE( W4dLight );

protected:
	// NB: the base class *owns* the render light and will delete it.
    W4dLight(W4dEntity* pParent, const W4dTransform3d& localXform, RenLight*, bool hasBoundingSphere);
	RenLight* pRenLight();

	void boundingSphere(const MexPoint3d&, MATHEX_SCALAR radius);

private:
	friend class W4dLocalLightList;
	friend class W4dLightImpl;
	friend class W4dSceneManager;

	const W4dLightImpl& impl() const;
	W4dLightImpl& impl();

	void enable();
	void disable();
	bool enabled() const;

	// The scene manager uses this combination of values heavily during
	// culling.  They're bundled together as an efficiency.
	bool isDynamicEnabledOn() const;

	void dynamicDisable(bool);
	bool dynamicDisable() const;

	// For global lights, this must be called once a frame by the scene
	// manager for a change in the light's position or intensity plan to
	// have any effect.  It will be called automatically if a local light
	// is turned on.
	void lazyUpdate();

    virtual void updatePosition() = 0;
    virtual void updateSphere() = 0;
	void updatePlans();

	void updateDynamicOn();

	W4dLightImpl* pImpl_;

    // Operations deliberately revoked
    W4dLight( const W4dLight& );
    W4dLight& operator =( const W4dLight& );
    bool operator ==( const W4dLight& );
};

// A concrete light which illuminates all objects with light rays from the same
// direction.  Such a light is effectively at infinity and is good for
// modelling light sources like the sun.
class W4dDirectionalLight : public W4dLight
{
public:
	W4dDirectionalLight(W4dEntity* pParent, const MexVec3& direction);

	// Create a world4d light given an existing render equivalent.  This class
	// becomes responsible for deleting the render light.
	W4dDirectionalLight(W4dEntity* pParent, RenDirectionalLight* l);	// PRE(l);

	const MexVec3& direction() const;

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dDirectionalLight);
    PER_FRIEND_READ_WRITE(W4dDirectionalLight);

private:
    virtual void updatePosition();
    virtual void updateSphere();

	RenDirectionalLight* pDirLight_;
};

// A concrete light which illuminates objects with light emanating for a
// point in space.  More expensive, but good for modelling small, local lights
// such as street lamps or explosions.
class W4dPointLight : public W4dLight
{
public:
	W4dPointLight(W4dEntity* pParent, const MexVec3& direction, MATHEX_SCALAR range);

	// Create a world4d light given an existing render equivalent.  This class
	// becomes responsible for deleting the render light.
	W4dPointLight(W4dEntity* pParent, RenPointLight* l);	// PRE(l);

	MATHEX_SCALAR maxRange() const;
	MATHEX_SCALAR constantAttenuation() const;
	MATHEX_SCALAR linearAttenuation() const;
	MATHEX_SCALAR quadraticAttenuation() const;

	void maxRange(MATHEX_SCALAR r);				// PRE(r > 0); PRE(!RenDevice::current()->rendering());
	void constantAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering());
	void linearAttenuation(MATHEX_SCALAR);		// PRE(!RenDevice::current()->rendering());
	void quadraticAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering());

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dPointLight);
    PER_FRIEND_READ_WRITE(W4dPointLight);

private:
    virtual void updatePosition();
    virtual void updateSphere();

	RenPointLight* pPtLight_;
};

// This is a rather odd light.  Vertices are illuminated as a function of
// *range only*, i.e., the vertex normals are ignored.  Put another way: it
// is an ambient source with a range of effect.  Although it has absolutely
// no basis in real physics, it may be useful for not-very-critical effects
// such as explosions.  It is, of course, a squillion times faster than a
// point light.
class W4dUniformLight : public W4dLight
{
public:
	W4dUniformLight(W4dEntity* pParent, const MexVec3& direction, MATHEX_SCALAR range);

	// Create a world4d light given an existing render equivalent.  This class
	// becomes responsible for deleting the render light.
	W4dUniformLight(W4dEntity* pParent, RenUniformLight* l);	// PRE(l);

	MATHEX_SCALAR maxRange() const;
	MATHEX_SCALAR constantAttenuation() const;
	MATHEX_SCALAR linearAttenuation() const;
	MATHEX_SCALAR quadraticAttenuation() const;

	void maxRange(MATHEX_SCALAR r);				// PRE(r > 0); PRE(!RenDevice::current()->rendering());
	void constantAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering());
	void linearAttenuation(MATHEX_SCALAR);		// PRE(!RenDevice::current()->rendering());
	void quadraticAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering());

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dUniformLight);
    PER_FRIEND_READ_WRITE(W4dUniformLight);

private:
	friend class W4dLocalLightImpl;
    virtual void updatePosition();
    virtual void updateSphere();

	RenUniformLight* pUniLight_;
};

PER_DECLARE_PERSISTENT(W4dLight);
PER_DECLARE_PERSISTENT(W4dDirectionalLight);
PER_DECLARE_PERSISTENT(W4dPointLight);
PER_DECLARE_PERSISTENT(W4dUniformLight);
PER_ENUM_PERSISTENT(W4dLight::Scope);

#endif

/* End LIGHT.HPP ****************************************************/
