/*
 * M A T E R I A L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MATERIAL_HPP
#define _RENDER_MATERIAL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class RenTexture;
class RenIMatBody;
class RenColour;
class RenMaterialTransform;

// A material collects together all the properties which affect the visible 
// surface appearance of a polygon.  This is a handle class.  It participates
// in a handle-body idiom with RenIMatBody.  There is also a material manager
// class which ensures that duplicate materials are never created.  For 
// materials, the handle-body idiom includes sharing of bodies with copy-on-
// write semantics.  Hence, you can create and copy materials with complete
// abandon and not worry about the memory overhead.
class RenMaterial
{
public:
	enum NonSharable { NON_SHARABLE };
    RenMaterial();
    RenMaterial(NonSharable);
    RenMaterial( const RenMaterial& );
    RenMaterial(const RenColour& diffuse);		// also sets ambient
    RenMaterial& operator =( const RenMaterial& );
    bool operator ==( const RenMaterial& );
    virtual ~RenMaterial();
	
	// If the ambient colour is never explicitly set, then it is always
	// set to the same as the diffuse colour.
	void texture(const RenTexture&);
	void diffuse (const RenColour&);
	void specular(const RenColour&);
	void ambient (const RenColour&);
	void emissive(const RenColour&);
	void shininess(double);
	void coplanarPriority(short);
	void alphaPriority(short);
	void interMeshCoplanar(bool);
	void absoluteAlphaPriority(bool);
	void intraMeshAlphaPriority(bool);
	void meshId(ushort);
	void fogMultiplier(float);
	
	const RenTexture& texture() const;
	const RenColour&  diffuse () const;
	const RenColour&  specular() const;
	const RenColour&  ambient () const;
	const RenColour&  emissive() const;
	double shininess() const;
	short coplanarPriority() const;
	short coplanarDepth() const;			// inverted: USHRT_MAX - priority
	short alphaPriority() const;
	bool interMeshCoplanar() const;
	bool absoluteAlphaPriority() const;
	bool intraMeshAlphaPriority() const;
	ushort meshId() const;
	float fogMultiplier() const;
	
	// Is this material transparent or translucent?  This will only return 
	// true if the RenDevice supports alpha; it returns false on devices with 
	// only stippling.  This also accounts for any alpha in the texture.
	bool hasAlphaTransparency() const;
	
	// Does the material have a diffuse alpha of less than one.  In contrast to
	// the above, this returns true iff stippling or alpha is supported and it
	// doesn't account for the texture.
	bool hasFlatTransparency() const;

	// Ideally bilinear filtering would be a material property, so that the
	// same texture could be drawn with or without bilinear in different 
	// contexts.  Unfortunately, the only way to get the information into the
	// system is to use a texture file naming convention which means that it
	// is a texture property.  Material delegates this method to RenTexture
	// to make it appear as if the ideal were implemented.		
	bool usesBilinear() const;
	
	// Returns true if this object has a non-black diffuse, specular or ambient
	// colour.  If it's purely emissive, it returns false indicating that 
	// RenDevice::disableLighting can be called without changing the rendering.
	bool needsLighting() const;

	// The material manager transparently tries to maximise sharing of
	// material bodies.  There is a cost overhead associated with searching
	// for duplicates etc.  Most materials should be created at start-up time
	// and not modified, so the overhead does not apply.  In the cases (hopefully
	// rare) where a material changes dynamically, this method will force the 
	// render library to bypass the sharing mechanism.  Thus, modifications to
	// this material will be much faster.  Of course, this defeats the space
	// saving features of the material manager and obviates optimisations
	// applying to identical materials.  ***Use sparingly.***
	void makeNonSharable();
	
	// Like an equality test, but ignoring the texture and the flags.
	bool areColoursEqual(const RenMaterial&) const;

	// Returns true if the material has a fog multiplier modifying the fog
	// depth interpolation
	// usefull for rendering of emissive material through the fog.
	bool hasFogMultiplier() const;

	// The effect should be newed; this class is responsible for deleting it.
	static void globalTransform(const RenMaterialTransform*);
	static const RenMaterialTransform* globalTransform();

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const RenMaterial& t );
	static void printAll(ostream&);

	// NB: 1) Equality has value semantics.  If you create two separate,
	//        but identical materials, they will compare equal.
	//     2) The ordering defined by < is based on comparing the pointers to
	//		  the internal body class.  Thus, the ordering has no connection
	//		  with the material's attributes and there is no meaningful
	//		  interpretation.  It can however be used for ordering materials 
	//		  in STL containers.
	friend bool operator< (const RenMaterial& m1, const RenMaterial& m2);
	friend bool operator==(const RenMaterial& m1, const RenMaterial& m2);

	// Note: This is provided as an equivalent to less<RenMaterial> simply 
	// to reduce STL dependencies.
	class Less
	{
	public:
		bool operator()(const RenMaterial& m1, const RenMaterial& m2) const
		{
			return m1 < m2;
		}
	};

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenMaterial );
    PER_FRIEND_READ_WRITE( RenMaterial );

private:
	friend class RenIDeviceImpl;
	friend class RenIIlluminator;
	RenIMatBody* body_;
};

PER_DECLARE_PERSISTENT( RenMaterial );

// A class which takes a an input material and produces an output material as
// some function of the input one.
class RenMaterialTransform
{
public:
	// PRE(mat);
	virtual void transform(const RenMaterial& in, RenMaterial* out) const =0;
	virtual ~RenMaterialTransform();
};

#ifdef _INLINE
	#include "render/material.ipp"
#endif

#endif

/* End MATERIAL.HPP *************************************************/
