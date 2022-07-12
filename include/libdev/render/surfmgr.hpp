 /*
 * S U R F M G R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_SURFMGR_HPP
#define _RENDER_SURFMGR_HPP

#include "base/base.hpp"
#include "stdlib/strfwd.hpp"

//class ostream;
class AfxResourceLib;
class SysPathName;
class RenTexture;
class RenSurface;
class RenISurfaceManagerImpl;
template <class T> class ctl_vector;

class RenSurfaceManager
{
public:
    typedef ctl_vector<SysPathName>   PathNames;

    //  Singleton class
    static RenSurfaceManager& instance();
    virtual ~RenSurfaceManager();

	// Create a texture; load it from disk if it hasn't been loaded already.
	// The pixel format is determined by the contents of the bitmap file.
	// Returns a null value on failure.  It would be nice to state a more
	// definitive post-condition about the name, however, the path searching
	// algorithm makes the actual name quite unpredicable.
	// POST(retval.sharable() && retval.readOnly());
	// POST(implies(!retval.isNull(), name().length() > 0));

    //  Note that this must be passed a string, not a pathname. If a SysPathName is
    //  used then any base directory set will be appended to it. This then stops the
    //  search mechanism from working.
	RenTexture createTexture(const std::string& pathName);

	// Create a surface which doesn't correspond to a named bitmap.  This can't
	// be shared because it doesn't have a name.  The pixel format matches that
	// of the given surface.
	// POST(!retval.sharable() && !retval.readOnly()); POST(name().length() == 0);
	// POST(retval.width() == width && retval.height() == height);
	RenSurface createAnonymousSurface(size_t width, size_t height, const RenSurface&);

	// Create a non-texture surface in video memory.
	RenSurface createAnonymousVideoSurface(
		size_t width,
		size_t height,
		const RenSurface& pixelFmt);
	// PRE(!pixelFmt.isNull());
	// PRE(width > 0 and height > 0);
	// POST(!result.sharable() and !result.readOnly());
	// POST(name.length() == 0);

	// In terms of locating and sharing the data, this works just like
	// createTexture, except that the pixel format is taken from the given surface.
	// PRE(!surf.isNull()); PRE(name.length() > 0);
	// POST(retval.sharable() && retval.readOnly());
	// POST(implies(!retval.isNull(), name().length() > 0));
	RenSurface createSharedSurface(const std::string& name, const RenSurface& surf);

	// Create a surface whose pixel format matches that of the given surface.
	// The surface is initialised with data from the named bitmap within the given
	// resource library.  This does *not* use the search path or look in BMP files.
	// Typically, the 2nd argument will be the back buffer.
	// PRE(!surf.isNull()); PRE(bitmapName.length() > 0);
	// POST(retval.sharable() && retval.readOnly());
	RenSurface createSharedSurface(const std::string& bitmapName, const AfxResourceLib&, const RenSurface& surf);

    //  Maintains a list of directories to search for any given texture
    const PathNames&  searchList();
    void  searchList( const PathNames& newSearchList );

	// When Alt-Tab is pressed, textures can get unloaded from a hardware
	// device.  Calling this method reloads all managed textures.
	bool restoreAll();

	// Statistics: what textures are loaded?  What textures were drawn
	// during the current frame?  Memory used may become a more complex
	// issue if textures can be swapped into or out of a graphics card.
	uint nTexturesLoaded() const;			// total textures loaded
	uint memoryUsed() const;				// memory used by the above
	void startFrame();						// reset hit counts before render

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const RenSurfaceManager& t );

	RenISurfaceManagerImpl& impl();
	const RenISurfaceManagerImpl& impl() const;

	// PRE(result);
	static bool extractBMPText(const SysPathName& pathName, std::string* result);

private:
	RenISurfaceManagerImpl* pImpl_;

	// Singleton => private.
    RenSurfaceManager();

    // Operations deliberately revoked
    RenSurfaceManager( const RenSurfaceManager& );
    RenSurfaceManager& operator =( const RenSurfaceManager& );
    bool operator ==( const RenSurfaceManager& );
};

#endif

/* End SURFMGR.HPP ***************************************************/
