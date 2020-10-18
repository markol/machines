/*
 * I N S U R F C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "system/pathname.hpp"
#include "render/texture.hpp"
#include "render/surfmgr.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "render/internal/displayi.hpp"
#include "render/internal/surfbody.hpp"
#include "render/internal/font.hpp"
#include "device/timer.hpp"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <SDL2/SDL_image.h>
#include "render/internal/vtxdata.hpp"
#include "render/internal/colpack.hpp"

// static
ctl_vector<RenISurfBody::Font>	RenISurfBody::fonts_;

// Maximum font texture width
#define MAXWIDTH 1024
/**
 * Code taken from modern opengl tutorials.
 * The struct holds a texture atlas that contains the visible US-ASCII characters
 * of a certain font rendered with a certain character height.
 * It also contains an array that contains all the information necessary to
 * generate the appropriate vertex and texture coordinates for each character.
 *
 * After the constructor is run, you don't need to use any FreeType functions anymore.
 */
RenISurfBody::Font::Font(FT_Face face, int height): renFont_(NULL), actualHeight_(0)
{
    FT_Set_Pixel_Sizes(face, 0, height);
    FT_GlyphSlot g = face->glyph;

    unsigned int roww = 0;
    unsigned int rowh = 0;
     w = 0;
     h = 0;

     memset(c, 0, sizeof c);

    /* Find minimum size for a texture holding all visible ASCII characters */
    for (int i = 32; i < 128; i++)
    {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            std::cerr << "Loading font character "<< i << " failed!" << std::endl;
            continue;
        }
        if (roww + g->bitmap.width + 1 >= MAXWIDTH)
        {
            w = std::max(w, roww);
            h += rowh;
            roww = 0;
            rowh = 0;
        }
        roww += g->bitmap.width + 1;
        rowh = std::max(rowh, g->bitmap.rows);
    }

    w = std::max(w, roww);
    h += rowh;

    /* Create a texture that will be used to hold all ASCII glyphs */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Paste all glyph bitmaps into the texture, remembering the offset */
    int ox = 0;
    int oy = 0;
    GLuint* rgbaBitmap = _NEW_ARRAY(GLuint, height * height);
    rowh = 0;

    for (int i = 32; i < 128; i++)
    {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            std::cerr << "Loading font character "<< i << " failed!" << std::endl;
            continue;
        }

        if (ox + g->bitmap.width + 1 >= MAXWIDTH)
        {
            oy += rowh;
            rowh = 0;
            ox = 0;
        }
        for(int j = 0; j < g->bitmap.width * g->bitmap.rows; ++j)
        {
            rgbaBitmap[j] = g->bitmap.buffer[j] | 0xFFFFFF00;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, rgbaBitmap);
        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = ox / (float)w;
        c[i].ty = oy / (float)h;
        c[i].tx2 = c[i].tx + c[i].bw / (float)w;
        c[i].ty2 = c[i].ty + c[i].bh / (float)h;

        rowh = std::max(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }
    _DELETE_ARRAY(rgbaBitmap);

}

RenISurfBody::Font::~Font()
{
	// There may be other entries in the fonts_ array which share the same
	// implementation as this font.  Set any other entries to zero so they
	// are not deleted twice.
	for (int i=0; i!=RenISurfBody::fonts_.size(); ++i)
	{

		if (fonts_[i].renFont_ == renFont_)
			fonts_[i].renFont_ = NULL;
	}


	_DELETE(renFont_);
	//glDeleteTextures(1, &tex);
}

RenISurfBody::RenISurfBody():
	displayType_(RenI::NOT_DISPLAY),
	refCount_(0),
    textureID_(0),
    width_(0),
    height_(0),
	loaded_(false),
	readOnly_(true),
	sharable_(true),
	currentHeight_(0),
	device_(NULL),
	name_(""),			// NB: look at precondition on name set method.
	sharedLeaf_(true)
{
	POST(width() == 0 && height() == 0);
	POST(sharable() && readOnly());
	POST(name().length() == 0);
}

