/*
 * S C A L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_SCALE_HPP
#define _RENDER_SCALE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/point3d.hpp"
#include <glm/gtc/matrix_transform.hpp>

class MexVec2;
class MexPoint3d;
class MexTransform3d;
class RenUnityScale;
class RenUniformScale;
class RenNonUniformScale;

// A abstract base class for (potentially) arbitrary geometric scaling
// functions.  In practice, completely arbitrary scaling may be impractical
// due to the complexity of the math.
class RenScale
{
public:
    //NB the derived classes defined below are intended to be the complete set

    //true if performs no scaling
    virtual bool isUnity() const = 0;

    //true iff applies a uniform scale
    virtual bool isUniform() const = 0;

    //true if applies a non-unform scale
    virtual bool isNonUniform() const = 0;

    //Cast to derived type. Safe if derived classes limited to those defined below
    RenUnityScale& asUnity();
    const RenUnityScale& asUnity() const;
    //PRE( isUnity() )

    RenUniformScale& asUniform();
    const RenUniformScale& asUniform() const;
    //PRE( isUniform() )

    RenNonUniformScale& asNonUniform();
    const RenNonUniformScale& asNonUniform() const;
    //PRE( isNonUniform() )

    //Returns a generic average scale
    virtual MATHEX_SCALAR average() const = 0;

	// Apply this scale to the given Direct3D matrix.
	virtual void apply(glm::mat4* mat) const = 0;		// PRE(mat);

	// Apply this scale to the given 3D point.
	virtual void apply(MexPoint3d* pt) const = 0;		// PRE(pt);

	// Find the resultant scale in the plane defined by the x-axis and this
	// vector (in y-z).  The implementation is trivial for unity & uniform.
	// There are two results: the component along the x-axis and the resultant
	// component in the given plane.  Used by spin-to-face math.
	// PRE(yz.modulus() == 1);  (plus or minus MexEpsilon::instance())
	// PRE(inPlane && x);
	virtual void project(const MexVec2& yz, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const = 0;

	// Like project, but it doesn't bother to compute the in-plane component.
	// Instead it just provides the average of the y-z scale components.
	virtual void yzAverage(MATHEX_SCALAR* yzAverage, MATHEX_SCALAR* x) const = 0;

	//similarly
	virtual void xzAverage(MATHEX_SCALAR* xzAverage, MATHEX_SCALAR* y) const = 0;
	virtual void xyAverage(MATHEX_SCALAR* xyAverage, MATHEX_SCALAR* z) const = 0;

	virtual void print(ostream&) const = 0;
	virtual RenScale* clone() const = 0;
    friend ostream& operator <<( ostream& o, const RenScale& t );
    virtual ~RenScale(){};

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( RenScale );
};

PER_READ_WRITE( RenScale );
PER_DECLARE_PERSISTENT( RenScale );

// A no-effect scale, i.e. the scale factor is one.
class RenUnityScale : public RenScale
{
public:
	virtual void apply(glm::mat4*) const;
	virtual void apply(MexPoint3d*) const;
	virtual void project(const MexVec2& yz, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const;
	virtual void yzAverage(MATHEX_SCALAR* yzAverage, MATHEX_SCALAR* x) const;
	virtual void xzAverage(MATHEX_SCALAR* xzAverage, MATHEX_SCALAR* y) const;
	virtual void xyAverage(MATHEX_SCALAR* xyAverage, MATHEX_SCALAR* z) const;
	virtual void print(ostream&) const;
	virtual RenScale* clone() const;
    virtual bool isUnity() const;
    virtual bool isUniform() const;
    virtual bool isNonUniform() const;
    virtual MATHEX_SCALAR average() const;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( RenUnityScale );
};

PER_READ_WRITE( RenUnityScale );
PER_DECLARE_PERSISTENT( RenUnityScale );

// A scale whose effect is the same in all directions.
class RenUniformScale : public RenScale
{
public:
	explicit RenUniformScale(MATHEX_SCALAR factor);		// PRE(factor > 0);
    explicit RenUniformScale( const RenUniformScale& rhs );
    explicit RenUniformScale( const RenUniformScale& a, const RenUniformScale& b );
    RenUniformScale& operator =( const RenUniformScale& rhs );
	virtual void apply(glm::mat4*) const;
	virtual void apply(MexPoint3d*) const;
	virtual void project(const MexVec2& yz, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const;
	virtual void yzAverage(MATHEX_SCALAR* yzAverage, MATHEX_SCALAR* x) const;
	virtual void xzAverage(MATHEX_SCALAR* xzAverage, MATHEX_SCALAR* y) const;
	virtual void xyAverage(MATHEX_SCALAR* xyAverage, MATHEX_SCALAR* z) const;
	virtual void print(ostream&) const;
	virtual RenScale* clone() const;
    virtual bool isUnity() const;
    virtual bool isUniform() const;
    virtual bool isNonUniform() const;
    virtual MATHEX_SCALAR average() const;
    MATHEX_SCALAR factor() const;
    void factor( MATHEX_SCALAR f );

    PER_MEMBER_PERSISTENT_VIRTUAL( RenUniformScale );
    PER_FRIEND_READ_WRITE( RenUniformScale );

private:
    friend class RenNonUniformScale;

	MATHEX_SCALAR factor_;
};

PER_DECLARE_PERSISTENT( RenUniformScale );

// A scale with different scale factors in the x, y and z directions.
class RenNonUniformScale : public RenScale
{
public:
	// PRE(x>0 && y>0 && z>0);
	RenNonUniformScale(MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z);
    RenNonUniformScale( const RenNonUniformScale& rhs );
    RenNonUniformScale( const RenNonUniformScale& a, const RenUniformScale& b );
    RenNonUniformScale( const RenNonUniformScale& a, const RenNonUniformScale& b );
    RenNonUniformScale( const RenNonUniformScale& a, const MexTransform3d& t );
    RenNonUniformScale& operator =( const RenNonUniformScale& rhs );
	virtual void apply(glm::mat4*) const;
	virtual void apply(MexPoint3d*) const;
	virtual void project(const MexVec2& yz, MATHEX_SCALAR* inPlane, MATHEX_SCALAR* x) const;
	virtual void yzAverage(MATHEX_SCALAR* yzAverage, MATHEX_SCALAR* x) const;
	virtual void xzAverage(MATHEX_SCALAR* xzAverage, MATHEX_SCALAR* y) const;
	virtual void xyAverage(MATHEX_SCALAR* xyAverage, MATHEX_SCALAR* z) const;
	virtual void print(ostream&) const;
	virtual RenScale* clone() const;
    virtual bool isUnity() const;
    virtual bool isUniform() const;
    virtual bool isNonUniform() const;
    virtual MATHEX_SCALAR average() const;
    MATHEX_SCALAR x() const;
    MATHEX_SCALAR y() const;
    MATHEX_SCALAR z() const;
    void factors( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z );

    PER_MEMBER_PERSISTENT_VIRTUAL( RenNonUniformScale );
    PER_FRIEND_READ_WRITE( RenNonUniformScale );

private:
	// The yz product is a constant used in the project function.  There
	// is a pure space-time trade-off here: 4 bytes versus one multiply.
	MATHEX_SCALAR x_, y_, z_, yz_;
};

PER_DECLARE_PERSISTENT( RenNonUniformScale );

#ifdef _INLINE
    #include "render/scale.ipp"
#endif

#endif

/* End SCALE.HPP ****************************************************/
