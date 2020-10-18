/*
 * R E N S P I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"			//  NB: this must come first.

#include <math.h>
#include "mathex/vec2.hpp"
#include "ctl/algorith.hpp"
#include "render/spinpoly.hpp"
#include "render/render.hpp"
#include "render/camera.hpp"
#include "render/uvanim.hpp"
#include "render/scale.hpp"
#include "render/stats.hpp"
#include "render/internal/dlyspin.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/illumine.hpp"
#include "render/internal/devicei.hpp"

#include "render/device.hpp"
#include "render/internal/glmath.hpp"

PER_DEFINE_PERSISTENT( RenSpinTFPolygon );

// If we're going to draw any polys there must be at least 3 vertices.
size_t RenSpinTFPolygon::maxVertices_ = 3;

// static
/*const MexVec3& RenSpinTFPolygon::SpinAxis::direction()
{
	static const MexVec3 xAxis(1,0,0);
	return xAxis;
}
*/
static  MexVec3& yAxis()
{
	static MexVec3 y(0,1,0);
	return y;
}


RenSpinTFPolygon::SpinAxis::SpinAxis( )
: direction_( MexVec3(1,0,0 ) ),
  isPerpendicularToY_( true ) // Added
{
	TEST_INVARIANT;
}

RenSpinTFPolygon::SpinAxis::SpinAxis( const SpinAxis& newSpinAxis )
: base_( newSpinAxis.base_ ),
  direction_( newSpinAxis.direction_ ),
  isPerpendicularToY_( newSpinAxis.isPerpendicularToY_ )
{

	TEST_INVARIANT;
}
RenSpinTFPolygon::SpinAxis::SpinAxis( const MexPoint3d& b, const MexVec3& d )
: base_( b ),
  direction_( d ),
  isPerpendicularToY_( true )
{
	PRE(not d.isZeroVector() );
	TEST_INVARIANT;

	direction_.makeUnitVector();
	isPerpendicularToY_ = direction_.isPerpendicularTo( yAxis() );
	TEST_INVARIANT;
}

void RenSpinTFPolygon::SpinAxis::direction( const MexVec3& d )
{
	PRE( not d.isZeroVector() );
	TEST_INVARIANT;

	direction_=d;
	direction_.makeUnitVector();
	isPerpendicularToY_ = direction_.isPerpendicularTo( yAxis() );

	TEST_INVARIANT;
}

void RenSpinTFPolygon::SpinAxis::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
	INVARIANT( direction_.modulus() == 1 );
	INVARIANT( isPerpendicularToY_ == direction_.isPerpendicularTo( yAxis() ) );
}



RenSpinTFPolygon::RenSpinTFPolygon(NormalType n):
  vertices_( _NEW( RenIVertexData() ) ),
  uv_(),
  normalType_(n),
  verticesOrdered_( false )
{
    uv_.reserve(4);
	vertices_->reserve(4);

	// TBD: verify that these default UV co-ordinates are sensible.
	uv_.push_back(MexPoint2d(0,1));
	uv_.push_back(MexPoint2d(0,0));
	uv_.push_back(MexPoint2d(1,0));
	uv_.push_back(MexPoint2d(1,1));

	if (maxVertices_ < vertices_->size())
		maxVertices_ = vertices_->size();

	TEST_INVARIANT;
}

RenSpinTFPolygon::RenSpinTFPolygon(const RenSpinTFPolygon& copyMe ):
  vertices_( _NEW( RenIVertexData( *copyMe.vertices_ ) ) ),
  uv_( copyMe.uv_ ),
  material_( copyMe.material_ ),
  spinAxis_( copyMe.spinAxis_ ),
  normalType_(copyMe.normalType_ ),
  verticesOrdered_( copyMe.verticesOrdered_ )
{

	TEST_INVARIANT;
}

RenSpinTFPolygon::RenSpinTFPolygon
(
	const RenSpinTFPolygon::Vertices& corners,
    const Vertices& uv,
	const RenMaterial& material,
	const RenSpinTFPolygon::SpinAxis& spinAxis,
	NormalType normal
)
:   vertices_( _NEW( RenIVertexData() ) ),
	uv_( uv ),
	material_( material ),
	spinAxis_( spinAxis ),
	normalType_(normal ),
	verticesOrdered_( false )
{
	PRE(corners.size() == uv.size());

	Vertices orderedCorners, orderedUV;
	makeOrderCorrect( corners, &orderedCorners );
	copyVertices( orderedCorners );

	if (maxVertices_ < vertices_->size())
		maxVertices_ = vertices_->size();

	TEST_INVARIANT;
}

// virtual
RenSpinTFPolygon::~RenSpinTFPolygon()
{
	TEST_INVARIANT;
	_DELETE( vertices_ );
}