RenISurfBody::RenISurfBody
(
	size_t rqWidth,
	size_t rqHeight,
	const RenIPixelFormat& format,
	Residence residence
):
	displayType_(RenI::NOT_DISPLAY),
	refCount_(0),
    textureID_(0),
    width_(0),
    height_(0),
	loaded_(false),
	readOnly_(false),
	sharable_(false),
	currentHeight_(0),
	device_(NULL),
	name_(""),			// NB: look at precondition on name set method.
	sharedLeaf_(true)
{
	// Initialise UltProperties.
	keyingOn(false);
	keyColour(RenColour::magenta());

	allocateDDSurfaces(rqWidth, rqHeight, format, residence);

	POST(!sharable() && !readOnly());
	POST(width() == rqWidth && height() == rqHeight);
//	POST(pixelFormat_.isValid());			// TBD: write operator==
	POST(name().length() == 0);
}

RenISurfBody::RenISurfBody
(
	const RenIPixelFormat& format
):
	displayType_(RenI::NOT_DISPLAY),
	pixelFormat_(format),
	refCount_(0),
    textureID_(0),
    width_(0),
    height_(0),
	loaded_(false),
	readOnly_(false),
	sharable_(false),
	currentHeight_(0),
	device_(NULL),
	name_(""),			// NB: look at precondition on name set method.
	sharedLeaf_(true)
{
	// Initialise UltProperties.
	keyingOn(false);
	keyColour(RenColour::magenta());


	POST(width() == 0 && height() == 0);
	POST(!sharable() && !readOnly());
//	POST(pixelFormat_.isValid());			// TBD: write operator==
	POST(name().length() == 0);
}

// This is set up as non-sharable.  If two separate clients ask for display
// surfaces, then the surface manager will allocate two separate bodies.
// However, this ctor does not allocate surface memory, so there isn't a huge
// overhead.
RenISurfBody::RenISurfBody
(
	const RenDevice* dev,
	RenI::DisplayType type
):
	displayType_(type),
	device_(dev),
	refCount_(0),
	loaded_(false),
	readOnly_(false),
	sharable_(false),
	currentHeight_(0),
	name_(""),			// NB: look at precondition on name set method.
	sharedLeaf_(true)
{
	PRE(dev);
	PRE(type != RenI::NOT_DISPLAY);		// i.e. back or front

	height_ = dev->windowHeight();
	width_ = dev->windowWidth();

	POST(!sharable() && !readOnly());
	POST(name().length() == 0);
	POST(width() > 0 && height() > 0);
}

bool RenISurfBody::allocateDDSurfaces
(
	size_t rqWidth,
	size_t rqHeight,
	const RenIPixelFormat& format,
	Residence residence
)
{
    glGenTextures(1, &textureID_);
	glBindTexture(GL_TEXTURE_2D, textureID_);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/

    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, // target
		0,  // level, 0 = base, no minimap,
		GL_RGBA, // internalformat
		rqWidth,  // width
		rqHeight,  // height
		0,  // border, always 0 in OpenGL ES
		GL_RGBA,  // format
		GL_UNSIGNED_INT_8_8_8_8, // type, /GL_UNSIGNED_BYTE
		(GLvoid*)NULL);

    width_ = rqWidth;
    height_ = rqHeight;
    return true;
}

// virtual
RenISurfBody::~RenISurfBody()
{
    //Delete texture
    if( textureID_ != NULL )
    {
        glDeleteTextures( 1, &textureID_ );
        textureID_ = NULL;
    }
}

// virtual
bool RenISurfBody::read(const std::string& bitmapName)
{
	PRE(bitmapName.length() > 0);
//	PRE(pixelFormat_.isValid());	// Use the ctor which initialises the format.

	bool retval = false;
    SDL_Surface* surface = IMG_Load(bitmapName.c_str());
	if (!surface)
	{
		RENDER_STREAM("Failed to load surface from file " << SDL_GetError() << std::endl);
		return false;
	}
    else
    {
        if (allocateDDSurfaces(surface->w, surface->h, pixelFormat_, SYSTEM))
            retval = copyWithColourKeyEmulation(surface, RenColour::magenta());

        name(bitmapName);
        SDL_FreeSurface(surface);
    }

	return retval;
}

