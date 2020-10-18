/*
 * B L I T A B L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/surface.hpp"

#include <string>
#include "stdlib/memory.hpp"

#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "afx/resource.hpp"
#include "render/surfmgr.hpp"
#include "render/internal/surfbody.hpp"
#include "render/internal/pixelfmt.hpp"
#include "render/internal/surfmgri.hpp"
#include "render/internal/linediag.hpp"
#include "render/internal/colpack.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/device.hpp"

#include <stdio.h>

//--------------------------------Creation & destruction--------------------------------
// static
RenSurface RenSurface::createAnonymousSurface(size_t w, size_t h, const RenSurface& surf)
{
	// This is pure delegation -- let the manager do the pre and post-conditions.
	return RenSurfaceManager::instance().createAnonymousSurface(w, h, surf);
}

// Static.
RenSurface RenSurface::createAnonymousVideoSurface(
		size_t width,
		size_t height,
		const RenSurface& pixelFmt)
{
	// The pre and post-conditions are in the surface manager.
	// PRE(!pixelFmt.isNull());
	// PRE(width > 0 and height > 0);

	return RenSurfaceManager::instance().createAnonymousVideoSurface(width, height, pixelFmt);

	// POST(!result.sharable() and !result.readOnly());
	// POST(name.length() == 0);
}

// static
RenSurface RenSurface::createAnonymousSurface( PerIstream& inStream, const RenSurface& surf)
{
	size_t w, h;

	PER_READ_RAW_OBJECT( inStream, w );
	PER_READ_RAW_OBJECT( inStream, h );

	// This is pure delegation -- let the manager do the pre and post-conditions.
	RenSurface retVal = RenSurfaceManager::instance().createAnonymousSurface(w, h, surf);

	retVal.read( inStream );

	return retVal;
}

// static
RenSurface RenSurface::createSharedSurface
(
	const std::string& bitmapName,
	const RenSurface& surf
)
{
	// This is pure delegation -- let the manager do the pre and post-conditions.
	return RenSurfaceManager::instance().createSharedSurface(bitmapName, surf);
}

// static
RenSurface RenSurface::createSharedSurface
(
	const std::string& bitmapName,
	const AfxResourceLib& lib,
	const RenSurface& surf
)
{
	// This is pure delegation -- let the manager do the pre and post-conditions.
	return RenSurfaceManager::instance().createSharedSurface(bitmapName, lib, surf);
}

// static
RenSurface RenSurface::createFromInternal(RenISurfBody* body)
{
	return RenSurface(RenSurfaceManager::instance().impl().getBodyId(body));
}

// static
const RenSurface RenSurface::createFromInternal(const RenISurfBody* body)
{
	return RenSurface(RenSurfaceManager::instance().impl().getBodyId(body));
}

RenSurface::RenSurface(Ren::TexId id):
	myId_(id)
{
	PRE(Ren::initialised());
	RenSurfaceManager::instance().impl().incRefCount(myId_);
    TEST_INVARIANT;
}

RenSurface::RenSurface():
	myId_(Ren::NullTexId)
{
	PRE(Ren::initialised());
	RenSurfaceManager::instance().impl().incRefCount(myId_);
    TEST_INVARIANT;
	POST(isNull() && sharable() && readOnly());
}

RenSurface::RenSurface(const RenSurface& tex):
	myId_(tex.myId_)
{
	RenSurfaceManager::instance().impl().incRefCount(myId_);
    TEST_INVARIANT;
}

RenSurface& RenSurface::operator =( const RenSurface& rhs)
{
	if (*this != rhs)
	{
		RenSurfaceManager::instance().impl().decRefCount(myId_);
		myId_ = rhs.myId_;
		RenSurfaceManager::instance().impl().incRefCount(myId_);
	}

	return *this;
}

RenSurface::~RenSurface()
{
    TEST_INVARIANT;
	RenSurfaceManager::instance().impl().decRefCount(myId_);
}

//------------------------------------Blitting---------------------------------------
void RenSurface::simpleBlit(const RenSurface& source, const Rect& srcArea, int destX, int destY)
{
	PRE_INFO(*this);
	PRE(!readOnly());
	PRE(!empty());
	PRE(!source.empty());

	RENDER_STREAM("Simple blit before clipping:\n");
	RENDER_STREAM("  from " << srcArea << " of " << source << "\n");
	RENDER_STREAM("  to (" << destX << "," << destY << ") of " << (*this) << "\n");

	const int srcW = source.width();
	const int srcH = source.height();
	const int intW = _STATIC_CAST(int, width());
	const int intH = _STATIC_CAST(int, height());

	if (srcArea.originX >= srcW || srcArea.originY >= srcH)
		return;

	// Check to see if the destination area is completely outside this surface.
	if (destX > intW || destY > intH || destX + srcArea.width <= 0 || destY + srcArea.height <= 0)
		return;

	// Clip the source rectangle so that the blit will lie in the destination.
	Rect tmp = srcArea;
	if (destX < 0)
	{
		tmp.originX -= destX;
		tmp.width   += destX;
		destX = 0;
	}

	if (destY < 0)
	{
		tmp.originY -= destY;
		tmp.height  += destY;
		destY = 0;
	}

	// Again, clip against the dest rectangle, this time for the maximum coords.
	const int rightMost = destX + tmp.width, bottomMost = destY + tmp.height;

	if (rightMost > intW)
		tmp.width -= (rightMost - intW);

	if (bottomMost > intH)
		tmp.height -= (bottomMost - intH);

	// Clip the source area so that it lies within the source surface.
	if (tmp.originX < 0)
	{
		tmp.width  += tmp.originX;
		destX      -= tmp.originX;
		tmp.originX = 0;
	}

	if (tmp.originY < 0)
	{
		tmp.height += tmp.originY;
		destY      -= tmp.originY;
		tmp.originY = 0;
	}

	const int overshootRight = tmp.originX + tmp.width - srcW;
	if (overshootRight > 0)
		tmp.width -= overshootRight;

	const int overshootBottom = tmp.originY + tmp.height - srcH;
	if (overshootBottom > 0)
		tmp.height -= overshootBottom;

	if (tmp.width <= 0 || tmp.height <= 0)
		return;

	ASSERT(tmp.originX >= 0, "Clipping logic error.");
	ASSERT(tmp.originY >= 0, "Clipping logic error.");
	ASSERT(tmp.originX + tmp.width  <= srcW, "Clipping logic error.");
	ASSERT(tmp.originY + tmp.height <= srcH, "Clipping logic error.");

	internals()->unclippedBlit(source.internals(), tmp, destX, destY);
}

// Copy the entire source to this.
void RenSurface::simpleBlit(const RenSurface& source, int destX, int destY)
{
	PRE(!readOnly());
	PRE(!empty());
	PRE(!source.empty());

	RENDER_STREAM("Simple blit before clipping:\n");
	RENDER_STREAM("  from all of " << source << "\n");
	RENDER_STREAM("  to (" << destX << "," << destY << ") of " << (*this) << "\n");

	const int srcW = source.width();
	const int srcH = source.height();
	const int intW = _STATIC_CAST(int, width());
	const int intH = _STATIC_CAST(int, height());

	// Check to see if the destination area is completely outside this surface.
	if (destX > intW || destY > intH || destX + srcW <= 0 || destY + srcH <= 0)
		return;

	// Clip the source rectangle so that the blit will lie in the destination.
	Rect tmp(0,0, srcW, srcH);
	if (destX < 0)
	{
		tmp.originX -= destX;
		tmp.width   += destX;
		destX = 0;
	}

	if (destY < 0)
	{
		tmp.originY -= destY;
		tmp.height  += destY;
		destY = 0;
	}

	// Again, clip against the dest rectangle, this time for the maximum coords.
	const int rightMost = destX + tmp.width, bottomMost = destY + tmp.height;

	if (rightMost > intW)
		tmp.width -= (rightMost - intW);

	if (bottomMost > intH)
		tmp.height -= (bottomMost - intH);

	if (tmp.width <= 0 || tmp.height <= 0)
		return;

	ASSERT(tmp.originX >= 0, "Clipping logic error.");
	ASSERT(tmp.originY >= 0, "Clipping logic error.");
	ASSERT(tmp.originX + tmp.width  <= srcW, "Clipping logic error.");
	ASSERT(tmp.originY + tmp.height <= srcH, "Clipping logic error.");

	internals()->unclippedBlit(source.internals(), tmp, destX, destY);
}

void RenSurface::tileBlit(const RenSurface& source, const Rect& srcArea, const Rect& destArea)
{
	PRE(!readOnly());
	PRE(srcArea.width   <= destArea.width);
	PRE(srcArea.height  <= destArea.height);
	PRE(srcArea.width   <= source.width());
	PRE(srcArea.height  <= source.height());
	PRE(destArea.width  <= width());
	PRE(destArea.height <= height());
	PRE(destArea.originX + destArea.width  <= width());
	PRE(destArea.originY + destArea.height <= height());

	// First copy however many whole repeats of the source are necessary.
	const int wholeXTiles = destArea.width  / srcArea.width;
	const int wholeYTiles = destArea.height / srcArea.height;

	int destX = destArea.originX, destY = destArea.originY;

	for (int x=0; x!=wholeXTiles; ++x)
	{
		for (int y=0; y!=wholeYTiles; ++y)
		{
			internals()->unclippedBlit(source.internals(), srcArea, destX, destY);
			destY += srcArea.height;
		}

		destX += srcArea.width;
		destY = destArea.originY;
	}

	// Partial tiles may be needed at the right and bottom edges of the dest.
	const int extraX = destArea.width  - (wholeXTiles * srcArea.width);
	const int extraY = destArea.height - (wholeYTiles * srcArea.height);
	ASSERT(extraX >= 0, logic_error());
	ASSERT(extraY >= 0, logic_error());

	// First, do any partial tiles down the right-hand edge.
	if (extraX > 0)
	{
		int destX = destArea.originX + wholeXTiles * srcArea.width;
		int destY = destArea.originY;

		Rect subArea  = srcArea;
		subArea.width = extraX;

		for (int y=0; y!=wholeYTiles; ++y)
		{
			internals()->unclippedBlit(source.internals(), subArea, destX, destY);
			destY += srcArea.height;
		}
	}

	// Then, do any partial tiles along the bottom-hand edge.
	if (extraY > 0)
	{
		int destX = destArea.originX;
		int destY = destArea.originY + wholeYTiles * srcArea.height;

		Rect subArea   = srcArea;
		subArea.height = extraY;

		for (int x=0; x!=wholeXTiles; ++x)
		{
			internals()->unclippedBlit(source.internals(), subArea, destX, destY);
			destX += srcArea.width;
		}
	}


	// Finally, fill in the bottom-right corner, if necessary.
	if (extraX > 0 && extraY > 0)
	{
		int destX = destArea.originX + wholeXTiles * srcArea.width;
		int destY = destArea.originY + wholeYTiles * srcArea.height;

		Rect subArea   = srcArea;
		subArea.width  = extraX;
		subArea.height = extraY;

		internals()->unclippedBlit(source.internals(), subArea, destX, destY);
	}
}

inline bool surfaceContainsRect(const RenSurface& surf, const Ren::Rect& rect)
{
	return	rect.originX >= 0 &&
			rect.originY >= 0 &&
			rect.originX + rect.width  <= surf.width() &&
			rect.originY + rect.height <= surf.height();
}

void RenSurface::stretchBlit(const RenSurface& source, const Rect& srcArea, const Rect& destArea)
{
	PRE(!readOnly());

	// Unlike simple blit, clipping the areas for a stretch blit would be very
	// complicated (you'd have to muck about with ratios and so on).  So use
	// pre-conditions to ensure that the rectangles are within bounds.
	PRE(surfaceContainsRect(source, srcArea));
	PRE(surfaceContainsRect(*this, destArea));

	internals()->unclippedStretchBlit(source.internals(), srcArea, destArea);
}

void RenSurface::copyFromRGBABuffer(const uint* buff)
{
    internals()->copyFromBuffer(buff);
}


//--------------------------------2D Drawing Primitives-----------------------------
void RenSurface::filledRectangle(const Rect& area, const RenColour& colour)
{
	PRE(!readOnly());

	if (internals())
    	internals()->filledRectangle(area, packColour(colour.r(), colour.g(), colour.b(), colour.a()));
}

void RenSurface::hollowRectangle(const Ren::Rect& area, const RenColour& col, int thickness)
{
	PRE(!readOnly());
	PRE(thickness > 0);

	const int x2 = area.originX + area.width;
	const int y2 = area.originY + area.height;
	// Note: DrawPrimitive appears unable to draw lines wider than 1 pixel.
//	if (device && thickness == 1)
	{
		// Due to the bad drawing capabilities of some cards we must use out own primitives.
		static RenIVertex vertices[8];
		static bool initialised = false;

		if(not initialised)
		{
			initialised = true;

			// This may look overly verbose but speed is key in here.

			vertices[0].z = 0.0;
			vertices[0].specular = 0;
			vertices[0].w = 0.1;
			vertices[0].tu = vertices[0].tv = 0.1;

			vertices[1].z = 0.0;
			vertices[1].specular = 0;
			vertices[1].w = 0.1;
			vertices[1].tu = vertices[1].tv = 0.1;

			vertices[2].z = 0.0;
			vertices[2].specular = 0;
			vertices[2].w = 0.1;
			vertices[2].tu = vertices[2].tv = 0.1;

			vertices[3].z = 0.0;
			vertices[3].specular = 0;
			vertices[3].w = 0.1;
			vertices[3].tu = vertices[3].tv = 0.1;

			vertices[4].z = 0.0;
			vertices[4].specular = 0;
			vertices[4].w = 0.1;
			vertices[4].tu = vertices[4].tv = 0.1;

			vertices[5].z = 0.0;
			vertices[5].specular = 0;
			vertices[5].w = 0.1;
			vertices[5].tu = vertices[5].tv = 0.1;

			vertices[6].z = 0.0;
			vertices[6].specular = 0;
			vertices[6].w = 0.1;
			vertices[6].tu = vertices[6].tv = 0.1;

			vertices[7].z = 0.0;
			vertices[7].specular = 0;
			vertices[7].w = 0.1;
			vertices[7].tu = vertices[7].tv = 0.1;
		}

		Ren::Rect orderedArea = area;

		// The two vertices of a rectangle must be in an order for this code to be fast enough.
		if(orderedArea.width < 0)
		{
			orderedArea.originX += orderedArea.width;
			orderedArea.width = -orderedArea.width;
		}

		if(orderedArea.height < 0)
		{
			orderedArea.originY += orderedArea.height;
			orderedArea.height = -orderedArea.height;
		}

		uint packedColour = packColour(col.r(), col.g(), col.b(), col.a());

		switch(RenILinesDiagnostic::instance().horizontalResult())
		{
		case RenILinesDiagnostic::UNKNOWN:
			//ASSERT_FAIL("Unknown line classification.");
			// Fall through in release and production versions.  This will
			// produce lines although not necessarilly 100% correct lines.
		case RenILinesDiagnostic::NORMAL:
		case RenILinesDiagnostic::SORTED:
		case RenILinesDiagnostic::LARGE:
			// Draw bottom horizontal line.
			vertices[2].x =orderedArea.originX;
			vertices[2].y =orderedArea.originY + orderedArea.height;
			vertices[2].color = packedColour;
			vertices[3].x =orderedArea.originX + orderedArea.width;
			vertices[3].y =orderedArea.originY + orderedArea.height;
			vertices[3].color = packedColour;

			// Draw top horizontal line.
			vertices[6].x =orderedArea.originX;
			vertices[6].y =orderedArea.originY;
			vertices[6].color = packedColour;
			vertices[7].x =orderedArea.originX + orderedArea.width;
			vertices[7].y =orderedArea.originY;
			vertices[7].color = packedColour;
			break;

		case RenILinesDiagnostic::OFFSET_SORTED:
			// Draw bottom horizontal line.
			vertices[2].x =orderedArea.originX;
			vertices[2].y =orderedArea.originY + orderedArea.height + 1;
			vertices[2].color = packedColour;
			vertices[3].x =orderedArea.originX + orderedArea.width;
			vertices[3].y =orderedArea.originY + orderedArea.height + 1;
			vertices[3].color = packedColour;

			// Draw top horizontal line.
			vertices[6].x =orderedArea.originX;
			vertices[6].y =orderedArea.originY + 1;
			vertices[6].color = packedColour;
			vertices[7].x =orderedArea.originX + orderedArea.width;
			vertices[7].y =orderedArea.originY + 1;
			vertices[7].color = packedColour;
			break;

		default:
			ASSERT_FAIL("Invalid switch parameter.");
			break;
		}

		switch(RenILinesDiagnostic::instance().verticalResult())
		{
		case RenILinesDiagnostic::UNKNOWN:
			//ASSERT_FAIL("Unknown line classification.");
			// Fall through in release and production versions.  This will
			// produce lines although not necessarilly 100% correct lines.
		case RenILinesDiagnostic::NORMAL:
		case RenILinesDiagnostic::SORTED:
			// Draw left vertical line.
			vertices[0].x =orderedArea.originX;
			vertices[0].y =orderedArea.originY;
			vertices[0].color = packedColour;
			vertices[1].x =orderedArea.originX;
			vertices[1].y =orderedArea.originY + orderedArea.height;
			vertices[1].color = packedColour;

			// Draw right vertical line.
			vertices[4].x =orderedArea.originX + orderedArea.width;
			vertices[4].y =orderedArea.originY;
			vertices[4].color = packedColour;
			vertices[5].x =orderedArea.originX + orderedArea.width;
			vertices[5].y =orderedArea.originY + orderedArea.height + 1;
			vertices[5].color = packedColour;
			break;

		case RenILinesDiagnostic::LARGE:
			// Draw left vertical line.
			vertices[0].x =orderedArea.originX;
			vertices[0].y =orderedArea.originY;
			vertices[0].color = packedColour;
			vertices[1].x =orderedArea.originX;
			vertices[1].y =orderedArea.originY + orderedArea.height;
			vertices[1].color = packedColour;

			// Draw right vertical line.
			vertices[4].x =orderedArea.originX + orderedArea.width;
			vertices[4].y =orderedArea.originY;
			vertices[4].color = packedColour;
			vertices[5].x =orderedArea.originX + orderedArea.width;
			vertices[5].y =orderedArea.originY + orderedArea.height;
			vertices[5].color = packedColour;
			break;

		case RenILinesDiagnostic::OFFSET_SORTED:
			// Draw left vertical line.
			vertices[0].x =orderedArea.originX + 1;
			vertices[0].y =orderedArea.originY;
			vertices[0].color = packedColour;
			vertices[1].x =orderedArea.originX + 1;
			vertices[1].y =orderedArea.originY + orderedArea.height;
			vertices[1].color = packedColour;

			// Draw right vertical line.
			vertices[4].x =orderedArea.originX + orderedArea.width + 1;
			vertices[4].y =orderedArea.originY;
			vertices[4].color = packedColour;
			vertices[5].x =orderedArea.originX + orderedArea.width + 1;
			vertices[5].y =orderedArea.originY + orderedArea.height + 1;
			vertices[5].color = packedColour;
			break;

		default:
			ASSERT_FAIL("Invalid switch parameter.");
			break;
		}

        glLineWidth((float)thickness);

        RenDevice* dev = RenDevice::current();
        if(internals() && internals()->isOffscreen())
        {
            dev->renderToTextureMode(handle(), width(), height());
            if(packedColour == 0xFFFF00FF) // Ugly hack to handle background colour
            {
                GLint blendSrc, blendDst;
                glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
                glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
                glBlendFunc(GL_ZERO, GL_ZERO);
                dev->renderScreenspace(vertices, 8, GL_LINES, width(), height());
                glBlendFunc(blendSrc, blendDst);
            }
            else
                dev->renderScreenspace(vertices, 8, GL_LINES, width(), height());
            dev->renderToTextureMode(0,0,0);
        }
        else
            dev->renderScreenspace(vertices, 8, GL_LINES, width(), height());

        glLineWidth(1.0);
	}
}

void RenSurface::getPixel(int x, int y, RenColour* colour) const
{
	PRE(colour);

	GLfloat pixel[4] = {0,0,0,0};
    if(internals() && internals()->isOffscreen())
    {
        RenDevice* dev = RenDevice::current();
        dev->renderToTextureMode(handle(), width(), height());
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);
        dev->renderToTextureMode(0,0,0);
    }
    else
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);

    colour->r(pixel[0]);
	colour->g(pixel[1]);
	colour->b(pixel[2]);
	colour->a(pixel[3]);
}

void RenSurface::setPixel(int x, int y, const RenColour& colour)
{
	PRE(!readOnly());
    ASSERT_FAIL("Not implemented.");
}

enum LineOrientation
{
	HORIZONTAL,
	VERTICAL,
	DIAGONAL
};

static LineOrientation lineOrientation(const MexPoint2d& vertex1, const MexPoint2d& vertex2)
{
	LineOrientation result = DIAGONAL;

	if(vertex1.y() == vertex2.y())
	{
		if(vertex1.x() != vertex2.x())
			result = HORIZONTAL;
	}
	else if(vertex1.x() == vertex2.x())
		result = VERTICAL;

	return result;
}

//#define REN_LINE_STREAM(x)		RENDER_STREAM(x)
//#define REN_LINE_INDENT(x)		RENDER_INDENT(x)
#define REN_LINE_STREAM(x)		;
#define REN_LINE_INDENT(x)		;

void RenSurface::polyLine(const Points& pts, const RenColour& colour, int thickness)
{
	PRE(!readOnly());
	PRE(pts.size() > 1);
	PRE(thickness > 0);

	REN_LINE_STREAM(std::endl << "RenSurface::polyLine()" << std::endl);
	REN_LINE_STREAM('{' << std::endl);
	REN_LINE_INDENT(3);
	REN_LINE_STREAM("colour = " << colour << std::endl);
	REN_LINE_STREAM("thickness = " << thickness << std::endl);
	REN_LINE_STREAM("vertices : size() = " << pts.size() << std::endl);
	REN_LINE_STREAM('{' << std::endl);
	REN_LINE_INDENT(1);
	REN_LINE_STREAM(pts);
	REN_LINE_INDENT(-1);
	REN_LINE_STREAM('}' << std::endl);
	REN_LINE_INDENT(-3);
	REN_LINE_STREAM('}' << std::endl << std::endl);

	// Note: DrawPrimitive appears unable to draw lines wider than 1 pixel.
//	if (device && thickness == 1)
	{
		// There *is* a Direct3D device associated with this surface, so we
		// can use DrawPrimitive and avoid the overheads of getting a DC.
		static size_t nVertices = 30;
		static auto_ptr_array< RenIVertex > vtx = _NEW_ARRAY(RenIVertex, nVertices);
		static bool initialised = false;

		if(nVertices < pts.size() + (pts.size() - 2))
		{
			nVertices = pts.size() + 10;
			RenIVertex * pD = vtx.release();
			_DELETE_ARRAY( pD );
			vtx = _NEW_ARRAY(RenIVertex, nVertices);
			initialised = false;
		}

		// Don't bother initialising the const stuff every frame.
		if(not initialised)
		{
			initialised = true;

			// We turn the z-buffer off, so any z values will do.
			// There's no texture, so rhw and uv are left uninitialised.
			for (int i=0; i!=nVertices; ++i)
			{
                vtx.get()[i].z = 0.0;
				vtx.get()[i].specular = 0;
				vtx.get()[i].w = 0.1;
				vtx.get()[i].tu = vtx.get()[i].tv = 0.1;
			}
		}

		uint packedColour = packColour(colour.r(), colour.g(), colour.b(), colour.a());

		// If the lines diagnostic has performed its' tests, apply the results.
		if(RenILinesDiagnostic::instance().hasTestedLines())
		{
			int index = 0;
			int vtxIndex = 0;
			int x1 = 0;
			int x2 = 0;
			int y1 = 0;
			int y2 = 0;

			while(index < pts.size() -1)
			 {
				switch(lineOrientation(pts[index], pts[index+1]))
				{
				case HORIZONTAL:
					switch(RenILinesDiagnostic::instance().horizontalResult())
					{
					// The vertices have to be sorted.
					case RenILinesDiagnostic::NORMAL:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						if(x1 > x2)
						{
							std::swap(x1, x2);
							std::swap(y1, y2);
						}
						break;

					// This is the default drawing style just cast and assign.
					case RenILinesDiagnostic::UNKNOWN:
						// TODO fix it, sometimes called
						//ASSERT_FAIL("Unknown line classification.");
						// Fall through in release and production versions.  This will
						// produce lines although not necessarilly 100% correct lines.
					case RenILinesDiagnostic::SORTED:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());
						break;

					// This needs moving but not sorting.
					case RenILinesDiagnostic::OFFSET_SORTED:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						++y1;
						++y2;
						break;

					case RenILinesDiagnostic::LARGE:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						if(x1 > x2)
						{
							std::swap(x1, x2);
							std::swap(y1, y2);
						}

						--x2;
						break;

					default:
						ASSERT_FAIL("Invalid switch parameter.");
						break;
					}
					break;

				case VERTICAL:
					switch(RenILinesDiagnostic::instance().verticalResult())
					{
					// The vertices have to be sorted.
					case RenILinesDiagnostic::NORMAL:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						if(y1 > y2)
						{
							std::swap(x1, x2);
							std::swap(y1, y2);
						}
						break;

					// This is the default drawing style just cast and assign.
					// UNKNOWN is assigned to this because it's the most common.
					case RenILinesDiagnostic::UNKNOWN:
						// TODO fix this
						//ASSERT_FAIL("Unknown line classification.");
						// Fall through in release and production versions.  This will
						// produce lines although not necessarilly 100% correct lines.
					case RenILinesDiagnostic::SORTED:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());
						break;

					// This needs moving but not sorting.
					case RenILinesDiagnostic::OFFSET_SORTED:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						++x1;
						++x2;
						break;

					case RenILinesDiagnostic::LARGE:
						x1 = _STATIC_CAST(int, pts[index].x());
						y1 = _STATIC_CAST(int, pts[index].y());
						x2 = _STATIC_CAST(int, pts[index+1].x());
						y2 = _STATIC_CAST(int, pts[index+1].y());

						if(y1 > y2)
						{
							std::swap(x1, x2);
							std::swap(y1, y2);
						}

						--y2;
						break;

					default:
						ASSERT_FAIL("Illegal switch parameter.");
						break;
					}
					break;

				// Diagonal is too hard to fudge so your left with the way the card does it.
				case DIAGONAL:
					x1 = _STATIC_CAST(int, pts[index].x());
					y1 = _STATIC_CAST(int, pts[index].y());
					x2 = _STATIC_CAST(int, pts[index+1].x());
					y2 = _STATIC_CAST(int, pts[index+1].y());
					break;
				}

				vtx.get()[vtxIndex].x = x1;
				vtx.get()[vtxIndex].y = y1;
				vtx.get()[vtxIndex++].color = packedColour;
				vtx.get()[vtxIndex].x = x2;
				vtx.get()[vtxIndex].y = y2;
				vtx.get()[vtxIndex++].color = packedColour;

				++index;
			} // while(index < pts.size() - 1)

			// They have to be rendered as a line list because otherwise as soon as
			// an unordered line popped up they would be buggered.
            RenDevice* dev = RenDevice::current();
            glLineWidth((float)thickness);
            if(internals() && internals()->isOffscreen())
            {
                dev->renderToTextureMode(handle(), width(), height());
                dev->renderScreenspace(vtx.get(), pts.size() + (pts.size() - 2), GL_LINES, width(), -height());
                dev->renderToTextureMode(0,0,0);
            }
            else
                dev->renderScreenspace(vtx.get(), pts.size()+ (pts.size() - 2), GL_LINES, width(), height());
            glLineWidth(1.0);
		}
		else
		{
			RENDER_STREAM("WARNING : Drawing without testing." << std::endl);

			Points::const_iterator it = pts.begin();
			int j = 0;

			while (it != pts.end())
			{
				vtx.get()[j].x = _STATIC_CAST(int, (*it).x());
				vtx.get()[j].y = _STATIC_CAST(int, (*it).y());
				vtx.get()[j].color = packedColour;

				RENDER_STREAM("  " << vtx.get()[j] << "\n");

				++it;
				++j;
			}

            RenDevice* dev = RenDevice::current();
            glLineWidth((float)thickness);
            if(internals() && internals()->isOffscreen())
            {
                dev->renderToTextureMode(handle(), width(), height());
                dev->renderScreenspace(vtx.get(), pts.size(), GL_LINE_STRIP, width(), -height());
                dev->renderToTextureMode(0,0,0);
            }
            else
                dev->renderScreenspace(vtx.get(), pts.size(), GL_LINE_STRIP, width(), height());
            glLineWidth(1.0);
		}
	}
}

size_t RenSurface::useFontHeight(size_t pixelHeight)
{
	return internals()->useFontHeight(pixelHeight);
}

size_t RenSurface::currentFontHeight() const
{
	return internals()->currentFontHeight();
}

void RenSurface::drawText(int x, int y, const std::string& text, const RenColour& col)
{
	internals()->drawText(x,y, text, col);
}

void RenSurface::textDimensions(const std::string& text, Ren::Rect* dimensions) const
{
	PRE(dimensions);
	internals()->textDimensions(text, dimensions);
}

//-----------------------------Simple properties & delegations-----------------------
bool RenSurface::empty() const
{
	return width() == 0 || height() == 0;
}

bool RenSurface::isColourKeyingOn() const
{
	return (isNull())? false: internals()->keyingOn();
}

void RenSurface::enableColourKeying()
{
	if (!internals()->keyingOn())
	{
		internals()->keyingOn(true);
		internals()->setDDColourKey();
	}
}

void RenSurface::disableColourKeying()
{
	internals()->keyingOn(false);
}

const RenColour& RenSurface::colourKey() const
{
	return internals()->keyColour();
}

void RenSurface::colourKey(const RenColour& c)
{
	internals()->keyColour(c);
	internals()->setDDColourKey();
}

RenISurfBody* RenSurface::internals()
{
	RenISurfBody* b = RenSurfaceManager::instance().impl().getSurface(myId_);
	ASSERT(b, "NULL internal surface.");
	return b;
}

const RenISurfBody* RenSurface::internals() const
{
	const RenISurfBody* b = RenSurfaceManager::instance().impl().getSurface(myId_);
	ASSERT(b, "NULL internal surface.");
	return b;
}

void RenSurface::makeReadOnlySharable()
{
	internals()->makeReadOnlySharable();
}

void RenSurface::name(const std::string& n)
{
	internals()->name(n);
}

bool RenSurface::isNull() const
{
	return myId_ == Ren::NullTexId;
}

const GLint RenSurface::handle() const
{
    return internals()->handle();
}

// Several methods are delegated to RenISurfBody using these macros.
#define TEX_FORWARD_GET(RETTYPE, METHOD)		\
RETTYPE RenSurface::METHOD() const				\
{												\
    TEST_INVARIANT;								\
	return RenSurfaceManager::instance().impl().getSurface(myId_)->METHOD();	\
}

TEX_FORWARD_GET(const std::string&,	name)
TEX_FORWARD_GET(bool,			sharable)
TEX_FORWARD_GET(bool,			readOnly)
TEX_FORWARD_GET(size_t,			width)
TEX_FORWARD_GET(size_t,			height)

//---------------------------------Canonnical stuff--------------------------------
bool operator==(const RenSurface& s1, const RenSurface& s2)
{
	return s1.myId_ == s2.myId_;
}

bool operator!=(const RenSurface& s1, const RenSurface& s2)
{
	return s1.myId_ != s2.myId_;
}

void RenSurface::CLASS_INVARIANT
{
	INVARIANT(internals());
	INVARIANT(internals()->refCount() > 0);
}

ostream& operator <<( ostream& o, const RenSurface& t )
{
	if (t.isNull())
		o << "(no surface)";
	else
		o << *t.internals();

    return o;
}

void RenSurface::saveAsBmp(const Rect& area, const SysPathName& filename ) const
{
	TEST_INVARIANT;

   	// Save the screen shot
   	unsigned char *screenPixels = _NEW_ARRAY(unsigned char, width()*height()*4);
    if (screenPixels)
    {
        // Read the pixels
        if(internals() && internals()->isOffscreen())
        {
            RenDevice* dev = RenDevice::current();
            dev->renderToTextureMode(handle(), width(), height());
            glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, screenPixels);
            dev->renderToTextureMode(0,0,0);
        }
        else
            glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, screenPixels);

        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
          SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(screenPixels, width(), height(), 32, width()*4,
          0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        #else
          SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(screenPixels, width(), height(), 32, width()*4,
          0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        #endif

        // Flip surface vertically because of OpenGL coordinates...
        // Code comes from https://halfgeek.org/wiki/Vertically_invert_a_surface_in_SDL
        Uint8 *t;
        Uint8 *a, *b;
        Uint8 *last;
        Uint16 pitch;

        /* get a place to store a line */
        pitch = surface->pitch;
        t = (Uint8*)malloc(pitch);

        if(t == NULL) {
            // mem error
        }

        /* get first line; it's about to be trampled */
        memcpy(t,surface->pixels,pitch);

        /* now, shuffle the rest so it's almost correct */
        a = (Uint8*)surface->pixels;
        last = a + pitch * (surface->h - 1);
        b = last;

        while(a < b) {
            memcpy(a,b,pitch);
            a += pitch;
            memcpy(b,a,pitch);
            b -= pitch;
        }

        /* in this shuffled state, the bottom slice is too far down */
        memmove( b, b+pitch, last-b );

        /* now we can put back that first row--in the last place */
        memcpy(last,t,pitch);

        /* everything is in the right place; close up. */
        free(t);

        // Write the file
        SDL_SaveBMP(surface, filename.pathname().c_str());

        // Free everything
        SDL_FreeSurface(surface);
        _DELETE_ARRAY( screenPixels );
    }

	TEST_INVARIANT;
}

