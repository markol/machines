/*
 * T E X B O D Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/internal/texbody.hpp"

#include <stdlib.h>					// for abs
#include <iostream>
#include <iomanip>
#include "stdlib/string.hpp"
#include "render/device.hpp"
#include "render/colour.hpp"
#include "render/capable.hpp"
#include "render/internal/debug.hpp"
#include "render/internal/pixelfmt.hpp"
#include "render/internal/devicei.hpp"

#ifndef _INLINE
    #include "render/internal/texbody.ipp"
#endif

RenITexBody::RenITexBody():
	RenISurfBody(),
#ifndef NDEBUG
//	handleDev_(NULL),
#endif
	loaded_(false),
	hitsPerFrame_(0),
	alpha_(false),
	bilinear_(false)
{
    TEST_INVARIANT;
}

RenITexBody::~RenITexBody()
{
    TEST_INVARIANT;
}

typedef struct {
    RenIPixelFormat	result, target;
}   FindTextureData;

static void chooseClosestFormat
(
	FindTextureData& data,
	const RenIPixelFormat& candidate
)
{

	const int d1 = data.result.RGBDepth() - data.target.RGBDepth();
	const int d2 = candidate.RGBDepth()   - data.target.RGBDepth();

	RENDER_STREAM("    d1=" << d1 << ", d2=" << d2 << std::endl);

	// If we are looking for an alpha format, we must still choose non-alpha
	// if there is no alpha format.  In that case we must do the else clause.
//	if (data.targetAlpha > 0 && candidate.hasAlpha())
	{
		// If we don't already have a valid format, pick the first candidate.
//		if (!data.result.isValid())
		{
			data.result = candidate;
			return;
		}

		// If we don't already have an alpha format, pick this one regardless.
		// Otherwise, pick the alpha depth which is closest -- not necessarily
		// greater than the requested one.  However, always pick an RGB depth
		// which is *at least* what is requested.
		if (data.result.alphaDepth() == 0)
			data.result = candidate;
		//else if (abs(ad2) < abs(ad1) && d2 >= 0 && abs(d2) < abs(d1))
			//data.result = candidate;
	}
//	else
	{
//		if (data.targetAlpha > 0 && data.result.hasAlpha())
		{
			RENDER_STREAM("    rejected non-alpha format because a candidate with alpha exists." << std::endl);
			return;
		}

		// Conceivably, we could use a format with alpha where none was
		// required.  However, this is wasteful and where alpha is available
		// there is typically a better choice of format without alpha.
//		if (candidate.hasAlpha())
		{
			RENDER_STREAM("    rejected format because it contains alpha." << std::endl);
			return;
		}

		// If we don't already have a valid format, pick the first candidate.
//		if (!data.result.isValid())
		{
			data.result = candidate;
			return;
		}

		// Prefer the closest format and always prefer a bit count which is
		// greater than or equal to the one requested.
		if (d2 >= 0 && abs(d2) < abs(d1))
			data.result = candidate;
		else if (d1 < 0 && d2 < 0 && abs(d2) < abs(d1))
			data.result = candidate;
	}
}

static bool isPowerOf2(int dimension)
{
	switch (dimension)
	{
		case 1:    return true;
		case 2:    return true;
		case 4:    return true;
		case 8:    return true;
		case 16:   return true;
		case 32:   return true;
		case 64:   return true;
		case 128:  return true;
		case 256:  return true;
		case 512:  return true;
		case 1024: return true;
		case 2048: return true;
		case 4096: return true;
		default:   return false;
	}
}

// Implements a naming convention: textures which use colour keyed
// transparency have names ending in "_t" or "_bt".
static bool isTransparent(const SysPathName& pathname)
{
	const std::string & texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_t.bmp" ) == 0)
		return true;

	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	return strcasecmp( end2.c_str(), "_bt.bmp" ) == 0;
}

// These two methods are only used for assert checking.
#ifndef NDEBUG
static bool isAlpha(const SysPathName& pathname)
{
	bool result=false;
	const std::string& texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_a.bmp" ) == 0)
		result = true;
	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	if( strcasecmp( end2.c_str(), "_ba.bmp" ) == 0 )
		result = true;
	return result;
}

static bool isColour(const SysPathName& pathname)
{
	bool result=false;
	const std::string & texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_c.bmp" ) == 0)
		result = true;
	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	if( strcasecmp( end2.c_str(), "_bc.bmp" ) == 0 )
		result = true;
		return result;
}
#endif

// Implements a naming convention: textures which use colour keyed
// transparency have names ending in "_b" or "_bt".
static bool bilinearRequired(const SysPathName& pathname)
{
	const std::string & texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_b.bmp" ) == 0)
		return true;

	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	return strcasecmp( end2.c_str(), "_bt.bmp" ) == 0;
}

// Implements a naming convention: alpha maps have the same name as their
// associated texture map, except that the "_t" is replaced with "_a".
static SysPathName alphaMapName(const SysPathName& pathname)
{
	PRE(isTransparent(pathname));

	string  retval = pathname.pathname();
	retval[retval.length() - 5] = 'a';
	return retval;
}

// Implements a naming convention: certain textures require a colour-only map
// *without* transparency colour-keying.  Given a normal, texture map name, the
// associated colour-only map has the same name, except that the "_t" is
// replaced with "_c".
static SysPathName colourMapName(const SysPathName& pathname)
{
	PRE(isTransparent(pathname));

	string  retval = pathname.pathname();
	retval[retval.length() - 5] = 'c';
	return retval;
}

//static bool checkTextureSize(const SysPathName& fileName, const BITMAP& bitmap)
static bool checkTextureSize(const SysPathName& fileName, const SDL_Surface* surface)
{
	//ASSERT_INFO(bitmap.bmWidth);
	//ASSERT_INFO(bitmap.bmHeight);
    ASSERT_INFO(surface->w);
	ASSERT_INFO(surface->h);

	// If you load a texture which doesn't have these dimensions, Direct3D
	// silently displays nothing.
	if (!isPowerOf2(surface->w) || !isPowerOf2(surface->h))
	{
		RENDER_STREAM("  Texture " << fileName << " must have dimensions which are a power of 2." << std::endl);
		RENDER_STREAM("  (Its size is " << surface->w << "x" << surface->h << ".)" << std::endl);

		ASSERT_FAIL("Texture size is not a power of two.");
		return false;
	}

	// There's a D3D capability query which tells you the maximum size of a
	// texture.  However, all 3Dfx systems appear to have this 256 limitation.
	// So, in practice, it applies to all textures all of the time.  To cope
	// the models must be built with this in mind, hence it's hardcoded.
	// TBD: we could maybe devise a better strategy for dealing with this, i.e.
	// automatically down-sizing some textures which are too large???
	const static uint16 maxTextureSize = 2048;

	if (surface->w > maxTextureSize || surface->h > maxTextureSize)
	{
		RENDER_STREAM("  Texture " << fileName << " must be smaller than " << maxTextureSize << "." << std::endl);
		RENDER_STREAM("  (Its size is " << surface->w << "x" << surface->h << ".)" << std::endl);

		ASSERT_FAIL("Texture size is greater than the maximum (2048x2048).");
		return false;
	}

	return true;
}

// Load a texture from a file by using windows functions to load
// a bmp, then GDI functions to copy the image to the DD surface.
// virtual
bool RenITexBody::read(const std::string & nameAsString)
{
	PRE(not isAlpha(SysPathName(nameAsString)));
	PRE(not isColour(SysPathName(nameAsString)));
	PRE(Ren::initialised());
    TEST_INVARIANT;

	const SysPathName requestedTextureName ( nameAsString );
	RENDER_STREAM("Trying to load texture " << requestedTextureName << ":" << std::endl);
	ASSERT_INFO(requestedTextureName);

	const bool	 transparent = isTransparent(requestedTextureName);
	const RenCapabilities& caps = RenDevice::current()->capabilities();
	bilinear_ = bilinearRequired(requestedTextureName) && caps.supportsBilinear();
	const bool colourKey = caps.supportsColourKey();

	const char* fname = requestedTextureName.pathname().c_str();
	const SysPathName colourName = (transparent)? colourMapName(requestedTextureName): requestedTextureName;

	// If this is a transparent texture and the device supports alpha texture
	// and a special colour-only map exists, then load that in preference to
	// the named file given as the arg to this function.
	if (transparent && caps.supportsTextureAlpha() && colourName.existsAsFile())
	{
		// However, if the texture is *not* bilinear blended and colour-keying
		// *is* supported, load the colour-keyed version because it will
		// typically be more efficient and avoids alpha sorting problems.  This
		// should be the correct choice for architectures without colour-keying
		//  (PowerVR) and for architectures with both alpha and keying (3Dfx).
	    if (bilinear_ || !colourKey)
	    {
			fname = colourName.pathname().c_str();
			RENDER_STREAM("  Colour-only map exists, loading " << colourName << " instead." << std::endl);
	    }
	}

    SDL_Surface* surface = IMG_Load(fname);
    if (!surface)
    {
		RENDER_STREAM("  Failed to load texture from file " << fname << std::endl);
        return false;
    }

    if (!checkTextureSize(requestedTextureName, surface))
		return false;

	// Use a separate alpha map file if it exists.
	bool tryToLoadAlpha = transparent && caps.supportsTextureAlpha();

	// But prefer colour-keying if it's supported and bilinear is not required.
	if (colourKey && !bilinear_)
		tryToLoadAlpha = false;

    RenIPixelFormat rqFormat;
    SDL_Surface* surfaceAlpha = NULL;
	if (tryToLoadAlpha)
	{
		const SysPathName alphaName = alphaMapName(requestedTextureName);
		RENDER_STREAM("  Looking for alpha map file " << alphaName);

		if (!alphaName.existsAsFile())
		{
			RENDER_STREAM(" (not found)" << std::endl);
		}
		else
		{
			RENDER_STREAM(" (found)" << std::endl);

			const char* fname = alphaName.pathname().c_str();
			surfaceAlpha = IMG_Load(fname);
			if (!surfaceAlpha)
			{
				RENDER_STREAM("  Failed to load alpha map from file " << alphaName << std::endl);
			    return false;
			}
			ASSERT(surfaceAlpha->w == surface->w,  "Wrong alpha map size.");
			ASSERT(surfaceAlpha->h == surface->h, "Wrong alpha map size.");
		}
	}

//	alpha_ = transparent && alphaHandle;
	alpha_ = transparent && surfaceAlpha;

	// If the HAL's colour model is set, that implies that 3D hardware is
	// present, therefore create the main surface in video memory.
	RenISurfBody::Residence residence = SYSTEM;

	if(caps.hardware())
		residence = TEXTURE;

    if (!allocateDDSurfaces(surface->w, surface->h, rqFormat, residence))
    {
	    //DeleteObject(handle);
	    SDL_FreeSurface(surface);
	    //DeleteObject(alphaHandle);
		return false;
    }

	name(nameAsString);

    // copy the bitmap to our surface
    bool retval;
	if (alpha_)
	{
        retval = copyWithAlpha(surface, surfaceAlpha, true);
        SDL_FreeSurface(surfaceAlpha);
	}
//	else if (transparent && !colourKey)
	else
	{
//		ASSERT(alphaChannelFound, logic_error());
		//retval = copyWithColourKeyEmulation(handle, RenColour::magenta());
		retval = copyWithColourKeyEmulation(surface, RenColour::magenta(), true);
		alpha_ = transparent;
	}

	loaded_ = true;
    TEST_INVARIANT;

	SDL_FreeSurface(surface);
	RENDER_STREAM(std::endl);
    return retval;
}

bool RenITexBody::hasAlphaTransparency() const
{
	return alpha_;
}

bool RenITexBody::usesBilinear() const
{
	return bilinear_;
}

uint RenITexBody::hitsPerFrame() const
{
    TEST_INVARIANT;
	return hitsPerFrame_;
}

void RenITexBody::incHits()
{
    TEST_INVARIANT;
	++hitsPerFrame_;
}

void RenITexBody::resetHits()
{
    TEST_INVARIANT;
	hitsPerFrame_ = 0;
}
void RenITexBody::CLASS_INVARIANT
{
    INVARIANT( this != NULL );

	// If the texture loaded correctly, these should all be non zero.
//	INVARIANT(implies(loaded_, width_    > 0));
//	INVARIANT(implies(loaded_, height_   > 0));
//	INVARIANT(implies(loaded_, bitDepth_ > 0));
}

// virtual
RenITexBody* RenITexBody::castToTexBody()
{
	return this;
}

// virtual
const RenITexBody* RenITexBody::castToTexBody() const
{
	return this;
}

// virtual
void RenITexBody::print(ostream& o) const
{
    o << "texture " << name()
	  << " (" << width() << "x" << height() << "x" << bitDepth() << ")";
}

/* End TEXBODY.CPP **************************************************/