void RenISurfBody::setDDColourKey()
{
//	PRE(surface_);

	if (keyingOn())
	{

	}
}


static char* formatMsg()
{
	char* msgBuf = nullptr;

	return msgBuf;
}

void RenISurfBody::unclippedBlit(const RenISurfBody* source, const Ren::Rect& srcArea, int destX, int destY)
{
	PRE_INFO(height());
	PRE_INFO(srcArea.height);
	PRE(source);
	PRE(srcArea.width  <= width());
	PRE(srcArea.height <= height());
	PRE(destX < width());
	PRE(destY < height());
	PRE(destX + srcArea.width  <= width());
	PRE(destY + srcArea.height <= height());

	// If a cursor is displayed, all blits to the display surfaces must be
	// bracketed by start-end frame calls.
	RenDevice* dev = RenDevice::current();
	PRE_DATA(const bool displayDest = displayType_ == RenI::FRONT || displayType_ == RenI::BACK);
	PRE(dev);
	PRE(dev->display());
//	PRE(implies(displayDest && dev->display()->currentCursor(), dev->rendering()));

	Ren::Rect dstArea;
	dstArea.originX = destX;
	dstArea.originY = destY;
	dstArea.width   = srcArea.width;
	dstArea.height  = srcArea.height;

//	if( (displayType_ != RenI::FRONT) && (displayType_ != RenI::BACK) )
	if( displayType_ == RenI::NOT_DISPLAY )
    {
        dev->renderToTextureMode(textureID_, width_, height_);
	    dev->renderSurface(source, srcArea, dstArea, width_, height_);
	    dev->renderToTextureMode(0,0,0);
    }
    else
    {
        dev->renderSurface(source, srcArea, dstArea);
    }
}

void RenISurfBody::unclippedStretchBlit
(
	const RenISurfBody* source,
	const Ren::Rect& srcArea,
	const Ren::Rect& dstArea
)
{
	PRE(source);

	// If a cursor is displayed, all blits to the display surfaces must be
	// bracketed by start-end frame calls.
	RenDevice* dev = RenDevice::current();
	PRE_DATA(const bool displayDest = displayType_ == RenI::FRONT || displayType_ == RenI::BACK);
	PRE(dev);
	PRE(dev->display());
	PRE(implies(displayDest && dev->display()->currentCursor(), dev->rendering()));

	dev->renderSurface(source, srcArea, dstArea);
}

void RenISurfBody::filledRectangle(const Ren::Rect& area, uint colour)
{
    RenDevice* device_ = RenDevice::current();

    PRE(device_);
    PRE(device_->display());
    Ren::Rect srcArea;
    srcArea.originX = srcArea.originY = 0;
    srcArea.width   = srcArea.height  = 1;
    RenISurfBody emptySurf;
    RenDevice* dev = _CONST_CAST(RenDevice*, device_);

    if( displayType_ == RenI::NOT_DISPLAY )
    {
        dev->renderToTextureMode(textureID_, width_, height_);
        if(colour == 0xFFFF00FF) // Handle background colour
        {
            GLint blendSrc, blendDst;
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
            glBlendFunc(GL_ZERO, GL_ZERO);
            dev->renderSurface(&emptySurf, srcArea, area, width_, height_, colour);
            glBlendFunc(blendSrc, blendDst);
        }
        else
            dev->renderSurface(&emptySurf, srcArea, area, width_, height_, colour);
        dev->renderToTextureMode(0,0,0);
    }
    else
    {
        dev->renderSurface(&emptySurf, srcArea, area, 0, 0, colour);
    }

}

// static
const std::string& RenISurfBody::fontName()
{
	static bool first = true;
	static std::string fn = "Arial";

	if (first)
	{
		first = false;

		const char* envVar = getenv("CB_RENDER_FONT");
		if (envVar)
		{
			char* copy = strdup(envVar);
			fn = strtok(copy, ":");
			free(copy);
		}
	}

	return fn;
}

