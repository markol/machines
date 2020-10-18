/*
 * T T F T R I S . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#ifndef _RENDER_TTFTRIS_HPP
#define _RENDER_TTFTRIS_HPP

#include "render/ttfpoly.hpp"

//////////////////////////////////////////////////////////////////////////
// A turn-to-face whose 2D shape is four triangles where the corners of a
// rectangle would be.  Something like this:
//
//      0   1     5   3
//		+====-----====+
//		|**/       \**| <--opaque
//		|*/			\*|
//	   2|/			 \|4
//		|             |
//		| transparent |
//		|             |
//	  10|\			 /|8
//		|*\			/*|
//		|**\       /**|
//		+====-----====+
//		9   11    7   6
//
// It has a very specialized use: 2D markers.
// The verices are numbered as shown above.  Clockwise with each triangle
// starting at the corner of the rectangle.  The first tri is top-left.
class RenTTFTriangles : public RenTTFPolygon
{
public:
    RenTTFTriangles();
    RenTTFTriangles(const RenTTFTriangles&);
    virtual ~RenTTFTriangles();
	virtual RenTTFPolygon* clone() const;

	// The width and height of the entire entity.
	MATHEX_SCALAR width () const;				// default = 1
	MATHEX_SCALAR height() const;				// default = 1
	void width (MATHEX_SCALAR w);				// PRE(w > 0);
	void height(MATHEX_SCALAR h);				// PRE(h > 0);

	// The width and height of the corners (it's symmetrical).
	MATHEX_SCALAR cornerWidth () const;				// default = 0.4
	MATHEX_SCALAR cornerHeight() const;				// default = 0.4
	void cornerWidth (MATHEX_SCALAR w);				// PRE(w > 0); PRE(w <= 0.5*width());
	void cornerHeight(MATHEX_SCALAR h);				// PRE(h > 0); PRE(h <= 0.5*height());

	virtual MATHEX_SCALAR radius() const;

    void CLASS_INVARIANT;

	virtual RenTTFRectangle& asRectangle();				// PRE(isRectangle());
	virtual const RenTTFRectangle& asRectangle() const;	// PRE(isRectangle());

    PER_MEMBER_PERSISTENT_VIRTUAL( RenTTFTriangles );
    PER_FRIEND_READ_WRITE( RenTTFTriangles );

private:
    virtual GLenum compose2DGeometry(RenIVertex*, const MexPoint3d& centre, MATHEX_SCALAR scale) const;
	static const Ren::VertexIdx* indices();

	MATHEX_SCALAR width_, height_, cornerWidth_, cornerHeight_;

    friend ostream& operator <<( ostream&, const RenTTFTriangles& );
    virtual void    write( ostream& ) const;

	// Revoked.
	RenTTFTriangles& operator=(const RenTTFTriangles&);
};

PER_DECLARE_PERSISTENT( RenTTFTriangles );

#endif

/* End TTFTRIS.HPP **************************************************/
