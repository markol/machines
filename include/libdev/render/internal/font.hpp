/*
 * F O N T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_FONT_HPP
#define _RENDER_FONT_HPP

#include "base/base.hpp"
#include "render/material.hpp"
#include "render/surface.hpp"
#include "render/render.hpp"
#include <string>

class SysPathName;
class RenTexture;
class RenIVertex;

// A simple bitmap font.
class RenIFont
{
public:
	// PRE(!t.isNull());
    RenIFont(const RenTexture& t, size_t h, size_t w, size_t vSpace, size_t hSpace);
    ~RenIFont();

	void drawText(int x, int y, const std::string & text, const RenColour&);
	void textDimensions(const std::string & text, Ren::Rect* dimensions) const;

	size_t height() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenIFont& t );

private:
	const size_t	height_, width_, vSpace_, hSpace_;
	RenMaterial		mat_;
	const std::string 	name_;

	void wrapper(RenIVertex* vtx, Ren::VertexIdx* idx);
	void renderCharPolygon(const char c, int x, int y, int w, int h, uint col);

    RenIFont( const RenIFont& );
    RenIFont& operator =( const RenIFont& );
    bool operator ==( const RenIFont& );
};

#endif

/* End FONT.HPP *****************************************************/