// static
size_t RenISurfBody::defaultHeight()
{
	static bool first = true;
	static size_t size = 12;

	if (first)
	{
		first = false;

		const char* envVar = getenv("CB_RENDER_FONT");
		if (envVar)
		{
			char* copy = strdup(envVar);
			strtok(copy, ":");
			const char* sizeStr = strtok(NULL, ":");

			if (sizeStr)
				size = atoi(sizeStr);

			free(copy);
		}
	}

	return size;
}

size_t RenISurfBody::useFontHeight(size_t pixelHeight)
{
    // Decrease to fit in game scale
    pixelHeight -= 2;

    if(currentHeight_ == 0)
        currentHeight_ = pixelHeight;

	if(fonts_.empty())
	{
        // Set this whatever results are
        currentHeight_ = pixelHeight;

        FT_Library ft;
        FT_Face face;
        /* Initialize the FreeType2 library */
        if (FT_Init_FreeType(&ft))
        {
            std::cerr << "Could not init freetype library!" << std::endl;
            return 0;
        }

        /* Load a font */
        SysPathName fontFile ("gui/" + (fontName() + ".ttf"));
        if (FT_New_Face(ft, fontFile.pathname().c_str(), 0, &face))
        {
            std::cerr << "Could not open font " << fontFile << std::endl;
            return 0;
        }
        /* Create texture atlasses for font sizes */
        fonts_.push_back(Font(face, pixelHeight));
    }

	return pixelHeight;
}

size_t RenISurfBody::currentFontHeight() const
{
	if (currentHeight_ == 0)
		return _CONST_CAST(RenISurfBody*, this)->useFontHeight(defaultHeight());
	else
		return fonts_[currentHeight_].actualHeight_;
}

void RenISurfBody::drawText(int x, int y, const std::string& text, const RenColour& col)
{
    if (currentHeight_ == 0)
		useFontHeight(defaultHeight());

    if(!fonts_.empty())
    {
        int originX = x;
        std::vector <RenIVertex> vertices;
        vertices.reserve(text.size() * 6);
        uint fontColor = packColour(col.r(), col.g(), col.b(), 1.0);
        y += currentHeight_;
        Font& font = fonts_.front();
        for (int i = 0; i < text.size(); ++i)
        {
            uint character = text[i];
            if(character == '\n')
            {
                x = originX;
                y += currentHeight_ + 2;
                continue;
            }
            // Ignore non ascii at present
            if(character >= 128)
                continue;

            float x2 = x + font.c[character].bl ;
            float y2 = y - font.c[character].bt;
            float w = font.c[character].bw ;
            float h = font.c[character].bh ;

            /* Advance the cursor to the start of the next character */
            x += font.c[character].ax ;
            y += font.c[character].ay ;

            /* Skip glyphs that have no pixels */
            if (w <= 0 || h <= 0)
                continue;

            RenIVertex vx;
            vx.color = fontColor;
            vx.z = 0;
            // Calculate some common coordinate values
            float x1 = x2 + w, y1 = y2 + h;
            float tu1 = font.c[character].tx, tv1 = font.c[character].ty;
            float tu2 = font.c[character].tx2, tv2 = font.c[character].ty2;
            vx.x = x2; vx.y = y2;       vx.tu = tu1;   vx.tv = tv1;
            vertices.push_back(vx);
            vx.x = x1; vx.y = y2;   vx.tu = tu2;   vx.tv = tv1;
            vertices.push_back(vx);
            vx.x = x2; vx.y = y1;   vx.tu = tu1;   vx.tv = tv2;
            vertices.push_back(vx);
            vx.x = x1; vx.y = y2;   vx.tu = tu2;   vx.tv = tv1;
            vertices.push_back(vx);
            vx.x = x2; vx.y = y1;   vx.tu = tu1;   vx.tv = tv2;
            vertices.push_back(vx);
            vx.x = x1; vx.y = y1;   vx.tu = tu2;   vx.tv = tv2;
            vertices.push_back(vx);
        }
        glDisable(GL_CULL_FACE);
        RenDevice::current()->renderScreenspace(&vertices.front(), vertices.size(), GL_TRIANGLES, width_, height_, font.tex);
        glEnable(GL_CULL_FACE);
    }
}

