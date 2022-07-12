/***********************************************************

  L I N E D I A G . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifndef _RENDER_INTERNAL_LINE_DIAGNOSTIC_HPP
#define _RENDER_INTERNAL_LINE_DIAGNOSTIC_HPP

/*
  Line drawing can be implemented in different ways depending on the
  graphics card and it's drivers. This class draws several lines to a
  surface and then tests them.

  The interface splits up the drawing and testing, This is due to the
  need to draw on a surface without testing it.

  The client is forced to ensure that the surface passed to to
  testLines() has been already passed to drawLines().
*/

#include "render/surface.hpp"
#include "render/colour.hpp"

// Pattern : Singleton.
class RenILinesDiagnostic
// Cannonical form revoked.
{
public:
	enum DrawMethod
	{
		// Draws the line as the GDI would, misses out the last pixel.
		NORMAL,
		// Draws as NORMAL would except for vertex dominance, the same
		// pixel is missed out no matter how you order the vertices.
		SORTED,
		// Draws as a sorted line but is moved by an offset.
		OFFSET_SORTED,
		// The lines are drawn with the last pixel.
		LARGE,
		// The tests look for known errors in line drawing. If the tests
		// are inconclusive the results are set to this.
		UNKNOWN
	};

    enum TestType
    {
        TEST1, TEST2
    };

	static RenILinesDiagnostic& instance();

	~RenILinesDiagnostic();

	// This is called to draw the test lines onto a surface. Allocates some line vertices.
	void drawLines(RenSurface* pSurface) const;
	// PRE(pSurface);
	// PRE(RenDevice::current());
	// PRE(RenDevice::current()->rendering2D());

	// Tests a surface for line drawing method. As this is normally called as soon after
	// drawLines() as possible the idleRendering() precondition must be upheld to ensure
	// that any batched drawing operatons are sent.
	void testLines(const RenSurface& surface);
	// PRE(RenDevice::current());
	//	PRE(RenDevice::current()->idleRendering() or not RenDevice::current()->rendering());
	// POST(hasTestedLines());

	DrawMethod horizontalResult() const;
	// PRE(hasTestedLines());
	DrawMethod verticalResult() const;
	// PRE(hasTestedLines());

	bool hasTestedLines() const;
    void setTestType( TestType type );
	
	void CLASS_INVARIANT;

private:
	RenILinesDiagnostic();

	// Operations deliberatly revoked.
	RenILinesDiagnostic(const RenILinesDiagnostic& copyMe);
	RenILinesDiagnostic& operator =(const RenILinesDiagnostic& assignMe);

	// These are the vertices actual used in the drawing.
	static const RenSurface::Points& horizontalVertices();
	static const RenSurface::Points& verticalVertices();

	// These are vertices gained from tests on machines with and without
	// drawing errors. They are tested against the actual results to determine
	// whether the drawing method is a known and fixable one.
	static const RenSurface::Points& horizontalStockVertices(DrawMethod method);
	static const RenSurface::Points& verticalStockVertices(DrawMethod method);

    bool pixelsMatchColour(const RenSurface& surface, const RenSurface::Points& points, RenColour colour);
	
	RenColour lineColour_;

	bool hasTestedLines_;

	DrawMethod horizontalResult_;
	DrawMethod verticalResult_;
    TestType   testTypeMethod_;
};

#ifdef _INLINE
	#include "internal/linediag.ipp"
#endif

#endif /* _RENDER_INTERNAL_LINE_DIAGNOSTIC_HPP ************/

