/*
 * M A T B O D Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MATBODY_HPP
#define _RENDER_MATBODY_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "render/render.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"

class RenTexture;
class RenDevice;
class RenIMatManager;
class RenMaterial;

// Represents all the attributes which affect the surface appearance of a
// polygon, i.e., colours and texture.  A few others such as sorting priorities
// are included for want of a better place.
// Why have both alpha and coplanar priorities?  It solves one very important
// case.  Shadows should have an alpha priority lower than any other alpha polys.
// But they need a coplanar priority which is higher than the surface they shadow.
class RenIMatBody
{
public:
    virtual ~RenIMatBody();
    bool operator ==( const RenIMatBody& );

	// Appearance properties.
	// For a description of material properties, see RenMaterial.
	void texture(const RenTexture&);
	void diffuse (const RenColour&);
	void specular(const RenColour&);
	void ambient (const RenColour&);
	void emissive(const RenColour&);
	void shininess(double);
	const RenTexture& texture() const;
	const RenColour&  diffuse () const;
	const RenColour&  specular() const;
	const RenColour&  ambient () const;
	const RenColour&  emissive() const;
	double shininess() const;

	bool hasAlphaTransparency() const;		// i.e. flat *or* texture
	bool hasFlatTransparency() const;		// i.e. flat, regardless of texture
	bool usesBilinear() const;

	// Sorting order properties.
	void coplanarPriority(short);
	void alphaPriority(short);
	void interMeshCoplanar(bool);
	void absoluteAlphaPriority(bool);
	void intraMeshAlphaPriority(bool);
	void meshId(ushort);
	void hasFogMultiplier(bool);
	void fogMultiplier(float);
	short coplanarPriority() const;
	short coplanarDepth() const;			// inverted: SHRT_MAX - priority
	short alphaPriority() const;
	bool interMeshCoplanar() const;
	bool absoluteAlphaPriority() const;
	bool intraMeshAlphaPriority() const;
	ushort meshId() const;
	bool hasFogMultiplier() const;
	float fogMultiplier() const;

	// Material management.
	bool sharable() const;
	void makeNonSharable();


    friend ostream& operator <<( ostream& o, const RenIMatBody& t );
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenIMatBody );
    PER_FRIEND_READ_WRITE( RenIMatBody );

private:
	// Methods which the material manager can call.
	friend class RenIMatManager;
    friend ostream& operator <<( ostream& o, const RenIMatManager& t );
    RenIMatBody();
    RenIMatBody( const RenIMatBody& );
    RenIMatBody(const RenColour& diffuse);		// also sets ambient
	void update();

	// Methods for use by the handle class, RenMaterial.
	friend class RenMaterial;
    friend void perRead( PerIstream&, RenMaterial& );
	void incRefCount();
	void decRefCount();				// PRE(refCount_ > 0);
	ushort refCount() const;

	void updateColours();
	void updateTransparent();

	RenTexture		texture_;
	RenColour		diffuse_, emissive_;
	short			depth_, alphaPriority_;
	ushort			refCount_;
	ushort			meshId_;
	float			fogMultiplier_;
	bool			dirty_:1, alphaTransparent_:1, transparent_:1;
	bool			interMeshCoplanar_:1, sharable_:1;
	bool			absolutePriority_:1, intraMeshPriority_:1;

	// This is a constant property of textures.  Thus, we can safely cache the
	// value and only update it when the texture changes.
	bool			usesBilinear_:1;
	bool			hasFogMultiplier_:1;

    // Operations deliberately revoked
    RenIMatBody& operator =( const RenIMatBody& );
};

PER_DECLARE_PERSISTENT( RenIMatBody );

// #define DBG_MAT_SHARE(x)	x
#define DBG_MAT_SHARE(x)	;

#ifdef _INLINE
    #include "internal/matbody.ipp"
#endif

#endif

/* End MATBODY.HPP *************************************************/
