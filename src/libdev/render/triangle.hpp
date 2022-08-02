/*
 * T R I A N G L E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TRIANGLE_HPP
#define _RENDER_TRIANGLE_HPP

#include "base/base.hpp"
#include "render/render.hpp"

class RenMaterial;
class RenVertex;

class RenTriangle
{
public:
    ~RenTriangle();

	// Provide read-only access to the vertices.  If you wish to move a
	// vertex, use the RenMesh interface.
	const RenVertex& vertex(size_t index) const;		// index [0,2]
	
	RenMaterial& material();
	const RenMaterial& material() const;
	
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenTriangle& t );

private:
    RenTriangle();

    // Operations deliberately revoked
    RenTriangle( const RenTriangle& );
    RenTriangle& operator =( const RenTriangle& );
    bool operator ==( const RenTriangle& );
};


#endif

/* End TRIANGLE.HPP *************************************************/
