/*
 * B L I T A B L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_BLITABLE_HPP
#define _RENDER_BLITABLE_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "render/render.hpp"
#include <GL/glew.h>

class SysPathName;
class RenColour;
class RenISurfBody;
class RenIPixelFormat;
class AfxResourceLib;
template <class T> class ctl_vector;
//template <class T> class basic_string;
//typedef basic_string< char > string;

// A class representing 2D images composed of pixels.  This interface can
// represent visible display surfaces, textures or arbitrary images created
// by clients.  The images can exist in main memory or video memory.
// RenDisplay and RenTexture both have methods which create RenSurfaces.
//
// In general, there are preconditions which ensure that any write operation,
// i.e., blit, fill, line, etc, does not write outside the destination's area.
// Plus, writes cannot be applied to read-only surfaces: PRE(!readOnly());
// They are not explicitly listed.
class RenSurface
{
public:
	typedef Ren::Rect Rect;

	// Creates a null, not very useful surface.
	// POST(isNull() && sharable() && readOnly());
    RenSurface();

	// Surface creation is controlled by the manager.  However, you may copy
	// surface objects to your heart's content.
    RenSurface(const RenSurface&);
    RenSurface& operator=(const RenSurface&);

	// Create a surface of the given size with a pixel format which matches
	// that of the given surface.  No data is copied form the given surface.
	// Typically, the 2nd argument will be the back buffer.
	// PRE(!surf.isNull()); PRE(width > 0 && height > 0);
	// POST(!retval.sharable() && !retval.readOnly()); POST(name.length() == 0);
	static RenSurface createAnonymousSurface(size_t width, size_t height, const RenSurface& surf);

	// Create a non-texture surface in video memory.
	static RenSurface createAnonymousVideoSurface(
		size_t width,
		size_t height,
		const RenSurface& pixelFmt);
	// PRE(!pixelFmt.isNull());
	// PRE(width > 0 and height > 0);
	// POST(!retval.sharable() && !retval.readOnly()); POST(name.length() == 0);

	// Create a surface with a pixel format which matches that of the given surface.
	// The surface is initialised ( width, height, pixels ) with data stored in the
	// persistent stream.
	// PRE(!surf.isNull());
	// POST(!retval.sharable() && !retval.readOnly()); POST(name.length() == 0);
	static RenSurface createAnonymousSurface( PerIstream&, const RenSurface& surf);

	// Create a surface whose pixel format matches that of the given surface.
	// The surface is initialised with data from the given named bitmap.  This
	// uses the surface manager's search path to locate the named BMP file.
	// Typically, the 2nd argument will be the back buffer.
	// PRE(!surf.isNull());
	// POST(retval.sharable() && retval.readOnly());
	static RenSurface createSharedSurface(const std::string& bitmapName, const RenSurface& surf);

	// Create a surface whose pixel format matches that of the given surface.
	// The surface is initialised with data from the named bitmap within the given
	// resource library.  This does *not* use the search path or look in BMP files.
	// Typically, the 2nd argument will be the back buffer.
	// PRE(!surf.isNull());
	// POST(retval.sharable() && retval.readOnly());
	static RenSurface createSharedSurface(const std::string& bitmapName, const AfxResourceLib&, const RenSurface& surf);

	// Create a new surface whose area subsets the given one's area.  The result
	// shares the same representation & data, so the input must be sharable.
	// Not implemented yet.
	// PRE(surf.sharable()); PRE(!surf.isNull());
	// PRE(subArea.width <= surf.width() && subArea.height <= surf.height());
	// POST(retval.width() == subArea.width && retval.height() == subArea.height);
	// POST(retval.sharable());
	static RenSurface createSurface(const RenSurface& surf, const Rect& subArea);

    virtual ~RenSurface();

	// Use to indicate bad return values etc.
	bool isNull() const;

	// Blit from the given surface to this one:
	//   simple = one copy, no repeats and no stretching
	//   tiled  = repeated w/o stretching to entirely cover the destination
	// The shorter version of simpleBlit copies the entire source.
	// PRE(source.pixelFormat() == pixelFormat());		applies to all blits
	void simpleBlit (const RenSurface& source, const Rect& srcArea, int destX=0, int destY=0);
	void simpleBlit (const RenSurface& source, int destX=0, int destY=0);
	void tileBlit   (const RenSurface& source, const Rect& srcArea, const Rect& destArea);
	void stretchBlit(const RenSurface& source, const Rect& srcArea, const Rect& destArea);
	void stretchBlit(const RenSurface& source, const Rect& destArea);
	void stretchBlit(const RenSurface& source);
	void copyFromRGBABuffer(const uint* buff);

	void hollowRectangle(const Rect&, const RenColour&, int thickness);
	void filledRectangle(const Rect&, const RenColour&);

	// OpenGL/DrawPrimitive style of line drawing API.
	void startPolyLine(const RenColour&, int thickness);	// PRE(!doingLine()); POST(doingLine());
	void lineVertex(int x, int y);							// PRE(doingLine());
	void endPolyLine();										// PRE(doingLine()); POST(!doingLine());
	bool doingLine();

	// PRE(pts.size() > 1); PRE(thickness > 0);
	typedef ctl_vector<MexPoint2d> Points;
	void polyLine(const Points& pts, const RenColour&, int thickness);

	void getPixel(int x, int y, RenColour*) const;
	void setPixel(int x, int y, const RenColour&);

	void saveAsBmp(const Rect& area, const SysPathName& ) const;

	void ellipse( const Rect& area, const RenColour& outlineColour, const RenColour& fillColour );

	// Returns the actual size used, in case an exact match doesn't exist.
	size_t useFontHeight(size_t pixelHieght);
	size_t currentFontHeight() const;			// defaults to 16.
	void drawText(int x, int y, const std::string& text, const RenColour& col);
	void textDimensions(const std::string& text, Ren::Rect* dimensions) const;

	// Is source colour keying enabled for this image?
	bool isColourKeyingOn() const;
	void  enableColourKeying();
	void disableColourKeying();				// default state is off

	// The colour key to use when this object is used as a source in a blit.
	const RenColour& colourKey() const;
	void colourKey(const RenColour&);		// default == RenColour::magenta

	// You cannot change the name of a surface which already has a name.
	// PRE(name().length() == 0);
	void name(const std::string&);
	const std::string& name() const;
	const GLint handle() const;

	// Sharing and read/write control.
	bool sharable() const;					// Is this English?
	bool readOnly() const;

	// You cannot do the opposite: change a read-only surface to be writable.
	// You can't change this property on a shared surface, however, if it's
	// already read-only, then it doesn't make any difference.
	// PRE(readOnly() || !sharable());
	// POST(sharable() && readOnly());
	void makeReadOnlySharable();

	size_t width () const;
	size_t height() const;
	bool   empty() const;		// i.e., width() == 0 || height() == 0

	// Allows a body to get a handle corresponding to itself.
	// PRE(body);
	static RenSurface createFromInternal(RenISurfBody* body);
	static const RenSurface createFromInternal(const RenISurfBody* body);

	// Read pixels from persistence stream. Assumes that the data in the persistence stream
	// corresponds to the same width and height as this RenSurface ( width and height information
	// should have already been removed from the persistence stream at this point). To avoid
	// having to create a RenSurface before calling this function you can just use the
	// createAnonymousSurface function passing in the PerIstream, this takes care of setting up
	// a surface of the correct width and height.
	// PRE( not readOnly() and not sharable() );
	void read( PerIstream& inStream );

	// Write pixels to persistence stream. Writes out the width and height first followed by
	// pixel colour information.
	void write( PerOstream& outStream );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenSurface& t );

    friend bool operator ==(const RenSurface&, const RenSurface&);
    friend bool operator !=(const RenSurface&, const RenSurface&);

protected:
	Ren::TexId myId() const		{ return myId_; }

private:
	// Create a blitable from an the internal data representation.
	// RenTexture can do this to create blitables corresponding to textures.
	// RenDisplay can likewise create blitables corresponding to its buffers.
	// This object becomes responsible for deleting the internals.
// TBD: replace this functionality in light of RenSurfaceManager.
//	friend class RenDisplay;
//	friend class RenTexture;
//	RenSurface(RenISurfBody* i);		// PRE(i);

	// Only the manager class can allocate Ids and create surfaces.
	// (A texture can create a surface corresponding to its own id.)
	friend class RenSurfaceManager;
	friend class RenISurfaceManagerImpl;
	friend class RenTexture;
	RenSurface(Ren::TexId);

	Ren::TexId myId_;

	RenISurfBody* internals();
	const RenISurfBody* internals() const;
};

#endif

/* End BLITABLE.HPP *************************************************/