void RenSpinTFPolygon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
	INVARIANT(vertices_ );
}

void RenSpinTFPolygon::applyUVTransform
(
	const RenUVTransform* anim,
	const RenMaterial& mat
) const
{
	TEST_INVARIANT;

	RenIVertexData* nonConstVertices_ = _CONST_CAST( RenIVertexData*, vertices_ );

	if (anim && anim->appliesTo(mat.texture()))
	{
		for (size_t i = 0; i < vertices_->size(); ++i)
			anim->transform( (*vertices_)[i].tu, (*vertices_)[i].tv, &((*nonConstVertices_)[i].tu), &((*nonConstVertices_)[i].tv) );
	}

	TEST_INVARIANT;
}

static void resizeVector(ctl_vector<MexPoint3d>& v, size_t newSize)
{
	PRE(newSize > v.size());

	v.reserve(newSize);
	const size_t extra = newSize - v.size();

	for (int i=extra; i!=0; --i)
		v.push_back(MexPoint3d());

	POST(v.size() == newSize);

}

inline void setWorldMatrix
(
	const MexTransform3d& world,
	glm::mat4& World,
	const RenScale& scale
)
{
	MexToGLMatrix(world, World);
	scale.apply(&World);
}


void RenSpinTFPolygon::render
(
	const MexTransform3d& localToGlobal,
	const RenScale& scale,
	const RenUVTransform* anim
) const
{
	render(localToGlobal, material_, scale, anim);
}

//void RenSpinTFPolygon::updateXForm( const MexTransform3d& globalTransform, const MexTransform3d& cameraTransform, const RenScale& scale )
//{
//		// update xForm
//		MexPoint3d cameraPosition ( cameraTransform.position() );
//		// Get the STF  origin plan vectors
//		MexVec3 xBasis( spinAxis_.direction() );
//		globalTransform.transform( &xBasis );
//		ASSERT(fabs(1 - xBasis.modulus()) <= MexEpsilon::instance(), "Spin poly axis is not unit vector");
//	    //If this line is not present, the transform constructor
//	    //can fail a pre-condition that X x Y == Z, due to slight rounding errors
//	    ASSERT_DATA( xBasis.makeUnitVector() );
//		MexPoint3d origin( spinAxis_.base() );
//		scale.apply(&origin);
//		globalTransform.transform( &origin );
//		// find the other polygon plan vector (yBasis)
//		MexVec3 yBasis;
//		yBasis.x( cameraPosition.x() - origin.x() );
//		yBasis.y( cameraPosition.y() - origin.y() );
//		yBasis.z( cameraPosition.z() - origin.z() );
//		yBasis.crossProduct( xBasis );
//		if(yBasis.isZeroVector() )
//		{
//			// the spin axis is in the camera direction
//			// pick any vector orthogonal to x axis
//			yBasis.x( xBasis.y() );
//			yBasis.y( -xBasis.x() );
//			yBasis.z( 0 );
//			if( yBasis.isZeroVector() )
//			{
//				yBasis.x( xBasis.z() );
//				yBasis.y( 0 );
//				yBasis.z( -xBasis.x() );
//				ASSERT( not yBasis.isZeroVector(), "yBasis is not orthogonal to xAxis" );
//			}
//		}
//
//		ASSERT( yBasis.isPerpendicularTo(xBasis), "yBasis is not orthogonal to xAxis" );
//
//		// Compute the polygon transform
//		xForm_ = MexTransform3d( MexTransform3d::X_XY, xBasis, yBasis, origin );
//
//		globalTransformKey_ = globalTransform.key();
//		cameraTransformKey_ = cameraTransform.key();
//}

