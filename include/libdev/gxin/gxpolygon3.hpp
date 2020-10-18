/*
 * G X P O L Y G O N 3 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXPolygon3

    A brief description of the class should go in here
*/

#ifndef _GXPOLYGON3_HPP
#define _GXPOLYGON3_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "gxin/gx.hpp"
#include "gxin/gxpolyvert3.hpp"
#include "gxin/gxmat.hpp"
#include "gxin/gxidpos.hpp"


class GXPolygon3
// Canonical form
{
public:
    GXPolygon3( void );
    ~GXPolygon3( void );
    GXPolygon3( const GXPolygon3& );
    GXPolygon3& operator =( const GXPolygon3& );
    friend bool operator ==( const GXPolygon3&, const GXPolygon3& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXPolygon3& t );

    // Get the size of the vertices array
    int numVertices( void ) const;
	// Resize the vertices array
	void numVertices(int newNumVertices);
    // Get vertex i
    const GXPolyVert3& vertex(int i) const { return vertices_[i]; }
	// Add a vertex to the array
	void addVertex(const GXPolyVert3& newVertex);

	UCHAR flag( void ) const { return flag_; }
	void flag( UCHAR newFlag) { flag_=newFlag; }

    // true if textureId_ refers to a valid texture name in the mesh
    bool hasValidTexture() { return mat_.hasValidTexture(); }
    void hasValidTexture(bool valid) { mat_.hasValidTexture(valid); }
	short transparancy( void ) const { return mat().transparancy();	}
	void transparancy(short newTransparancy) { mat_.transparancy(newTransparancy); }
	const GXMat& mat() const { return mat_;	}
	void mat(const GXMat& newMat) { 
	  mat_=newMat;
	  mat_.hasOneColor(noVertexColored());
	}

	const GXColor& diffuseColor() const;
	  // PRE(hasOneColor());

    // cannot set directly the polygon's color, get it from 
    // the vertices	if possible
    void setColorStatus();

    // true if all vertices don't have a color (color hold by the material)
    bool hasOneColor() const { return noVertexColored(); }

private:
	// private method ensuring that the class remains in a consistent status
	// -> TEST_INVARIANT
	bool allVerticesColored() const;
	bool noVertexColored() const;													

	ctl_vector<GXPolyVert3> vertices_;
	UCHAR flag_;
	USHORT flag2_;

	// color_ does not exist in file format, 
	// but get it from vertices_ if they have same color
	GXMat mat_; 

};


#endif

/* End GXPOLYGON3.HPP ***********************************************/
