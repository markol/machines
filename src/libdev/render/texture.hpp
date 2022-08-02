/*
 * T E X T U R E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TEXTURE_HPP
#define _RENDER_TEXTURE_HPP

#include "base/persist.hpp"
#include "render/render.hpp"
#include "render/surface.hpp"

// Represents a texture loaded into memory.  The memory used and the
// associated Direct3D object are reference counted by the Singleton
// RenTexManager object.  These objects are just handles.
class RenTexture : public RenSurface
{
public:
	RenTexture();
    virtual ~RenTexture();

	bool hasAlphaTransparency() const;
	bool usesBilinear() const;

	// These methods allow internal clients to keep track of how often
	// this texture is drawn.
	uint hitsPerFrame() const;
	void incHits();
	
	void print(ostream& o) const;
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenTexture );

private:
	// Only the texture manager can create objects of this type.
	friend class RenSurfaceManager;
    RenTexture(Ren::TexId);
};

PER_READ_WRITE( RenTexture );
PER_DECLARE_PERSISTENT( RenTexture );

ostream& operator <<( ostream& o, const RenTexture& t );

#endif

/* End TEXTURE.HPP **************************************************/