void RenSurface::ellipse( const Rect& area, const RenColour& penColour, const RenColour& brushColour )
{
	PRE(!readOnly());

    int x, y, RX, RY;
    uint packedColour = packColour(brushColour.r(), brushColour.g(), brushColour.b(), brushColour.a());
    RX = area.width / 2; RY = area.height / 2;
    x = area.originX + RX; y = area.originY + RY;
    std::vector<RenIVertex> vertices;
    vertices.reserve(10);

	float i, inc, endAngle;
	endAngle = 3.1415 * 2;
	inc = endAngle / 10;
	i = 0;
	while(i <= endAngle)
	{
        RenIVertex vtx;
		vtx.x = ((RX*cos(i)+x));
		vtx.y = ((RY*sin(i)+y));
		vtx.color = packedColour;
		vertices.push_back(vtx);
		i += inc;
	}

	glDisable(GL_CULL_FACE);
	RenDevice* dev = RenDevice::current();
    if(internals() && internals()->isOffscreen())
    {
        dev->renderToTextureMode(handle(), width(), height());
        dev->renderScreenspace(vertices.data(), vertices.size(), GL_TRIANGLE_FAN, width(), -height());
        dev->renderToTextureMode(0,0,0);
    }
    else
        dev->renderScreenspace(vertices.data(), vertices.size(), GL_TRIANGLE_FAN, width(), height());
    glEnable(GL_CULL_FACE);
}

