/*
 * S P I N . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#include "world4d/spin.hpp"

#include "render/mesh.hpp"
#include "render/spinpoly.hpp"
#include "render/meshinst.hpp"

#include "mathex/double.hpp"
#include "mathex/vec2.hpp"

PER_DEFINE_PERSISTENT( W4dSpinSprite );

W4dSpinSprite::W4dSpinSprite(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    const RenMaterial& mat,
    const MexVec3& spinAxis )
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

    MATHEX_SCALAR halfWidth = width/2.0;
	MATHEX_SCALAR halfHeight = height/2.0;
	ctl_vector<MexPoint2d> verts, uv;
	verts.reserve(4);

	verts.push_back( MexPoint2d( height, -halfWidth ) );
	verts.push_back( MexPoint2d( height, halfWidth ) );
	verts.push_back( MexPoint2d( 0, halfWidth ) );
	verts.push_back( MexPoint2d( 0, -halfWidth ) );

	uv.reserve(4);
	uv.push_back(MexPoint2d(0,0));
	uv.push_back(MexPoint2d(1,0));
	uv.push_back(MexPoint2d(1,1));
	uv.push_back(MexPoint2d(0,1));

    RenSpinTFPolygon   polygon( verts,
                                uv ,
                                mat,
                                RenSpinTFPolygon::SpinAxis(MexPoint3d(0,0,0), spinAxis)
                              );

    pMesh_->addSpinTFPolygon( polygon );
    add( pMeshInstance_ );

    TEST_INVARIANT;
}


W4dSpinSprite::~W4dSpinSprite()
{
    TEST_INVARIANT;

    //  The mesh instance will be deleted in the entity's destructor
//    _DELETE( pMeshInstance_ );
}

void W4dSpinSprite::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool W4dSpinSprite::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void W4dSpinSprite::visibleRange( MATHEX_SCALAR distance )
{
    setDistance( distance, 0 );
}

void perWrite( PerOstream& ostr, const W4dSpinSprite& sprite )
{
    const W4dEntity& base = sprite;

    ostr << base;
}

void perRead( PerIstream& istr, W4dSpinSprite& sprite )
{
    W4dEntity& base = sprite;

    istr >> base;
}

W4dSpinSprite::W4dSpinSprite( PerConstructor con )
: W4dEntity( con )
{
}
/* End SPRITE3D.CPP ***************************************************/
