/*
 * I N S U R F C E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_INSURFCE_HPP
#define _RENDER_INSURFCE_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "render/render.hpp"
#include "render/colour.hpp"
#include "utility/property.hpp"
#include "render/internal/internal.hpp"
#include "render/internal/pixelfmt.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_image.h>
// FONTS
#include <ft2build.h>
#include FT_FREETYPE_H

//class ostream;
class SysPathname;
class RenITexBody;
class RenIFont;
class RenDevice;

// The internal data representation for RenSurface.
class RenISurfBody
{
public:
	// The various ctors have different intended clients.
	// This creates a default, null surface and is used by the manager.
	// POST(width() == 0 && height == 0);
	// POST(sharable() && readOnly()); POST(name().length() == 0);
	RenISurfBody();

	virtual ~RenISurfBody();

	// This is a concrete base class.  The only intended derived class is RenITexBody.
	virtual bool read(const std::string& name);
	virtual RenITexBody* castToTexBody();
	virtual const RenITexBody* castToTexBody() const;
	virtual void print(ostream&) const;

	UtlProperty<RenColour>	keyColour;		// default is purple
	UtlProperty<bool>		keyingOn;		// default is false

	// This refers to the primary surface, i.e., the video memory copy if
	// there is one, otherwise the system memory surface.
	//const COMPTR(IDirectDrawSurface2)& surface() const		{ return surface_; }

	size_t bitDepth() const;
	size_t width   () const;
	size_t height  () const;
	size_t memoryUsed() const;
	bool   isEmpty() const;			// width == 0 || height == 0

	// These are methods which would be private internal methods of RenSurface,
	// however, they are placed here to reduce dependancies.
	void setDDColourKey();
	void filledRectangle(const Ren::Rect& area, uint colour);
	size_t useFontHeight(size_t pixelHieght);
	size_t currentFontHeight() const;			// defaults to 20.
	void drawText(int x, int y, const std::string& text, const RenColour& col);
	void textDimensions(const std::string& text, Ren::Rect* dimensions) const;

	// When Alt-Tab is pressed, textures can get unloaded from a hardware
	// device.  Calling this method reloads this image.
	bool restoreToVRAM() const;
	bool loadIntoVRAM() const;
	bool recreateVRAMSurface();

	//HDC getDC();
	//HDC DC() const;
	void releaseDC();

	uint refCount() const;
	void incRefCount();
	void decRefCount();

	// You cannot change the name of a surface which already has a name.
	// PRE(name().length() == 0);
	void name(const std::string&);
	const std::string& name() const;

	// returns the name of the shared resource (leafName or full pathName)
	const std::string& sharedName() const;

	// is the file leaf name to be shared or the full pathname
	void shareLeafName( bool );

	// Sharing and read/write control.
	bool sharable() const;					// Is this English?
	bool readOnly() const;

	// You cannot do the opposite: change a read-only surface to be writable.
	// You can't change this property on a shared surface, however, if it's
	// already read-only, then it doesn't make any difference.
	// PRE(readOnly() || !sharable());
	// POST(sharable() && readOnly());
	void makeReadOnlySharable();

	// Apply the given DD palette to this surface (including any system memory
	// backup).
	// PRE(pixelFormat.isPalette());
	// PRE(pixelFormat.RGBDepth() <= 8);
	//void usePalette(const COMPTR(IDirectDrawPalette)&);

	// Same pre-conditions extract a palette from the given bitmap and use it.
	//void usePalette(HBITMAP bitmap);

	const RenIPixelFormat& pixelFormat() const;

	void unclippedBlit(const RenISurfBody* source, const Ren::Rect& srcArea, int destX, int destY);
	void unclippedStretchBlit(const RenISurfBody* source, const Ren::Rect& srcArea, const Ren::Rect& destArea);

	// Copy an image using the GDI.  This can do conversions between numerous
	// bit depths but can't do alpha or colour key emulation.
	//bool copyUsingGDI(HBITMAP bitmap);

	// Initialise this surface using a second bitmap as an alpha map.
	// PRE(pixelFormat().hasAlpha());
	bool copyWithAlpha(SDL_Surface* surface, SDL_Surface* surfaceAlpha, bool createMipmaps = false);

	// Copy an image from a GDI Bitmap to a DirectDraw surface.  Whilst copying,
	// if any texel matches the given colour key, the alpha component of the
	// output texture will be appropriately set to transparent.
	// PRE(pixelFormat().hasAlpha());
	bool copyWithColourKeyEmulation(SDL_Surface* surface, const RenColour& keyColour, bool createMipmaps = false);
	// Copy image from RGBA buffer of size width * height
	bool copyFromBuffer(const uint* pixelsBuffer);

	// This does a case-insensitive comparison on the names.  The manager
	// needs this test to implement it's factory like behaviour.
    bool matches(const std::string& name) const;

	bool isFront() const	{ return displayType_ == RenI::FRONT; }
	bool isOffscreen() const	{ return displayType_ == RenI::NOT_DISPLAY; }
	const GLuint handle() const { return textureID_; }

protected:
	// Used by the surface manager to create surfaces is specific places. The = 2 is a
	// safety measure as the parameter used to be a bool.
	enum Residence
	{
		TEXTURE = 2,
		SYSTEM,
		VIDEO
	};

	// POST(width() == w && height() == h); POST(pixelFormat.isValid());
	bool allocateDDSurfaces(size_t width, size_t height, const RenIPixelFormat&, Residence);

private:
	// Only the surface manager can create internal surface objects.
	friend class RenSurfaceManager;
	friend class RenISurfaceManagerImpl;

	// Allocates DirectDraw memory for textures and non-texture bitmaps. This
	// creates a writable surface.  The client must change it to read-only, if
	// appropriate, e.g., all textures are read-only.
	// POST(!sharable() && !readOnly());  POST(name().length() == 0);
	// POST(width() == w && height() == h); POST(pixelFormat.isValid());
	RenISurfBody(size_t w, size_t h, const RenIPixelFormat&, Residence);

	// Create an empty surface with the given pixel format.  Call read to
	// subsequently allocate memory and load up some data.
	// POST(width() == 0 && height() == 0); POST(!sharable() && !readOnly());
	// POST(pixelFormat_.isValid()); POST(name().length() == 0);
	RenISurfBody(const RenIPixelFormat&);

	// This creates a surface corresponding to a display backbuffer.  Although
	// the display may be shared by multiple clients, it is not sharable in the
	// sense that the surface manager will find a named copy of the surface.  In
	// order to aquire a display surface, you must go via a RenDisplay.
	// PRE(dev); PRE(t != NOT_DISPLAY);
	// POST(!sharable() && !readOnly()); POST(name().length() == 0);
	// POST(pixelFormat.isValid());
	RenISurfBody(const RenDevice* dev, RenI::DisplayType t);

	// Truly private, as opposed to available to friends.
	void updateDescr();

	// Like DirectDraw's blit with the DDBLT_WAIT flag set.  However, this fn
	// does any necessary waiting and writes info about it onto the render stream.
	//bool waitForBlit(RECT& dest, const RenISurfBody* source, RECT& src);

	// NB: a COMPTR is *not* used for the device.  The surface gets released
	// after the RenDevice is destroyed and releasing a COM pointer to the
	// D3D device then appears to make Direct3D fall over.
	RenI::DisplayType			displayType_;
	const RenDevice*			device_;
	RenIPixelFormat				pixelFormat_;
	GLuint                      textureID_;
	uint                        width_, height_;

	uint						refCount_;
	std::string						name_, leafName_;
	bool						sharedLeaf_;
	bool						loaded_, readOnly_, sharable_;
	size_t						currentHeight_;
	static size_t				defaultHeight();
	static const std::string&		fontName();

	struct Font
	{
		Font(): renFont_(NULL), actualHeight_(0) {}
		Font(FT_Face face, int height);
		~Font();

		bool isDefined() const	{ return renFont_; }

		RenIFont*	renFont_;
		size_t		actualHeight_;

        GLuint tex;		// texture atlas object

        unsigned int w;			// width of texture in pixels
        unsigned int h;			// height of texture in pixels

        struct
        {
            float ax;	// advance.x
            float ay;	// advance.y

            float bw;	// bitmap.width;
            float bh;	// bitmap.height;

            float bl;	// bitmap_left;
            float bt;	// bitmap_top;

            float tx;	// x offset of glyph in texture coordinates
            float ty;	// y offset of glyph in texture coordinates
            float tx2, ty2;
        } c[128];		// character information

	};

	static ctl_vector<Font>		fonts_;
	friend Font::~Font();

    RenISurfBody( const RenISurfBody& );
    RenISurfBody& operator =( const RenISurfBody& );
};

ostream& operator <<(ostream& o, const RenISurfBody& t);

#endif

/* End INSURFCE.HPP *************************************************/
