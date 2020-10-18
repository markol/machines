/*
 * S P R I T E 3 D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/sprite3d.hpp"

#include "render/mesh.hpp"
#include "render/ttfpoly.hpp"
#include "render/meshinst.hpp"

#include "mathex/double.hpp"
#include "mathex/vec2.hpp"

PER_DEFINE_PERSISTENT( W4dSprite3d );

W4dSprite3d::W4dSprite3d(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    const RenMaterial& mat )
: W4dEntity( pParent, localTransform, NOT_SOLID )
{
	// Because of the way TTFs are "lit" by the render library, they are
	// unaffected by local or dynamic lights.  Setting this flag for all
	// sprites means we can avoid the world4d light culling overhead, but
	// TTFs should still be rendered with the same lighting regardless.
	// This could easily break if TTFs were lit differently by the render lib.
	doNotLight(true);

    pMesh_ = RenMesh::createEmpty();
    pMeshInstance_ = _NEW( RenMeshInstance( pMesh_ ) );

    //  Set up a turn to face polygon in the mesh
    RenTTFRectangle   polygon;
    polygon.width( width );
    polygon.height( height );
    polygon.material( mat );

    pMesh_->addTTFPolygon( polygon );
    add( pMeshInstance_ );

    TEST_INVARIANT;
}

W4dSprite3d::W4dSprite3d(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    const RenMaterial& mat,
    const MexVec2& uv0, const MexVec2& uv1, const MexVec2& uv2, const MexVec2& uv3 )
: W4dEntity( pParent, localTransform, NOT_SOLID )
{
	// See comment in other ctor.
	doNotLight(true);

    pMesh_ = RenMesh::createEmpty();
    pMeshInstance_ = _NEW( RenMeshInstance( pMesh_ ) );

    //  Set up a turn to face polygon in the mesh
    RenTTFRectangle   polygon;
    polygon.width( width );
    polygon.height( height );
    polygon.material( mat );
    polygon.uv(uv0, 0);
    polygon.uv(uv1, 1);
    polygon.uv(uv2, 2);
    polygon.uv(uv3, 3);

    pMesh_->addTTFPolygon( polygon );
    add( pMeshInstance_ );

    TEST_INVARIANT;
}

W4dSprite3d::~W4dSprite3d()
{
    TEST_INVARIANT;

    //  The mesh instance will be deleted in the entity's destructor
//    _DELETE( pMeshInstance_ );
}

void W4dSprite3d::depthOffset( MATHEX_SCALAR offset )
{
    for( Ren::TriangleIdx i = 0; i < pMesh_->nTTFPolygons(); ++i )
    {
        RenTTFPolygon& ttfPolygon = pMesh_->TTFPolygon( i );
        ttfPolygon.depthOffset( offset );
    }
}

void W4dSprite3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool W4dSprite3d::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void W4dSprite3d::rotate( const MexRadians& angle )
{
    static const MATHEX_SCALAR s2 = sqrt( 2.0 ) / 2.0;
	const MexRadians a2 = MexDegrees(45) - angle;
    const MATHEX_SCALAR c = s2 * cos(a2);
    const MATHEX_SCALAR s = s2 * sin(a2);

	const MexVec2 uv0(0.5 - c, 0.5 - s);
	const MexVec2 uv1(0.5 + s, 0.5 - c);
	const MexVec2 uv2(0.5 + c, 0.5 + s);
	const MexVec2 uv3(0.5 - s, 0.5 + c);

    for( Ren::TriangleIdx i = 0; i < pMesh_->nTTFPolygons(); ++i )
    {
        RenTTFPolygon& ttfPolygon = pMesh_->TTFPolygon(i);

		// This is a real hack for backwards compatiblity with the old system
		// wherein RenTTFPolygon was the concrete class.  TBD: ideally this would
		// be rewritten so that we rotate the 2D vertices of the TTF, rather than
		// the texture co-ordinates (the result would be visually better).
		if (ttfPolygon.isRectangle())
		{
			ASSERT(ttfPolygon.nVertices() == 4, "TTF rectangle doesn't have 4 vertices.");
			RenTTFRectangle& rect = ttfPolygon.asRectangle();

	        rect.uv(uv0, 0);
	        rect.uv(uv1, 1);
	        rect.uv(uv2, 2);
	        rect.uv(uv3, 3);
		}
    }
}

void W4dSprite3d::visibleRange( MATHEX_SCALAR distance )
{
    setDistance( distance, 0 );
}

void perWrite( PerOstream& ostr, const W4dSprite3d& sprite )
{
    const W4dEntity& base = sprite;

    ostr << base;
}

void perRead( PerIstream& istr, W4dSprite3d& sprite )
{
    W4dEntity& base = sprite;

    istr >> base;
}

W4dSprite3d::W4dSprite3d( PerConstructor con )
: W4dEntity( con )
{
}
/* End SPRITE3D.CPP ***************************************************/
