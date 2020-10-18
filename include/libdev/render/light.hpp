/*
 * L I G H T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_LIGHT_HPP
#define _REN_LIGHT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"

class RenColour;
class RenLightColourTransform;
class RenILight;
class RenIDirectionalLight;
class RenIPointLight;
class RenIUniformLight;
class RenIAttenuatedLight;
class MexVec3;
class MexPoint3d;

// An abstract base class for objects which can illuminate the scene.  The 
// only properties which all lights share are colour and an on/off flag.
// In general, all lighting parameters except on/off cannot be changed whilst
// the scene is being rendered.
class RenLight
{
public:
	virtual ~RenLight();

	void colour(const RenColour&);		
	const RenColour& colour() const;
	
	// If set, all light colours are transformed by this before being used.  
	// Defaults to NULL.
	static void globalColourTransform(const RenLightColourTransform*);
	static const RenLightColourTransform* globalColourTransform();

	void turnOn();
	void turnOff();
	bool isOn() const;

	// Will return true if this light actually lit some vertices.
	bool wasUsedThisFrame() const;

	RenILight* pILight();
	
	void print(ostream&) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( RenLight );
    PER_FRIEND_READ_WRITE( RenLight );
    
protected:
	// NB: the base class *owns* the internal light and will delete it.
	RenLight(RenILight*);

private:
	RenILight* pILight_;
};

ostream& operator<<(ostream&, const RenLight*);
ostream& operator<<(ostream&, const RenLight&);

PER_DECLARE_PERSISTENT( RenLight );

// A concrete light which illuminates all objects with light rays from the same
// direction.  Such a light is effectively at infinity and is good for 
// modelling light sources like the sun.
class RenDirectionalLight : public RenLight
{
public:
	// Sets the direction along x-axis and the colour to white.
	RenDirectionalLight();
	virtual ~RenDirectionalLight();

	void direction(const MexVec3&);			// PRE(!RenDevice::current()->rendering3D());
	const MexVec3& direction() const;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenDirectionalLight );
    PER_FRIEND_READ_WRITE( RenDirectionalLight );
    
private:
	RenIDirectionalLight* pDir_;
};

ostream& operator<<(ostream&, const RenDirectionalLight&);

PER_DECLARE_PERSISTENT( RenDirectionalLight );

// The attenuation co-efficients default to a reasonable mixture of linear (0.7)
// and quadratic (0.3).  The default constant value is (0).
class RenAttenuatedLight : public RenLight
{
public:
	const MexPoint3d& position() const;
	MATHEX_SCALAR maxRange() const;
	MATHEX_SCALAR constantAttenuation() const;
	MATHEX_SCALAR linearAttenuation() const;
	MATHEX_SCALAR quadraticAttenuation() const;

	void position(const MexPoint3d&); 			// PRE(!RenDevice::current()->rendering3D());
	void maxRange(MATHEX_SCALAR r);				// PRE(r > 0); PRE(!RenDevice::current()->rendering3D());
	void constantAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering3D());
	void linearAttenuation(MATHEX_SCALAR);		// PRE(!RenDevice::current()->rendering3D());
	void quadraticAttenuation(MATHEX_SCALAR);	// PRE(!RenDevice::current()->rendering3D());

	// The outputs of the attenuation calculations will be considered the same
	// if they are equal within this tollerance.  This allows optimisations to
	// be made whereby multiple vertices are attenuated with one calculation.
	static MATHEX_SCALAR resultTollerance();
	static void resultTollerance(MATHEX_SCALAR v);		// PRE(v >= 0);

    PER_MEMBER_PERSISTENT_VIRTUAL( RenAttenuatedLight );
    PER_FRIEND_READ_WRITE( RenAttenuatedLight );
    
protected:
	RenAttenuatedLight(RenIAttenuatedLight*);
	
private:
	RenIAttenuatedLight* atLight_;
};

ostream& operator<<(ostream&, const RenAttenuatedLight&);

PER_DECLARE_PERSISTENT( RenAttenuatedLight );

// A concrete light which illuminates objects with light emanating for a 
// point in space.  More expensive, but good for modelling small, local lights
// such as street lamps or explosions.
class RenPointLight	: public RenAttenuatedLight
{
public:
	// Defaults the position to the origin. There's not really a sensible 
	// default for the range -- so you must supply one.
	RenPointLight(MATHEX_SCALAR range);		// PRE(r > 0);

    PER_MEMBER_PERSISTENT_VIRTUAL( RenPointLight );
    PER_FRIEND_READ_WRITE( RenPointLight );
    
};

PER_DECLARE_PERSISTENT( RenPointLight );

// This is a rather odd light.  Vertices are illuminated as a function of 
// *range only*, i.e., the vertex normals are ignored.  Put another way: it 
// is an ambient source with a range of effect.  Although it has absolutely 
// no basis in real physics, it may be useful for not-very-critical effects 
// such as explosions.  It is, of course, a squillion times faster than a
// point light.
class RenUniformLight : public RenAttenuatedLight
{
public:
	// Defaults the position to the origin. There's not really a sensible 
	// default for the range -- so you must supply one.
	RenUniformLight(MATHEX_SCALAR range);		// PRE(r > 0);

    PER_MEMBER_PERSISTENT_VIRTUAL( RenUniformLight );
    PER_FRIEND_READ_WRITE( RenUniformLight );
    
};

PER_DECLARE_PERSISTENT( RenUniformLight );

#endif

/* End LIGHT.HPP ****************************************************/
