/*
 * T T F P O L Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/ttfpoly.hpp"
#include "render/colour.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"
#include "render/uvanim.hpp"
#include "render/stats.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/vpmap.hpp"
#include "render/internal/polysord.hpp"
#include "render/internal/tlpoly.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/illumine.hpp"
#include "render/internal/ttfpolyi.hpp"
#include "render/internal/devicei.hpp"
#include "render/device.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( RenTTFPolygon );
PER_DEFINE_PERSISTENT( RenTTFRectangle );

//--------------------------------Public base class-----------------------------
RenTTFPolygon::RenTTFPolygon(size_t nVertices, const Ren::VertexIdx* idx, size_t nIndices):
	pImpl_(_NEW(RenITTFImpl(nVertices, idx, nIndices)))
{
	PRE(Ren::initialised());
	depthOffset(0);
}

RenTTFPolygon::RenTTFPolygon(const RenTTFPolygon& rhs):
	pImpl_(_NEW(RenITTFImpl(*rhs.pImpl_)))
{
	centre(rhs.centre());
	depthOffset(rhs.depthOffset());
}

// virtual
RenTTFPolygon::~RenTTFPolygon()
{
	// This should ensure that no TTFs get destroyed during rendering.
	// Thus the post-sorted polygons can refernce this polygon's attributes
	// without need for reference counting or copying the data.
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	_DELETE(pImpl_);
}

const RenMaterial& RenTTFPolygon::material() const
{
	RENDER_STREAM("Retrieved TTF " << pImpl_->material() << "\n");
	return pImpl_->material();
}

void RenTTFPolygon::material(const RenMaterial& m)
{
	RENDER_STREAM("Set new TTF " << m << "\n");
	pImpl_->material(m);
}

//#define TTF_STREAM(x)	;
//#define TTF_DATA(x)		;
#define TTF_STREAM(x)		RENDER_STREAM(x)
#define TTF_DATA(x)			x

void RenTTFPolygon::render
(
	const glm::mat4& xform,
	MATHEX_SCALAR scale,
	const RenUVTransform* anim
) const
{
	render(xform, pImpl_->material(), scale, anim);
}

void RenTTFPolygon::render
(
	const glm::mat4& xform,
	const RenMaterial& mat,
	MATHEX_SCALAR scale,
	const RenUVTransform* anim
) const
{
	PRE(pImpl_);
	PRE( RenIDeviceImpl::currentPimpl() );
	TTF_STREAM("Rendering " << *this << "\n");

	RenIDeviceImpl*   devImpl = RenIDeviceImpl::currentPimpl();
	const size_t nVertices = pImpl_->nVertices();

	// Project the centre of the TTF into what D3D calls "post-perspective
	// homogenous space."
	MATHEX_SCALAR w=1;
	MexPoint3d cen = centre();
	transformPt(xform, &cen, &w);
	TTF_STREAM("Centre " << cen << ", w=" << w << std::endl);

    glm::mat4 proj = RenDevice::current()->getProjectionMatrix();

	// The z-offset needs to be scaled by the 3,3 element of the projection
	// matrix.  (This is apparent if you look at the maths of the matrices.)
	// The centre point is affected by any scale in the world matrix.  However,
	// the depth offset, width and height are not, so we need to scale them
	// separately.
	const MATHEX_SCALAR scaledOffset = depthOffset() * scale;
	cen.z(cen.z() + proj[2][2] * scaledOffset);

	// We need two homogeneous w values.  The first does not take the z-offset
	// into	account and is used to compute the 2D co-ordinates of the projected
	// polygon (just as if it hadn't been offset).  The second is used to
	// compute the z values used for z-buffering, i.e. the desired offset.
	const double w2 = w + proj[2][3] * scaledOffset;

	TTF_STREAM("Centre " << cen << ", w2=" << w2 << std::endl);

	// Clip the centre of the TTF against the hither and yon clipping
	// planes.  As the TTF is parallel to the screen, we can accept or
	// reject the whole poly based on a single depth value.  In this
	// co-ordinate space, the hither plane is at z=0 and the yon plane
	// is at z=w.
	if (cen.z() < 0 || cen.z() > w2)
		return;
	ASSERT(w != 0, "Error in TTF clipping algorithm.");

	// Create four new vertices -- the corners of the TTF poly.
	const MATHEX_SCALAR recipW = 1 / w;
	cen.x(cen.x() * recipW);
	cen.y(cen.y() * recipW);
	cen.z(cen.z() / w2);
	const MATHEX_SCALAR projectedScale = recipW * scale;

//	const GLenum tmpType = compose2DGeometry(pImpl_->vertices(), cen, projectedScale);
    MexPoint3d center = centre();
    const glm::mat4& modelMatrix = RenDevice::current()->getModelMatrix();
    glm::vec4 centerWorld = glm::vec4(center.x(), center.y(), center.z(), 1.0f);
    centerWorld = modelMatrix * centerWorld;
    center.x(centerWorld[0]); center.y(centerWorld[1]); center.z(centerWorld[2]);
	const GLenum tmpType = compose2DGeometry(pImpl_->vertices(), center, scale);

	RenIIlluminator* ill = devImpl->illuminator();
	ill->applyMaterialGenericLighting(mat, pImpl_->vertices(), nVertices);

	TTF_DATA(for (int i=0; i!=nVertices; ++i))
		TTF_STREAM("vtx[" << i << "]=" << pImpl_->vertices()[i] << std::endl);

	// If we've been given a texture co-orindate animation, apply it,
	// otherwise just copy the UV values to the Direct3D structure.
	const bool doUVAnim = anim && anim->appliesTo(mat.texture());


	// Map the co-ordinates from camera space to the viewport's screen coords.
	for (int i=0; i!=nVertices; ++i)
	{
        RenIVertex& vtx = pImpl_->vertices()[i];
        vtx.w = (1.0);

		const MexVec2& uv = pImpl_->uvs()[i];

		if (doUVAnim)
		{
			anim->transform(uv.x(), uv.y(), &(vtx.tu), &(vtx.tv));
		}
		else
		{
			vtx.tu = (uv.x());
			vtx.tv = (uv.y());

		}

		TTF_STREAM("vtx[" << i << "]=" << vtx << std::endl);
	}

	if (mat.hasAlphaTransparency())
	{
		if (devImpl->isAlphaSortingEnabled())
		{
			// Shove it into the post-sorter.
	 		RenITLPolygon* poly = _NEW(RenITLPolygon(pImpl_, mat, cen.z()*w2, tmpType));
	 		std::auto_ptr<RenIDepthSortedItem> item (poly);
	 		devImpl->alphaSorter().addItem(item);
		}
		else
		{
			// There is no post-sorter, so enable alpha and render it now.
			devImpl->enableAlphaBlending();
			pImpl_->renderGL(pImpl_->vertices(), mat, tmpType);
			devImpl->disableAlphaBlending();
		}
	}
	else
	{
		pImpl_->renderGL(pImpl_->vertices(), mat, tmpType);
	}

	// TBD: maybe take this out of the final version for performance.
	RenStats* stats = devImpl->statistics();
	if (stats)
		stats->incrTTFCount(1);
}

const MexVec2& RenTTFPolygon::uv(Ren::VertexIdx i) const
{
	PRE(pImpl_);
	PRE(i < nVertices());
	return pImpl_->uvs()[i];
}

void RenTTFPolygon::uv(const MexVec2& v, Ren::VertexIdx i)
{
	PRE(pImpl_);
	PRE(i < nVertices());
	pImpl_->uvs()[i] = v;
}

size_t RenTTFPolygon::nVertices() const
{
	PRE(pImpl_);
	return pImpl_->nVertices();
}

// virtual
bool RenTTFPolygon::isRectangle() const
{
	return false;
}

void perWrite( PerOstream& ostr, const RenTTFPolygon& polygon )
{
	// Note: we don't do most of the pimple members:
	//		vertices_	is a temporary used for rendering
	//		indices_	is owned by the sub-class and set-up in its PerConstructor
	//		nVertices_	is always set by the sub-class PerConstructor
	//		nIndices_	ditto
	PRE(polygon.pImpl_);
    ostr << polygon.centre;
    ostr << polygon.depthOffset;
    ostr << polygon.pImpl_->material();

	for (Ren::VertexIdx i=0; i!=polygon.nVertices(); ++i)
		ostr << polygon.uv(i);
}

void perRead( PerIstream& istr, RenTTFPolygon& polygon )
{
	PRE(polygon.pImpl_);
    istr >> polygon.centre;
    istr >> polygon.depthOffset;

	RenMaterial mat;
    istr >> mat;
	polygon.pImpl_->material(mat);

	MexVec2 tmpVec;
	for (Ren::VertexIdx i=0; i!=polygon.nVertices(); ++i)
	{
		istr >> tmpVec;
		polygon.uv(tmpVec, i);
	}
}

ostream& operator <<( ostream& ostr, const RenTTFPolygon& polygon )
{
    polygon.write( ostr );
    return ostr;
}

// virtual
void RenTTFPolygon::write(ostream&) const
{
}

//--------------------------Rectangle concrete sub-class-----------------------
RenTTFRectangle::RenTTFRectangle():
	RenTTFPolygon(4, indices(), 4),
	width_(1),
	height_(1)
{
	PRE(Ren::initialised());

	// TBD: verify that these default UV co-ordinates are sensible.
	uv(MexVec2(0,1), 0);
	uv(MexVec2(0,0), 1);
	uv(MexVec2(1,0), 2);
	uv(MexVec2(1,1), 3);
}

RenTTFRectangle::RenTTFRectangle(const RenTTFRectangle& rhs):
	RenTTFPolygon(rhs),
	width_(rhs.width_),
	height_(rhs.height_)
{
}

// virtual
RenTTFRectangle::~RenTTFRectangle()
{
}

// virtual
RenTTFPolygon* RenTTFRectangle::clone() const
{
	return _NEW(RenTTFRectangle(*this));
}

// virtual
GLenum RenTTFRectangle::compose2DGeometry
(
	RenIVertex* vtx,
	const MexPoint3d& centre,
	MATHEX_SCALAR scale
) const
{
    const glm::mat4& vMatrix = RenDevice::current()->getViewMatrix();
    glm::vec3 right     (vMatrix[0][0], vMatrix[1][0], vMatrix[2][0]);
    glm::vec3 up        (vMatrix[0][1], vMatrix[1][1], vMatrix[2][1]);
    glm::vec3 center    ((float)centre.x(), (float)centre.y(), (float)centre.z());
    scale *= 0.5;
    right   *=          (float)width_ * scale;
    up      *=          (float)height_ * scale;

    glm::vec3 a, b, c, d;
    a = center + (right - up);
    b = center + (right + up);
    c = center - (right - up);
    d = center - (right + up);

    vtx[0].x = a.x; vtx[0].y = a.y; vtx[0].z = a.z;
    vtx[1].x = b.x; vtx[1].y = b.y; vtx[1].z = b.z;
    vtx[2].x = c.x; vtx[2].y = c.y; vtx[2].z = c.z;
    vtx[3].x = d.x; vtx[3].y = d.y; vtx[3].z = d.z;

	return GL_TRIANGLE_FAN;
}
// static
const Ren::VertexIdx* RenTTFRectangle::indices()
{
	static const Ren::VertexIdx idx[4] = { 0,1,2,3 };
	return idx;
}

double RenTTFRectangle::width() const
{
	return width_;
}

double RenTTFRectangle::height() const
{
	return height_;
}

void RenTTFRectangle::width(double w)
{
	PRE(w > 0);
	width_ = w;
}

void RenTTFRectangle::height(double h)
{
	PRE(h > 0);
	height_ = h;
}

// virtual
MATHEX_SCALAR RenTTFRectangle::radius() const
{
	// If our TTFs had corners this wouldn't be accurate, we need the Euclidean
	// distance between the vertices.  However, 99% of TTFs have circular-ish
	// textures, so this is probabaly the best measure in most cases.
	return 0.5 * std::max(width_, height_);
}

// virtual
bool RenTTFRectangle::isRectangle() const
{
	return true;
}

// virtual
RenTTFRectangle& RenTTFRectangle::asRectangle()
{
	PRE(isRectangle());
	return *this;
}

// virtual
const RenTTFRectangle& RenTTFRectangle::asRectangle() const
{
	PRE(isRectangle());
	return *this;
}

// Note this is a bit non-standard.  It doesn't call a PerConstructor for
// the base class because we *must* set-up the indices in the base class.
// The indices cannot be persisted because they are statically allocated.
RenTTFRectangle::RenTTFRectangle(PerConstructor):
	RenTTFPolygon(4, indices(), 4)
{
}

void perWrite( PerOstream& ostr, const RenTTFRectangle& rect )
{
    const RenTTFPolygon& base = rect;

    ostr << base;
    ostr << rect.width_;
    ostr << rect.height_;
}

void perRead( PerIstream& istr, RenTTFRectangle& rect )
{
    RenTTFPolygon& base = rect;

    istr >> base;
    istr >> rect.width_;
    istr >> rect.height_;
}

// virtual
void RenTTFRectangle::write( ostream& ostr ) const
{
    ostr << "TTF rectangle: ";
    ostr << centre() << " w=" << width() << " h=" << height();
    ostr << " depth off=" << depthOffset() << " " << material();
}

/* End TTFPOLY.CPP **************************************************/
