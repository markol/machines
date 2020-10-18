/*
 * S P I N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_SPIN_HPP
#define _REN_SPIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/t3dkey.hpp"
#include "mathex/transf3d.hpp"
#include "render/material.hpp"

//class ostream;
class MexTransform3d;
class RenScale;
class RenUVTransform;
class RenIVertexData;

class RenSpinTFPolygon
{
public:
	class SpinAxis
	{
		public:
			SpinAxis();
			SpinAxis(const SpinAxis& newSpinAxis);
			SpinAxis(const MexPoint3d& b, const MexVec3& d = MexVec3(1, 0, 0));

			const MexPoint3d& base() const	{return base_;}
			void base(const MexPoint3d& b)	{base_ = b;}

			const MexVec3& direction() const	{return direction_;}
			void direction(const MexVec3& d);

			bool isPerpendicularToY() const {return isPerpendicularToY_; }

			// At present the direction is fixed (in the local co-ordinate system).
			// It is the local x-axis.  This simplifies lots of math.
			//static const MexVec3& direction();

		    SpinAxis& operator=(const SpinAxis& copyMe)		{ base_ = copyMe.base_; direction_ = copyMe.direction_; return *this; }
	    	bool operator ==( const SpinAxis& that)			{ return (this->base_ == that.base_) && (this->direction_ == that.direction_); }

		    void CLASS_INVARIANT;

            PER_MEMBER_PERSISTENT_DEFAULT_INLINE( SpinAxis );
            PER_FRIEND_READ_WRITE( SpinAxis );

		private:
			MexPoint3d base_;
			MexVec3 direction_;
			bool isPerpendicularToY_;
	};

	typedef ctl_vector<MexPoint2d> Vertices;

	enum NormalType { USE_AXIS, SPUN_POLYGON_NORMAL };

	// PRE(corners.size() == uv.size());
	RenSpinTFPolygon(const Vertices& corners, const Vertices& uv, const RenMaterial&,
					 const SpinAxis&, NormalType = USE_AXIS);
	RenSpinTFPolygon(NormalType = USE_AXIS);
	RenSpinTFPolygon(const RenSpinTFPolygon& copyMe );

    virtual ~RenSpinTFPolygon();

	const RenIVertexData* vertices() const;
	void vertices(const Vertices& verts);

	const RenMaterial& material() const		{return material_;}
	void material(const RenMaterial& m)		{material_ = m;}

	const Vertices& uv() const				{return uv_;}
	void uv(const Vertices& xy)				{uv_ = xy;}

	const SpinAxis& spinAxis() const		{return spinAxis_;}
	void spinAxis(const SpinAxis& sa)		{spinAxis_ = sa;}

	const MexPoint2d uv(const size_t index) const 		{return uv_[index];}
	void uv(const size_t index, const MexPoint2d xy)	{uv_[index] = xy;}

	NormalType normalType() const		{ return normalType_; }
	void normalType(NormalType n)		{ normalType_ = n; }

	RenSpinTFPolygon& operator=(const RenSpinTFPolygon& copyMe);

    friend ostream& operator <<( ostream& o, const RenSpinTFPolygon& t );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenSpinTFPolygon );
    PER_FRIEND_READ_WRITE( RenSpinTFPolygon );

private:
	friend class RenMesh;
	void render(const MexTransform3d&, const RenScale&, const RenUVTransform*) const;
	void render(const MexTransform3d&, const RenMaterial&, const RenScale&, const RenUVTransform*) const;

	void makeOrderCorrect( const Vertices& points, Vertices *pOrderedPoints );
	void copyVertices( const Vertices& pts );

	void spinPlan( const MexPoint3d& cameraPosition,
							 const MexPoint3d& origin,
						   	 const MexVec3& xBasis,
							MexVec3* pNormal ) const;

	void applyUVTransform(const RenUVTransform*, const RenMaterial&) const;

	RenIVertexData*	vertices_;
	Vertices 		uv_;
	RenMaterial 	material_;
	SpinAxis 		spinAxis_;
	NormalType		normalType_;
	bool			verticesOrdered_;

	static size_t	maxVertices_;
};


PER_DECLARE_PERSISTENT( RenSpinTFPolygon );

#endif

/* End RENSPIN.HPP **************************************************/
