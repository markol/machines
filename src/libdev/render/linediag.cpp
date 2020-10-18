/***********************************************************

  L I N E D I A G . C P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#include "render/internal/linediag.hpp"

#include "render/render.hpp"
#include "render/device.hpp"
#include "render/colour.hpp"

#include <string>
#include "ctl/vector.hpp"
#include "system/pathname.hpp"

#ifndef _INLINE
	#include "render/internal/linediag.ipp"
#endif

// STATIC //
RenILinesDiagnostic& RenILinesDiagnostic::instance( void )
{
	static RenILinesDiagnostic instance;

	return instance;
}

RenILinesDiagnostic::~RenILinesDiagnostic( void )
{
	TEST_INVARIANT;
}

void RenILinesDiagnostic::setTestType( TestType type )
{
    testTypeMethod_ = type;
}

void RenILinesDiagnostic::drawLines(RenSurface* pSurface) const
{
	PRE(pSurface);
	PRE(RenDevice::current());
	PRE(RenDevice::current()->rendering2D());

	RenSurface::Points proxy;
	proxy.reserve(2);

	// Clear the drawing area.
	pSurface->filledRectangle(Ren::Rect(0, 0, pSurface->width(), pSurface->height()), RenColour::black());

	proxy.push_back(horizontalVertices()[0]);
	proxy.push_back(horizontalVertices()[1]);
	pSurface->polyLine(proxy, lineColour_, 1);
	proxy.erase(proxy.begin(), proxy.end());

	proxy.push_back(horizontalVertices()[2]);
	proxy.push_back(horizontalVertices()[3]);
	pSurface->polyLine(proxy, lineColour_, 1);
	proxy.erase(proxy.begin(), proxy.end());


	proxy.push_back(verticalVertices()[0]);
	proxy.push_back(verticalVertices()[1]);
	pSurface->polyLine(proxy, lineColour_, 1);
	proxy.erase(proxy.begin(), proxy.end());

	proxy.push_back(verticalVertices()[2]);
	proxy.push_back(verticalVertices()[3]);
	pSurface->polyLine(proxy, lineColour_, 1);
	proxy.erase(proxy.begin(), proxy.end());

	TEST_INVARIANT;
}

// Checks each pixel described by a point in points to see if its' colour is colour.
bool RenILinesDiagnostic::pixelsMatchColour(const RenSurface& surface, const RenSurface::Points& points, RenColour colour)
{
	int index = 0;
	RenColour pixelColour;
	bool result = true;

	RENDER_STREAM(points);

	while(index < points.size())
	{
		int y = testTypeMethod_ == TEST1 ? points[index].y() : surface.height() - 1 - points[index].y();
        surface.getPixel(points[index].x(), y, &pixelColour);

		if(not(pixelColour == colour))
		{
			RENDER_STREAM("Pixel match failiure in line diagnosis. Non-fatal." << std::endl);

			result = false;
			break;
		}

		++index;
	}

	RENDER_STREAM(std::endl);

	return result;
}

void RenILinesDiagnostic::testLines(const RenSurface& surface)
{
	PRE(RenDevice::current());
	PRE(RenDevice::current()->idleRendering() or not RenDevice::current()->rendering());

	RENDER_STREAM("Testing the 2D line drawing." << std::endl);
	RENDER_INDENT(4);

	// Horizontal tests.
	bool isNormal = pixelsMatchColour(surface, horizontalStockVertices(NORMAL), lineColour_);
	bool isSorted = pixelsMatchColour(surface, horizontalStockVertices(SORTED), lineColour_);
	bool isOffsetSorted = pixelsMatchColour(surface, horizontalStockVertices(OFFSET_SORTED), lineColour_); //isOffsetSorted = true;
	bool isLarge = pixelsMatchColour(surface, horizontalStockVertices(LARGE), lineColour_);

	RENDER_STREAM("Testing horizontal lines." << std::endl);
	RENDER_INDENT(4);
	RENDER_STREAM("isNormal == " << isNormal << std::endl);
	RENDER_STREAM("isSorted == " << isSorted << std::endl);
	RENDER_STREAM("isOffsetSorted == " << isOffsetSorted << std::endl);
	RENDER_STREAM("isLarge == " << isLarge << std::endl);
	RENDER_INDENT(4);

	// When LARGE occurs it could be mistaken for one of the others
	// so test for it first.
	if(isLarge)
	{
		horizontalResult_ = LARGE;
		RENDER_STREAM("LARGE drawing method diagnosed." << std::endl);
	}
	else
	{
		if(isNormal)
		{
			horizontalResult_ = NORMAL;
			RENDER_STREAM("NORMAL drawing method diagnosed." << std::endl);
		}
		else if(isSorted)
		{
			horizontalResult_ = SORTED;
			RENDER_STREAM("SORTED drawing method diagnosed." << std::endl);
		}
		else if(isOffsetSorted)
		{
			horizontalResult_ = OFFSET_SORTED;
			RENDER_STREAM("OFFSET_SORTED drawing method diagnosed." << std::endl);
		}
	}

	RENDER_INDENT(-8);

	// Vertical tests.
	isNormal = pixelsMatchColour(surface, verticalStockVertices(NORMAL), lineColour_); //isNormal = true;
	isSorted = pixelsMatchColour(surface, verticalStockVertices(SORTED), lineColour_);
	isOffsetSorted = pixelsMatchColour(surface, verticalStockVertices(OFFSET_SORTED), lineColour_);
	isLarge = pixelsMatchColour(surface, verticalStockVertices(LARGE), lineColour_);

	RENDER_STREAM("Testing vertical lines." << std::endl);
	RENDER_INDENT(4);
	RENDER_STREAM("isNormal == " << isNormal << std::endl);
	RENDER_STREAM("isSorted == " << isSorted << std::endl);
	RENDER_STREAM("isOffsetSorted == " << isOffsetSorted << std::endl);
	RENDER_STREAM("isLarge == " << isLarge << std::endl);
	RENDER_INDENT(4);

	// When LARGE occurs it could be mistaken for one of the others
	// so test for it first.
	if(isLarge)
	{
		verticalResult_ = LARGE;
		RENDER_STREAM("LARGE drawing method diagnosed." << std::endl);
	}
	else
	{
		if(isNormal)
		{
			verticalResult_ = NORMAL;
			RENDER_STREAM("NORMAL drawing method diagnosed." << std::endl);
		}
		else if(isSorted)
		{
			verticalResult_ = SORTED;
			RENDER_STREAM("SORTED drawing method diagnosed." << std::endl);
		}
		else if(isOffsetSorted)
		{
			verticalResult_ = OFFSET_SORTED;
			RENDER_STREAM("OFFSET_SORTED drawing method diagnosed." << std::endl);
		}
	}

	RENDER_INDENT(-8);

	hasTestedLines_ = true;

	RENDER_INDENT(-4);

	POST(hasTestedLines());
}

void RenILinesDiagnostic::CLASS_INVARIANT
{
	INVARIANT(this);
}

// PRIVATE //
RenILinesDiagnostic::RenILinesDiagnostic( void )

	:lineColour_(RenColour::white()),
	hasTestedLines_(false),
	horizontalResult_(UNKNOWN),
	verticalResult_(UNKNOWN),
    testTypeMethod_(TEST1)
{
	TEST_INVARIANT;
}

// STATIC //
const RenSurface::Points& RenILinesDiagnostic::horizontalVertices( void )
{
	static RenSurface::Points vertices;
	static bool initialised = false;

	if(not initialised)
	{
		initialised = true;

		vertices.reserve(4);

		vertices.push_back(MexPoint2d(15, 5));
		vertices.push_back(MexPoint2d(20, 5));

		vertices.push_back(MexPoint2d(20, 10));
		vertices.push_back(MexPoint2d(15, 10));
	}

	return vertices;
}

// STATIC //
const RenSurface::Points& RenILinesDiagnostic::verticalVertices( void )
{
	static RenSurface::Points vertices;
	static bool initialised = false;

	if(not initialised)
	{
		initialised = true;

		vertices.reserve(4);

		vertices.push_back(MexPoint2d(5, 5));
		vertices.push_back(MexPoint2d(5, 10));

		vertices.push_back(MexPoint2d(10, 10));
		vertices.push_back(MexPoint2d(10, 5));
	}

	return vertices;
}


// STATIC //
const RenSurface::Points& RenILinesDiagnostic::horizontalStockVertices(DrawMethod method)
{
	PRE(method != UNKNOWN);

	static RenSurface::Points vertices;
	static bool initialised = false;

	if(not initialised)
	{
		initialised = true;

		vertices.reserve(4);
	}
	else
		vertices.erase(vertices.begin(), vertices.end());

	switch(method)
	{
	case NORMAL:
		vertices.push_back(MexPoint2d(15, 5));
		vertices.push_back(MexPoint2d(19, 5));

		vertices.push_back(MexPoint2d(16, 10));
		vertices.push_back(MexPoint2d(20, 10));
		break;

	case SORTED:
		vertices.push_back(MexPoint2d(15, 5));
		vertices.push_back(MexPoint2d(19, 5));

		vertices.push_back(MexPoint2d(15, 10));
		vertices.push_back(MexPoint2d(19, 10));
		break;

	case OFFSET_SORTED:
		vertices.push_back(MexPoint2d(15, 4));
		vertices.push_back(MexPoint2d(19, 4));

		vertices.push_back(MexPoint2d(15, 9));
		vertices.push_back(MexPoint2d(19, 9));
		break;

	case LARGE:
		vertices.push_back(MexPoint2d(15, 5));
		vertices.push_back(MexPoint2d(20, 5));

		vertices.push_back(MexPoint2d(15, 10));
		vertices.push_back(MexPoint2d(20, 10));
		break;

	default:
		ASSERT_FAIL("Invalid enum.");
		break;
	}

	return vertices;
}

// STATIC //
const RenSurface::Points& RenILinesDiagnostic::verticalStockVertices(DrawMethod method)
{
	PRE(method != UNKNOWN);

	static RenSurface::Points vertices;
	static bool initialised = false;

	if(not initialised)
	{
		initialised = true;

		vertices.reserve(4);
	}
	else
		vertices.erase(vertices.begin(), vertices.end());

	switch(method)
	{
	case NORMAL:
		vertices.push_back(MexPoint2d(5, 5));
		vertices.push_back(MexPoint2d(5, 9));

		vertices.push_back(MexPoint2d(10, 6));
		vertices.push_back(MexPoint2d(10, 10));
		break;

	case SORTED:
		vertices.push_back(MexPoint2d(5, 5));
		vertices.push_back(MexPoint2d(5, 9));

		vertices.push_back(MexPoint2d(10, 5));
		vertices.push_back(MexPoint2d(10, 9));
		break;

	case OFFSET_SORTED:
		vertices.push_back(MexPoint2d(4, 5));
		vertices.push_back(MexPoint2d(4, 9));

		vertices.push_back(MexPoint2d(9, 5));
		vertices.push_back(MexPoint2d(9, 9));
		break;

	case LARGE:
		vertices.push_back(MexPoint2d(5, 5));
		vertices.push_back(MexPoint2d(5, 10));

		vertices.push_back(MexPoint2d(10, 5));
		vertices.push_back(MexPoint2d(10, 10));
		break;

	default:
		ASSERT_FAIL("Invalid enum.");
		break;
	}

	return vertices;
}

/* LINEDIAG.CPP *******************************************/


