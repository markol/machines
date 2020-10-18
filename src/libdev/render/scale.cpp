/*
 * S C A L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/scale.hpp"
#include "mathex/vec2.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

#ifndef _INLINE
    #include "render/scale.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( RenScale );
PER_DEFINE_PERSISTENT( RenUnityScale );
PER_DEFINE_PERSISTENT( RenUniformScale );
PER_DEFINE_PERSISTENT( RenNonUniformScale );

//////////////////////////////////////////////////////////////////////////
ostream& operator <<( ostream& o, const RenScale& t )
{
	t.print(o);
    return o;
}

//////////////////////////////////////////////////////////////////////////
// virtual
void RenUnityScale::apply(glm::mat4*) const
{
	// Do nothing -- leave the given matrix untouched.
}

// virtual
void RenUnityScale::apply(MexPoint3d*) const
{
	// Leave the point as it is.
}

// virtual
void RenUnityScale::project(const MexVec2&, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	*inPlane = 1;
	*x       = 1;
}

// virtual
void RenUnityScale::yzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	*inPlane = 1;
	*x       = 1;
}

//virtual
void RenUnityScale::xzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* y) const
{
	PRE(inPlane && y);
	*inPlane = 1;
	*y       = 1;
}
//virtual
void RenUnityScale::xyAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* z) const
{
	PRE(inPlane && z);
	*inPlane = 1;
	*z       = 1;
}

// virtual
void RenUnityScale::print(ostream& o) const
{
	o << "unity scale";
}

// virtual
RenScale* RenUnityScale::clone() const
{
	return _NEW(RenUnityScale);
}

//////////////////////////////////////////////////////////////////////////
RenUniformScale::RenUniformScale(MATHEX_SCALAR f):
	factor_(f)
{
	PRE(f>0);
	PRE(!Mathex::isNan(f));
}

// virtual
void RenUniformScale::apply(glm::mat4* matrix) const
{
	PRE(matrix);
	(*matrix)[0][0] *= factor_;
	(*matrix)[0][1] *= factor_;
	(*matrix)[0][2] *= factor_;
	(*matrix)[1][0] *= factor_;
	(*matrix)[1][1] *= factor_;
	(*matrix)[1][2] *= factor_;
	(*matrix)[2][0] *= factor_;
	(*matrix)[2][1] *= factor_;
	(*matrix)[2][2] *= factor_;
}

// virtual
void RenUniformScale::apply(MexPoint3d* pt) const
{
	PRE(pt);
	pt->setPoint(pt->x() * factor_, pt->y() * factor_, pt->z() * factor_);
}

// virtual
void RenUniformScale::project(const MexVec2&, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	*inPlane = factor_;
	*x       = factor_;
}

// virtual
void RenUniformScale::yzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	*inPlane = factor_;
	*x       = factor_;
}

// virtual
void RenUniformScale::xzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* y) const
{
	PRE(inPlane && y);
	*inPlane = factor_;
	*y       = factor_;
}

// virtual
void RenUniformScale::xyAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* z) const
{
	PRE(inPlane && z);
	*inPlane = factor_;
	*z       = factor_;
}

// virtual
void RenUniformScale::print(ostream& o) const
{
	o << "uniform scale of " << factor_;
}

// virtual
RenScale* RenUniformScale::clone() const
{
	return _NEW(RenUniformScale(*this));
}

//////////////////////////////////////////////////////////////////////////
RenNonUniformScale::RenNonUniformScale(MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z):
	x_(x),
	y_(y),
	z_(z),
	yz_(y*z)
{
	PRE(x>0 && y>0 && z>0);
	PRE(!Mathex::isNan(x));
	PRE(!Mathex::isNan(y));
	PRE(!Mathex::isNan(z));
}

// virtual
void RenNonUniformScale::apply(glm::mat4* matrix) const
{
	PRE(matrix);
	(*matrix)[0][0] *= x_;
	(*matrix)[0][1] *= x_;
	(*matrix)[0][2] *= x_;
	(*matrix)[1][0] *= y_;
	(*matrix)[1][1] *= y_;
	(*matrix)[1][2] *= y_;
	(*matrix)[2][0] *= z_;
	(*matrix)[2][1] *= z_;
	(*matrix)[2][2] *= z_;
}
// virtual
void RenNonUniformScale::apply(MexPoint3d* pt) const
{
	PRE(pt);
	pt->setPoint(pt->x() * x_, pt->y() * y_, pt->z() * z_);
}

// virtual
void RenNonUniformScale::project(const MexVec2& yz, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	PRE(fabs(1 - yz.modulus()) <= MexEpsilon::instance());

	const MATHEX_SCALAR tx = yz.x() * z_;
	const MATHEX_SCALAR ty = yz.y() * y_;

	*inPlane = yz_ / sqrt(tx*tx + ty*ty);
	*x		 = x_;
}

// virtual
void RenNonUniformScale::yzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const
{
	PRE(inPlane && x);
	*inPlane = 0.5 * (y_ + z_);
	*x       = x_;
}

// virtual
void RenNonUniformScale::xzAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* y) const
{
	PRE(inPlane && y);
	*inPlane = 0.5 * (x_ + z_);
	*y       = y_;
}

// virtual
void RenNonUniformScale::xyAverage(MATHEX_SCALAR* inPlane, MATHEX_SCALAR* z) const
{
	PRE(inPlane && z);
	*inPlane = 0.5 * (x_ + y_);
	*z       = z_;
}

// virtual
void RenNonUniformScale::print(ostream& o) const
{
	o << "non-uniform scale of " << x_ << "," << y_ << "," << z_;
}

// virtual
RenScale* RenNonUniformScale::clone() const
{
	return _NEW(RenNonUniformScale(*this));
}

RenUnityScale& RenScale::asUnity()
{
    PRE( isUnity() );
    return _STATIC_CAST( RenUnityScale&, *this );
}

const RenUnityScale& RenScale::asUnity() const
{
    PRE( isUnity() );
    return _STATIC_CAST( const RenUnityScale&, *this );
}

RenUniformScale& RenScale::asUniform()
{
    PRE( isUniform() );
    return _STATIC_CAST( RenUniformScale&, *this );
}

const RenUniformScale& RenScale::asUniform() const
{
    PRE( isUniform() );
    return _STATIC_CAST( const RenUniformScale&, *this );
}

RenNonUniformScale& RenScale::asNonUniform()
{
    PRE( isNonUniform() );
    return _STATIC_CAST( RenNonUniformScale&, *this );
}

const RenNonUniformScale& RenScale::asNonUniform() const
{
    PRE( isNonUniform() );
    return _STATIC_CAST( const RenNonUniformScale&, *this );
}

//virtual
bool RenUnityScale::isUnity() const
{
    return true;
}

//virtual
bool RenUnityScale::isUniform() const
{
    return false;
}

//virtual
bool RenUnityScale::isNonUniform() const
{
    return false;
}

//virtual
bool RenUniformScale::isUnity() const
{
    return false;
}

//virtual
bool RenUniformScale::isUniform() const
{
    return true;
}

//virtual
bool RenUniformScale::isNonUniform() const
{
    return false;
}

//virtual
bool RenNonUniformScale::isUnity() const
{
    return false;
}

//virtual
bool RenNonUniformScale::isUniform() const
{
    return false;
}

//virtual
bool RenNonUniformScale::isNonUniform() const
{
    return true;
}

RenUniformScale::RenUniformScale( const RenUniformScale& a, const RenUniformScale& b )
:   factor_( a.factor_ * b.factor_ )
{
    PRE( factor_ > 0.0 );
}

RenNonUniformScale::RenNonUniformScale( const RenNonUniformScale& a, const RenUniformScale& b )
:   x_( a.x_ * b.factor_ ),
    y_( a.y_ * b.factor_ ),
    z_( a.z_ * b.factor_ ),
    yz_( y_ * z_ )
{
    PRE( x_ > 0.0 );
    PRE( y_ > 0.0 );
    PRE( z_ > 0.0 );
}

RenNonUniformScale::RenNonUniformScale( const RenNonUniformScale& a, const RenNonUniformScale& b )
:   x_( a.x_ * b.x_ ),
    y_( a.y_ * b.y_ ),
    z_( a.z_ * b.z_ ),
    yz_( y_ * z_ )
{
    PRE( x_ > 0.0 );
    PRE( y_ > 0.0 );
    PRE( z_ > 0.0 );
}

RenUniformScale::RenUniformScale( const RenUniformScale& rhs )
:   factor_( rhs.factor_)
{
}

RenUniformScale& RenUniformScale::operator =( const RenUniformScale& rhs )
{
    factor_ = rhs.factor_;
    return *this;
}

RenNonUniformScale::RenNonUniformScale( const RenNonUniformScale& rhs )
:   x_( rhs.x_),
    y_( rhs.y_),
    z_( rhs.z_)
{
}

RenNonUniformScale& RenNonUniformScale::operator =( const RenNonUniformScale& rhs )
{
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
    yz_ = rhs.yz_;

    return *this;
}

void RenNonUniformScale::factors( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z )
{
    PRE( x > 0 );
    PRE( y > 0 );
    PRE( z > 0 );
	PRE(!Mathex::isNan(x));
	PRE(!Mathex::isNan(y));
	PRE(!Mathex::isNan(z));

    x_ = x;
    y_ = y;
    z_ = z;
    yz_ = y * z;
}

RenNonUniformScale::RenNonUniformScale( const RenNonUniformScale& rhs, const MexTransform3d& t )
{
    //Get the basis vectors for each axis and hence compute the multipliers
    MATHEX_SCALAR a,b,c;
    MexVec3 xv = t.xBasis();
    a = xv.x();
    b = xv.y();
    c = xv.z();
    x_ = a * a * rhs.x_  +  b * b * rhs.y_  +  c * c *rhs.z_;

    MexVec3 yv = t.yBasis();
    a = yv.x();
    b = yv.y();
    c = yv.z();
    y_ = a * a * rhs.x_  +  b * b * rhs.y_  +  c * c *rhs.z_;

    MexVec3 zv = t.zBasis();
    a = zv.x();
    b = zv.y();
    c = zv.z();
    z_ = a * a * rhs.x_  +  b * b * rhs.y_  +  c * c *rhs.z_;

    yz_ = y_ * z_;
}

//virtual
MATHEX_SCALAR RenNonUniformScale::average() const
{
    return (x_ + y_ + z_) * 0.33333333;
}

//virtual
MATHEX_SCALAR RenUniformScale::average() const
{
    return factor_;
}

//virtual
MATHEX_SCALAR RenUnityScale::average() const
{
    return 1.0;
}

RenUniformScale::RenUniformScale( PerConstructor )
{
}

RenNonUniformScale::RenNonUniformScale( PerConstructor )
{
}

void perWrite( PerOstream&, const RenScale& )
{
}

void perRead( PerIstream&, RenScale& )
{
}

void perWrite( PerOstream& ostr, const RenUnityScale& scale )
{
    const RenScale& base = scale;
    ostr << base;
}

void perRead( PerIstream& istr, RenUnityScale& scale )
{
    RenScale& base = scale;
    istr >> base;
}

void perWrite( PerOstream& ostr, const RenUniformScale& scale )
{
    const RenScale& base = scale;
    ostr << base;
    ostr << scale.factor_;
}

void perRead( PerIstream& istr, RenUniformScale& scale )
{
    RenScale& base = scale;
    istr >> base;
    istr >> scale.factor_;
}

void perWrite( PerOstream& ostr, const RenNonUniformScale& scale )
{
    const RenScale& base = scale;
    ostr << base;
    ostr << scale.x_;
    ostr << scale.y_;
    ostr << scale.z_;
    ostr << scale.yz_;
}

void perRead( PerIstream& istr, RenNonUniformScale& scale )
{
    RenScale& base = scale;
    istr >> base;
    istr >> scale.x_;
    istr >> scale.y_;
    istr >> scale.z_;
    istr >> scale.yz_;
}

/* End SCALE.CPP ****************************************************/