void RenSpinTFPolygon::render
(
	const MexTransform3d& localToGlobal,
	const RenMaterial& mat,
	const RenScale& scale,
	const RenUVTransform* anim
) const
{
	PRE( RenIDeviceImpl::currentPimpl() );
	PRE( verticesOrdered_ );

	TEST_INVARIANT;

	RenIDeviceImpl*    	devImpl = RenIDeviceImpl::currentPimpl();
	RenDevice*    	    dev = RenDevice::current();

	MexPoint3d cameraPosition ( devImpl->currentCamera()->transform().position() );
	// Get the STF  origin plan vectors
	MexVec3 xBasis( spinAxis_.direction() );
	localToGlobal.transform( &xBasis );
	ASSERT(fabs(1 - xBasis.modulus()) <= MexEpsilon::instance(), "Spin poly axis is not unit vector");
    //If this line is not present, the transform constructor
    //can fail a pre-condition that X x Y == Z, due to slight rounding errors
    ASSERT_DATA( xBasis.makeUnitVector() );
	MexPoint3d origin( spinAxis_.base() );
	scale.apply(&origin);
	localToGlobal.transform( &origin );
	MexVec3 yBasis;
	spinPlan( cameraPosition, origin, xBasis, &yBasis );
	MexTransform3d xForm( MexTransform3d::X_XY, xBasis, yBasis, origin );

	// Set a direct3d version of the matrix incorporating scale.
    glm::mat4 glWorld;
    setWorldMatrix(xForm, glWorld, scale);

	applyUVTransform( anim, mat );

	RenIIlluminator*  ill = devImpl->illuminator();
	ill->lightVertices(*vertices_, vertices_->size());

	if (mat.hasAlphaTransparency())
	{
		if (devImpl->isAlphaSortingEnabled() && mat.usesBilinear()) // ueseBilinear fixes gun barrels rendering issue
		{
			// Shove it into the post-sorter.
			RenI::LitVtxAPtr lit = ill->applyMaterialAndCopy(mat, *vertices_, vertices_->size());
			RenIDelayedSpinPolygon* spoly = _NEW(RenIDelayedSpinPolygon(lit, vertices_->size(), mat, glWorld));
			std::auto_ptr<RenIDepthSortedItem> item( spoly );
			devImpl->alphaSorter().addItem(item);
		}
		else
		{
			// There is no post-sorter, so enable alpha and render it now.
			const RenIVertex* lit = ill->applyMaterial(mat, *vertices_, vertices_->size());
			devImpl->setMaterialHandles(mat);
			devImpl->enableAlphaBlending();
            dev->setModelMatrix(glWorld);
            dev->renderPrimitive(lit, vertices_->size(), mat);
			devImpl->disableAlphaBlending();
		}
	}
	else
	{
		const RenIVertex* lit = ill->applyMaterial(mat, *vertices_, vertices_->size());
		devImpl->setMaterialHandles(mat);
		dev->setModelMatrix(glWorld);
		dev->renderPrimitive(lit, vertices_->size(), mat);
	}

	// TBD: maybe take this out of the final version for performance.
	RenStats* stats = devImpl->statistics();
	if (stats)
		stats->incrSTFCount(1);
	TEST_INVARIANT;
}

RenSpinTFPolygon& RenSpinTFPolygon::operator=(const RenSpinTFPolygon& copyMe)
{
	TEST_INVARIANT;
	*vertices_ = *copyMe.vertices_;
	uv_ = copyMe.uv_;
	material_ = copyMe.material_;
	spinAxis_ = copyMe.spinAxis_;

	return *this;
}

void RenSpinTFPolygon::spinPlan
(
	const MexPoint3d& cameraPosition,
	const MexPoint3d& origin,
	const MexVec3& xBasis,
	MexVec3* pYBasis
) const
{
	TEST_INVARIANT;

	PRE( pYBasis );

	MexVec3& yBasis = *pYBasis;

	yBasis.x( cameraPosition.x() - origin.x() );
	yBasis.y( cameraPosition.y() - origin.y() );
	yBasis.z( cameraPosition.z() - origin.z() );
	yBasis.crossProduct( xBasis );
	if(yBasis.isZeroVector() )
	{
		// the spin axis is in the camera direction
		// pick any vector orthogonal to x axis
		yBasis.x( xBasis.y() );
		yBasis.y( -xBasis.x() );
		yBasis.z( 0 );
		if( yBasis.isZeroVector() )
		{
			yBasis.x( xBasis.z() );
			yBasis.y( 0 );
			yBasis.z( -xBasis.x() );
			ASSERT( not yBasis.isZeroVector(), "yBasis is not orthogonal to xAxis" );
		}
	}

	ASSERT( yBasis.isPerpendicularTo(xBasis), "yBasis is not orthogonal to xAxis" );

	TEST_INVARIANT;
}

static MATHEX_SCALAR areaOfTriangle(const MexPoint2d& p1, const MexPoint2d& p2, const MexPoint2d& p3)
{
	//*1000 to prevent rounding the result to zero too easity
	return p1.x() * ( p3.y()-p2.y() ) +
	       p2.x() * ( p1.y()-p3.y() ) +
	       p3.x() * ( p2.y()-p1.y() );
}