void RenISurfBody::textDimensions(const std::string& text, Ren::Rect* dimensions) const
{
	PRE(dimensions);

	if (currentHeight_ == 0)
		_CONST_CAST(RenISurfBody*, this)->useFontHeight(defaultHeight());

	ASSERT(currentHeight_ > 0, "Failed to create default Windows font.");
	ASSERT(fonts_[currentHeight_].isDefined(), "Failed to get valid font.");

	// Prefer our own faster implementation of bitmap fonts.
	if (fonts_[currentHeight_].renFont_)
	{
		fonts_[currentHeight_].renFont_->textDimensions(text, dimensions);
	}
	else
	{
		// A RenIFont is unavailable -- use the Windows GDI instead.
	}
}

void RenISurfBody::releaseDC()
{
	//Delete texture
    if( textureID_ != NULL )
    {
        glDeleteTextures( 1, &textureID_ );
        textureID_ = NULL;
    }

}


static void computeScaleAndShift
(
	unsigned long bitMask,
	int& shift,
	int& scale
)
{
	unsigned long m = bitMask;
	int s;
    for (s = 0; !(m & 1); s++)
		m >>= 1;

    shift = s;
    scale = 255 / (bitMask >> shift);
}

bool RenISurfBody::copyWithAlpha
(
	SDL_Surface* surface,
	SDL_Surface* surfaceAlpha,
    bool createMipmaps
)
{
    SDL_Surface* surfaceDst = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Surface* surfaceTmp = SDL_ConvertSurfaceFormat(surfaceAlpha, SDL_PIXELFORMAT_ARGB8888, 0);

    Uint32* pixelsDst = (Uint32 *)surfaceDst->pixels;
    Uint32* pixelsSrc = (Uint32 *)surfaceTmp->pixels;
    for (int y = 0; y < surfaceDst->h; y++)
    {
        for (int x = 0; x < surfaceDst->w; x++)
        {
            Uint32 index = y * surfaceDst->w + x;
            Uint32 pixel = pixelsDst[index] & 0xFFFFFF00;
            pixelsDst[index] = pixel | (pixelsSrc[index] & 0x000000FF);
        }
    }
    glBindTexture(GL_TEXTURE_2D, textureID_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surfaceDst->w, surfaceDst->h,
                    GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, surfaceDst->pixels);
    if (createMipmaps && surfaceDst->w > 128 && surfaceDst->h > 128) 
    {
        glGenerateMipmap(GL_TEXTURE_2D); 
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    width_ = surface->w; height_ = surface->h;

    // unbind
	glBindTexture(GL_TEXTURE_2D, NULL);
	SDL_FreeSurface(surfaceDst);
	SDL_FreeSurface(surfaceTmp);
	return true;
}

bool RenISurfBody::copyWithColourKeyEmulation
(
	SDL_Surface* surface,
	const RenColour& keyColour,
    bool createMipmaps
)
{
    // Convert to RGBA and set alpha 0 for key magenta colour
	SDL_Surface* surfaceTmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_SetColorKey(surfaceTmp, SDL_TRUE, SDL_MapRGB( surfaceTmp->format, 0xFF, 0x0, 0xFF ));

	SDL_Surface* surfaceDst = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_BlitSurface(surfaceTmp, NULL, surfaceDst, NULL);

    glBindTexture(GL_TEXTURE_2D, textureID_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surfaceDst->w, surfaceDst->h,
                    GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, surfaceDst->pixels);
    if (createMipmaps && surfaceDst->w > 128 && surfaceDst->h > 128)
    {
        glGenerateMipmap(GL_TEXTURE_2D); 
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    width_ = surface->w; height_ = surface->h;

    // unbind
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(surfaceTmp);
	SDL_FreeSurface(surfaceDst);
	return true;
}

bool RenISurfBody::copyFromBuffer(const uint* pixelsBuffer)
{
    glBindTexture(GL_TEXTURE_2D, textureID_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_,
                    GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixelsBuffer);

    // unbind
	glBindTexture(GL_TEXTURE_2D, NULL);
	return true;
}


bool RenISurfBody::restoreToVRAM() const
{

    return true;
}

bool RenISurfBody::loadIntoVRAM() const
{
	return false;
}

bool RenISurfBody::recreateVRAMSurface()
{
	return true;
}

bool RenISurfBody::isEmpty() const
{
	return width() == 0 || height() == 0;
}

size_t RenISurfBody::memoryUsed() const
{
	// Conceivably, this might not be a 100% accurate figure depending on how
	// the bits are packed.  (The texture could even use compressed storage.)
	return (width() * height() * bitDepth()) / 8;
}

void RenISurfBody::incRefCount()
{
	++refCount_;
}

void RenISurfBody::decRefCount()
{
	--refCount_;
}

uint RenISurfBody::refCount() const
{
	return refCount_;
}

const RenIPixelFormat& RenISurfBody::pixelFormat() const
{
	return pixelFormat_;
}

size_t RenISurfBody::width() const
{
	//return descr_.dwWidth;
	return width_;
}

size_t RenISurfBody::height() const
{
	//return descr_.dwHeight;
	return height_;
}

size_t RenISurfBody::bitDepth() const
{
	return pixelFormat().totalDepth();
}

const std::string& RenISurfBody::sharedName() const
{
	if( sharedLeaf_ )
		return leafName_;
	else
		return name_;
}

void RenISurfBody::shareLeafName( bool shared )
{
	sharedLeaf_ = shared;
}

const std::string& RenISurfBody::name() const
{
	return name_;
}

void RenISurfBody::name(const std::string& n)
{
	PRE(name().length() == 0);
	name_ = n;

	// Note : original code below is inefficient ( probably more portable though ).
	//const SysPathName pathName = n;
	const SysPathName pathName(n);
	ASSERT(pathName.components().size() > 0, "");
	leafName_ = pathName.components().back();

	// Above, inefficient code, restored due to change in SysPathName breaking following code...
	// If the given string is a pathname, set leafName_ to be the last component.
	//char* leafName = strrchr( name_.c_str(), '/' );
	//if ( leafName )
	//{
	//	leafName_ = ++leafName;
	//}
	//else
	//{
	//	leafName_ = name_;
	//}
}

bool RenISurfBody::sharable() const
{
	return sharable_;
}

bool RenISurfBody::readOnly() const
{
	return readOnly_;
}

void RenISurfBody::makeReadOnlySharable()
{
	// You can't change this property on a shared surface, however, if it's
	// already read-only, then it doesn't make any difference.
	PRE(readOnly() || !sharable());

	readOnly_ = true;
	sharable_ = true;

	POST(sharable() && readOnly());
}

// virtual
RenITexBody* RenISurfBody::castToTexBody()
{
	return NULL;
}

// virtual
const RenITexBody* RenISurfBody::castToTexBody() const
{
	return NULL;
}

// virtual
void RenISurfBody::print(ostream& o) const
{
	o << "surface ";

	if (name().length() > 0)
    	o << name() << " ";

	switch (displayType_)
	{
		case RenI::BACK:	o << "back-buffer ";  break;
		case RenI::FRONT:	o << "front-buffer "; break;
	}

	o << "(" << width() << "x" << height() << "x" << bitDepth() << ")";
}

bool RenISurfBody::matches(const std::string& name) const
{
	return strcasecmp(sharedName().c_str(), name.c_str()) == 0;
}

ostream& operator <<( ostream& o, const RenISurfBody& t )
{
	t.print(o);
    return o;
}

void RenISurfBody::updateDescr()
{
	
}

/* End INSURFCE.CPP *************************************************/
