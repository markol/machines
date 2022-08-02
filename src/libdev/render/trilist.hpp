/*
 * T R I L I S T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TRILIST_HPP
#define _RENDER_TRILIST_HPP

#include "base/base.hpp"
#include "render/render.hpp"

class RenTriangle;
class RenMesh;
class RenVertex;
class RenMaterial;

class RenTriangleList
{
public:
    virtual ~RenTriangleList();

	// Allow triangles to be added or removed from this list.
	RenTriangle& addTriangle(const RenVertex&, const RenVertex&, const RenVertex&, const RenMaterial&);
	void removeTriangle(const RenTriangle&);
	
	// Allow access to individual triangles.
	Ren::TriangleIdx nTriangles() const;
	RenTriangle& triangle(Ren::TriangleIdx index);
	const RenTriangle& triangle(Ren::TriangleIdx index) const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const RenTriangleList& t );

private:
    RenTriangleList();

    // Operations deliberately revoked
    RenTriangleList( const RenTriangleList& );
    RenTriangleList& operator =( const RenTriangleList& );
    bool operator ==( const RenTriangleList& );
};

#endif

/* End TRILIST.HPP **************************************************/