void RenSpinTFPolygon::makeOrderCorrect( const Vertices& points, Vertices* pOrderedPoints )
{
	PRE( not verticesOrdered_ );
	PRE( pOrderedPoints );

 	int length = points.size();

	*pOrderedPoints = points;
	Vertices& orderedPoints = *pOrderedPoints;

	if( length == 4 )
	{
		MATHEX_SCALAR areaOfTriangle012 = areaOfTriangle(orderedPoints[0], orderedPoints[1], orderedPoints[2]);
		MATHEX_SCALAR areaOfTriangle123 = areaOfTriangle(orderedPoints[1], orderedPoints[2], orderedPoints[3]);
		MATHEX_SCALAR areaOfTriangle230 = areaOfTriangle(orderedPoints[2], orderedPoints[3], orderedPoints[0]);

 		ASSERT( areaOfTriangle123 != 0, "areaOfTriangle123 = 0");
		ASSERT( areaOfTriangle230 != 0, "areaOfTriangle230 = 0");
		ASSERT( areaOfTriangle012 != 0, "areaOfTriangle012 = 0");

		bool pair1 = (areaOfTriangle012 * areaOfTriangle123 > 0);
		bool pair2 = (areaOfTriangle012 * areaOfTriangle230 > 0);
		bool NotBowTie = pair1&&pair2;
		if( !NotBowTie )
		{

			if( (areaOfTriangle012 * areaOfTriangle123 < 0)	&& (areaOfTriangle230 * areaOfTriangle123 > 0) )
			{
				std::swap( orderedPoints[3], orderedPoints[2]);
				std::swap( uv_[3], uv_[2]);
			}

			else
			{
				std::swap( orderedPoints[0], orderedPoints[3]);
				std::swap( uv_[0], uv_[3]);
			}
		}
	}
	verticesOrdered_=true;
}

void RenSpinTFPolygon::copyVertices
(
	const Vertices& pts
)
{

	TEST_INVARIANT;

	PRE( verticesOrdered_ );

	// will be cleaned another time
	//	PRE(vertices_->size() > pts.size());
	size_t nPts = pts.size();

	typedef ctl_vector< MexPoint3d > Points3d;
	static Points3d pts3d;
		if (pts3d.size() < nPts)
		resizeVector(pts3d, nPts);

	Points3d::iterator pts3dIt = pts3d.begin();
	for( Vertices::const_iterator ptsIt = pts.begin(); ptsIt != pts.end(); ++ptsIt )
	{
		MexPoint3d &pt3d = (*pts3dIt);
		pt3d.x( (*ptsIt).x() );
		pt3d.y( (*ptsIt).y() );
		pt3d.z( 0 );
		++pts3dIt;
	}

	vertices_->eraseAll();
	switch (normalType_)
	{
		case USE_AXIS:
			vertices_->addVertices(pts3d, spinAxis_.direction(), uv_, nPts);
			break;
		case SPUN_POLYGON_NORMAL:
			vertices_->addVertices(pts3d, yAxis(), uv_, nPts);
			break;
	}

	TEST_INVARIANT;
}

const RenIVertexData* RenSpinTFPolygon::vertices() const
{
	TEST_INVARIANT;
	return vertices_;
}

void RenSpinTFPolygon::vertices(const Vertices& verts)
{
	// if the vertices have been ordered yet it means that uv_ may have
	// been altered.
	// thus reordering will cause uv to be inconsitent with the vertices
	// all that is not very pretty and should be cleaned someday
	PRE( not verticesOrdered_ );
	TEST_INVARIANT;

	Vertices orderedVertices;
	makeOrderCorrect( verts, &orderedVertices );
	copyVertices( orderedVertices );
	TEST_INVARIANT;
}

ostream& operator <<( ostream& o, const RenSpinTFPolygon& p )
{
	RenIVertexData::const_iterator i = p.vertices_->begin();
	RenSpinTFPolygon::Vertices::const_iterator j = p.uv_.begin();

	o << "SpinAxis_base:       " << p.spinAxis_.base() << std::endl;
	o << "SpinAxis_direction:  " << p.spinAxis_.direction() << std::endl;

	o << "Vertices: " << std::endl;
	for( ; i != p.vertices_->end(); ++i )
		o << "\t" << (*i) << std::endl;

	o << "UVs:" << std::endl;
	for( ; j != p.uv_.end(); ++j )
		o << "\t" << *j << std::endl;

	return o;
}

void perWrite( PerOstream& ostr, const RenSpinTFPolygon::SpinAxis& axis )
{
	ostr << axis.base_;
	ostr << axis.direction_;
	ostr << axis.isPerpendicularToY_;
}

void perRead( PerIstream& istr, RenSpinTFPolygon::SpinAxis& axis )
{
	istr >> axis.base_;
	istr >> axis.direction_;
	istr >> axis.isPerpendicularToY_;
}

void perWrite( PerOstream& ostr, const RenSpinTFPolygon& polygon )
{
    ostr << polygon.vertices_;
    ostr << polygon.uv_;
    ostr << polygon.material_;
    ostr << polygon.spinAxis_;
    ostr << polygon.verticesOrdered_;
}

void perRead( PerIstream& istr, RenSpinTFPolygon& polygon )
{
    istr >> polygon.vertices_;
    istr >> polygon.uv_;
    istr >> polygon.material_;
    istr >> polygon.spinAxis_;
    istr >> polygon.verticesOrdered_;
}

/* End RENSPIN.CPP **************************************************/
