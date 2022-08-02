/*
 * U V A N I M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_UVANIM_HPP
#define _RENDER_UVANIM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "render/texture.hpp"

// A base class which defines an animation of texture u-v co-ordinates.  
// Derived classes should implement a transform as (probably) a function of
// time.  The most common use will probably be a translation.  The render 
// library does not have the concept of time, so concrete implementations
// are left to derived classes in client libraries.
class RenUVTransform
{
public:
	RenUVTransform();
    virtual ~RenUVTransform();
	
	// PRE(uOut && vOut);
	virtual void transform(float uIn, float vIn, float* uOut, float* vOut) const = 0;
	virtual void print(ostream&) const = 0;
	
	// A virtual copy operation.  _NEW is used to allocate the return value.
	// The caller is responsible for calling _DELETE.
	virtual RenUVTransform* clone() const = 0;
	
	// Does this transfrom have any effect on UV values?  
	// The default implementation returns false.
	virtual bool isUnity() const;

	// Use a null texture to indicate that this anim applies to all polygons.
	const RenTexture& filterTexture() const;
	void filterTexture( const RenTexture& );

	// Based on filter texture, does this xform apply to polygons with the
	// given texture?
	bool appliesTo(const RenTexture&) const;

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( RenUVTransform );
	PER_FRIEND_READ_WRITE( RenUVTransform );

private:
	RenTexture filterTexture_;
};

PER_DECLARE_PERSISTENT( RenUVTransform );

// A no effect transform, simply copies uv values.
class RenUVUnityTransform : public RenUVTransform
{
public:
	RenUVUnityTransform()	{}

	// PRE(uOut && vOut);
	virtual void transform(float uIn, float vIn, float* uOut, float* vOut) const;
	virtual void print(ostream&) const;
	virtual RenUVTransform* clone() const;

	// Returns true for this class.
	virtual bool isUnity() const;
    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenUVUnityTransform );
};

PER_READ_WRITE( RenUVUnityTransform );
PER_DECLARE_PERSISTENT( RenUVUnityTransform );

// A "virtual" insertion operator -- calls t.print(o).
ostream& operator <<( ostream& o, const RenUVTransform& t );

#endif

/* End UVANIM.HPP ***************************************************/