// These read/write functions are used for fog of war in savegame and store alpha only
void RenSurface::read( PerIstream& inStream )
{
	PRE( not readOnly() and not sharable() );

	size_t w = width();
	size_t h = height();

	size_t lineLength = w;

	uint* image     = _NEW_ARRAY( uint, w * h );
	char* imageLine = _NEW_ARRAY( char, lineLength );

    {
        uint pixel = 0;
		for ( size_t y = 0; y < h; ++y )
		{
   			PER_READ_RAW_DATA( inStream, imageLine, w );

			for ( size_t x = 0; x < w; ++x )
			{
//				colRef = RGB( image[ ( x * 3 ) + 2 ], image[ ( x * 3 ) + 1 ], image[ x * 3 ] );
//				SetPixel(hdc, x, y, colRef );
                image[pixel++] = imageLine[x] & 0x000000FF;
			}
		}
        internals()->copyFromBuffer(image);
    }

   	_DELETE_ARRAY( image );
   	_DELETE_ARRAY( imageLine );
}

void RenSurface::write( PerOstream& outStream )
{
	size_t w = width();
	size_t h = height();

	PER_WRITE_RAW_OBJECT( outStream, w );
	PER_WRITE_RAW_OBJECT( outStream, h );

	size_t lineLength = w;

	char* image = _NEW_ARRAY( char, lineLength );

    {
		for ( size_t y = 0; y < h; ++y )
		{
   			for ( size_t x = 0; x < w; ++x )
			{
				RenColour col;
				getPixel( x, y, &col );

//				image[ x * 3 ]    	   = GetBValue(colRef);
//				image[ ( x * 3 ) + 1 ] = GetGValue(colRef);
//				image[ ( x * 3 ) + 2 ] = GetRValue(colRef);
                image[x] = (unsigned char) (col.a() * 0xFF);
			}

			PER_WRITE_RAW_DATA( outStream, image, w );
		}
    }

   	_DELETE_ARRAY( image );
}

/* End BLITABLE.CPP *************************************************/
