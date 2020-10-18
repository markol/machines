/*
 * F O N T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/font.hpp"

#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/internal/devicei.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/colpack.hpp"
#include "stdlib/string.hpp"

#include <sstream>
#include <ctype.h>

// IS this in use?
RenIFont::RenIFont
(
	const RenTexture& tex, size_t h, size_t w,
	size_t vSpace, size_t hSpace
):
	height_(h),
	width_(w),
	vSpace_(vSpace),
	hSpace_(hSpace),
	name_(tex.name())
{
	PRE(!tex.isNull());
	mat_.texture(tex);
    TEST_INVARIANT;
}

RenIFont::~RenIFont()
{
    TEST_INVARIANT;
}

void RenIFont::wrapper(RenIVertex* vtx, Ren::VertexIdx* idx)
{

}

void RenIFont::renderCharPolygon(const char c, int x, int y, int w, int h, uint col)
{
    RenIVertex vtx[4];
	vtx[0].x = (x);
	vtx[1].x = (x+w);
	vtx[2].x = vtx[1].x;
	vtx[3].x = vtx[0].x;

	vtx[0].y = (y);
	vtx[1].y = vtx[0].y;
	vtx[2].y = (y+h);
	vtx[3].y = vtx[2].y;

	vtx[0].z = (0.5);
	vtx[1].z = (0.5);
	vtx[2].z = (0.5);
	vtx[3].z = (0.5);

	vtx[0].w = (0.5);
	vtx[1].w = (0.5);
	vtx[2].w = (0.5);
	vtx[3].w = (0.5);

	vtx[0].color = vtx[1].color = vtx[2].color = vtx[3].color = col;

	// The first 32 ASCII chars are non-printable and space isn't printed
	// so the first printable starts at code 33.
	const int charIndex = c - 33;

	// For now this is hardcoded.  It should depend upon the contents of the
	// texture map being used.
	const int charsPerLine = 12;
	const int gridX = charIndex % charsPerLine;
	const int gridY = charIndex / charsPerLine;

	const float uPerChar =  9 / 128.0;
	const float vPerChar = 14 / 128.0;

	// Set up texture co-ordinates which pick out the desired character.
	vtx[0].tu = (gridX * uPerChar);
	vtx[1].tu = vtx[0].tu + (uPerChar);
	vtx[2].tu = vtx[1].tu;
	vtx[3].tu = vtx[0].tu;

	vtx[0].tv = (gridY * vPerChar);
	vtx[1].tv = vtx[0].tv;
	vtx[2].tv = vtx[0].tv + (vPerChar);
	vtx[3].tv = vtx[2].tv;

	static Ren::VertexIdx indices[4] = { 0,1,2,3 };
	wrapper(vtx, indices);

}

void RenIFont::drawText(int x, int y, const std::string& text, const RenColour& col)
{
	const int startX = x;
	Ren::Rect  srcArea(0,0, 1,1);

	RenIDeviceImpl::currentPimpl()->setMaterialHandles(mat_);
	const uint d3dCol = packColourChecked(col.r(), col.g(), col.b());

	for (int i=0; i!=text.length(); ++i)
	{
		const char asChar = text[i];

		// Adjust the coords if this is a whitespace char.
		if (asChar == '\n')
		{
			x = startX;
			y = y + height_ + vSpace_;
		}
		else if (asChar == ' ')
		{
			x += (width_ + vSpace_);
		}
		else if (asChar == '\t')
		{
			x += 8*width_;
		}
		else
		{
			renderCharPolygon(asChar, x,y, width_, height_, d3dCol);
			x += (width_ + vSpace_);
		}
	}
}

void RenIFont::textDimensions(const std::string&, Ren::Rect*) const
{
}

size_t RenIFont::height() const
{
	return height_;
}

void RenIFont::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenIFont& t )
{
    o << "RenIFont " << (void*)&t << " start" << std::endl;
    o << "RenIFont " << (void*)&t << " end" << std::endl;

    return o;
}

/* End FONT.CPP *****************************************************/
