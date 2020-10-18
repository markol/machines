/*
 * T T F T R I S . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/ttftris.hpp"
#include "render/material.hpp"
#include "mathex/vec2.hpp"
#include "render/internal/vtxdata.hpp"

PER_DEFINE_PERSISTENT( RenTTFTriangles );

RenTTFTriangles::RenTTFTriangles():
	RenTTFPolygon(12, indices(), 12),
	width_(1),
	height_(1),
	cornerWidth_(0.4),
	cornerHeight_(0.4)
{
	PRE(Ren::initialised());

	// TBD: verify that these default UV co-ordinates are sensible.
	uv(MexVec2(0,1), 0);
	uv(MexVec2(1,1), 1);
	uv(MexVec2(0,0), 2);

	uv(MexVec2(0,1), 3);
	uv(MexVec2(1,1), 5);
	uv(MexVec2(0,0), 4);

	uv(MexVec2(0,1), 6);
	uv(MexVec2(1,1), 7);
	uv(MexVec2(0,0), 8);

	uv(MexVec2(0,1), 9);
	uv(MexVec2(1,1), 11);
	uv(MexVec2(0,0), 10);
}

RenTTFTriangles::RenTTFTriangles(const RenTTFTriangles& rhs):
	RenTTFPolygon(rhs),
	width_(rhs.width_),
	height_(rhs.height_),
	cornerWidth_(rhs.cornerWidth_),
	cornerHeight_(rhs.cornerHeight_)
{
}

// virtual
RenTTFTriangles::~RenTTFTriangles()
{
}

// virtual
RenTTFPolygon* RenTTFTriangles::clone() const
{
	return _NEW(RenTTFTriangles(*this));
}

// virtual
GLenum RenTTFTriangles::compose2DGeometry
(
    RenIVertex* vtx,
	const MexPoint3d& centre,
	MATHEX_SCALAR scale
) const
{
	const MATHEX_SCALAR cw = cornerWidth_  * scale;
	const MATHEX_SCALAR ch = cornerHeight_ * scale;

	scale *= 0.5;
	const MATHEX_SCALAR wDiv2 = width_  * scale;
	const MATHEX_SCALAR hDiv2 = height_ * scale;

	const MATHEX_SCALAR x0 = centre.x() - wDiv2;
	const MATHEX_SCALAR x1 = x0 + cw;
	const MATHEX_SCALAR x3 = centre.x() + wDiv2;
	const MATHEX_SCALAR x2 = x3 - cw;

	const MATHEX_SCALAR y0 = centre.y() + hDiv2;
	const MATHEX_SCALAR y1 = y0 - ch;
	const MATHEX_SCALAR y3 = centre.y() - hDiv2;
	const MATHEX_SCALAR y2 = y3 + ch;

	vtx[0].x = vtx[2].x = vtx[10].x = vtx[9].x = (x0);
	vtx[3].x = vtx[4].x = vtx[ 8].x = vtx[6].x = (x3);

 	vtx[0].y = vtx[ 1].y = vtx[5].y = vtx[3].y = (y0);
 	vtx[9].y = vtx[11].y = vtx[7].y = vtx[6].y = (y3);

 	vtx[1].x = vtx[11].x = (x1);
 	vtx[5].x = vtx[ 7].x = (x2);

 	vtx[ 2].y = vtx[4].y = (y1);
 	vtx[10].y = vtx[8].y = (y2);

	return GL_TRIANGLES;
}

// static
const Ren::VertexIdx* RenTTFTriangles::indices()
{
	static const Ren::VertexIdx idx[12] = { 0,1,2, 3,4,5, 6,7,8, 9,10,11 };
	return idx;
}

double RenTTFTriangles::width() const
{
	return width_;
}

double RenTTFTriangles::height() const
{
	return height_;
}

void RenTTFTriangles::width(double w)
{
	PRE(w > 0);
	width_ = w;
}

void RenTTFTriangles::height(double h)
{
	PRE(h > 0);
	height_ = h;
}

double RenTTFTriangles::cornerWidth() const
{
	return cornerWidth_;
}

double RenTTFTriangles::cornerHeight() const
{
	return cornerHeight_;
}

void RenTTFTriangles::cornerWidth(double w)
{
	PRE(w > 0);
	PRE(w <= 0.5*width());
	cornerWidth_ = w;
}

void RenTTFTriangles::cornerHeight(double h)
{
	PRE(h > 0);
	PRE(h <= 0.5*height());
	cornerHeight_ = h;
}

// virtual
MATHEX_SCALAR RenTTFTriangles::radius() const
{
	return 0.5 * std::max(width_, height_);
}

// virtual
RenTTFRectangle& RenTTFTriangles::asRectangle()
{
	ASSERT_FAIL("This isn't a TTF rectangle.");
	RenTTFRectangle* ptr = 0;
	return *ptr;
}

// virtual
const RenTTFRectangle& RenTTFTriangles::asRectangle() const
{
	ASSERT_FAIL("This isn't a TTF rectangle.");
	RenTTFRectangle* ptr = 0;
	return *ptr;
}

// Note this is a bit non-standard.  It doesn't call a PerConstructor for
// the base class because we *must* set-up the indices in the base class.
// The indices cannot be persisted because they are statically allocated.
RenTTFTriangles::RenTTFTriangles(PerConstructor):
	RenTTFPolygon(12, indices(), 12)
{
}

void perWrite( PerOstream& ostr, const RenTTFTriangles& rect )
{
    const RenTTFPolygon& base = rect;

    ostr << base;
    ostr << rect.width_;
    ostr << rect.height_;
    ostr << rect.cornerWidth_;
    ostr << rect.cornerHeight_;
}

void perRead( PerIstream& istr, RenTTFTriangles& rect )
{
    RenTTFPolygon& base = rect;

    istr >> base;
    istr >> rect.width_;
    istr >> rect.height_;
    istr >> rect.cornerWidth_;
    istr >> rect.cornerHeight_;
}

// virtual
void RenTTFTriangles::write( ostream& ostr ) const
{
    ostr << "TTF corner triangles: ";
    ostr << centre() << " w=" << width() << " h=" << height();
    ostr << " depth off=" << depthOffset() << " " << material();
}

/* End TTFTRIS.CPP **************************************************/
