/*
 * T R A N S F 3 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved


	If you change the data members in this class it is very important to remember to change
	the compressed version as well.


 */

#ifndef _MEX_TRANSF3D_HPP
#define _MEX_TRANSF3D_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/vec3.hpp"
#include "mathex/t3dkey.hpp"

class MexPoint3d;
class MexPoint2d;
class MexLine2d;
class MexQuaternion;
class MexEulerAngles;
class MexCompressedTransform3d;
class MexAlignedBox2d;

typedef MATHEX_SCALAR MexTransformMatrix3d[4][3];
typedef float MexFloatMatrix3d[4][4];

//////////////////////////////////////////////////////////////////////

//Represents a 3d spatial transformation involving rotation and translation,
//but NOT scaling, perspective, mirroring, etc
class MexTransform3d
{
public:
    MexTransform3d( void );

    MexTransform3d( const MexTransform3d& );
    MexTransform3d( const MexQuaternion& );
    MexTransform3d( const MexEulerAngles& );
    MexTransform3d( const MexPoint3d& );
    MexTransform3d( const MexQuaternion&, const MexPoint3d& );
    MexTransform3d( const MexEulerAngles&, const MexPoint3d& );

    MexTransform3d(
        const MexVec3& xBasis,
        const MexVec3& yBasis,
        const MexVec3& zBasis,
        const MexPoint3d& position );

    MexTransform3d( const MATHEX_SCALAR[4][4] );
    MexTransform3d( const MATHEX_SCALAR[3][3] );
// GCC has some troubles with overloading this
//    MexTransform3d( const MATHEX_SCALAR[4][3] );
    MexTransform3d( const MATHEX_SCALAR[4*3] );

    MexTransform3d( const MexCompressedTransform3d& );

    enum AxisPlane  { X_XY, X_XZ, Y_YZ, Y_YX, Z_ZX, Z_ZY };

    //  Create a transform given the direction of one of the axes
    //  ( defined by axisDirection ) and the orientation of a plane
    //  ( given by the plane containing axisDirection and
    //  planeDefiningVector )

    MexTransform3d(
      AxisPlane axisPlane,
      const MexVec3& axisDirection,
      const MexVec3& planeDefiningVector,
      const MexPoint3d& position );
    // PRE( not axisDirection.parallel( planeDefiningVector ) );

    ~MexTransform3d();


    //Modify the transform to include an additional translation relative to
    //its transformed axis system.
    void translate( const MexPoint3d& );
    void translate( const MexTransform3d& );

    //Set the translation component of the transform
    void position( const MexPoint3d& newPosition );
    //  PRE( isValid( newPos ) );

    //Get the translation component of the transform
    void position( MexPoint3d* pPos ) const;
    MexPoint3d  position( void ) const;

    //Modify the transform to include an additional rotation relative to
    //its transformed axis system.
    void rotate( const MexEulerAngles& );
    void rotate( const MexQuaternion& );
    void rotate( const MexTransform3d& );

    //Set just the rotation component of the transform
    void rotation( const MexQuaternion& params );
    void rotation( const MexEulerAngles& angles );
    void rotation( const MexTransform3d& tx );

    //Get the rotation component of the transform
    void rotation( MexEulerAngles* pAngles) const;
    void rotation( MexQuaternion* pParams ) const;
    MexEulerAngles  rotationAsEulerAngles( void ) const;
    MexQuaternion   rotationAsQuaternion( void ) const;

    //this = this * t
    void transform( const MexTransform3d& t);

    //this = t * this
    void preTransform( const MexTransform3d& t);

    //*pResult = this * t
    void transform( const MexTransform3d& t, MexTransform3d* pResult ) const;

    //v = this * v
    void transform( MexVec3* pVector ) const;

    //p = this * p
    void transform( MexPoint3d* pPoint ) const;

    //pPoint = this * MexPoint3d( pPoint->x(), pPoint->y(), 0.0 );
    void transform( MexPoint2d* pPoint ) const;

    //pLine = MexLine2d( this * pLine->end1(), this * pLine->end2() )
    void transform( MexLine2d* pLine ) const;

    // The new box will encompass the corners of the old box when
    // rotated. It might therefore be bigger than the new box.
    void transform( MexAlignedBox2d* pBox ) const;

    const MexTransform3d& operator =( const MexTransform3d& rhs );
    // POST( *this == rhs );

    const MexTransform3d& operator =( const MATHEX_SCALAR rhs[4][4] );
    // POST( *this == rhs );

    //this = inV(this) * t
 	void transformInverse( const MexTransform3d& t);

    //this = t * inV(this)
	void preTransformInverse( const MexTransform3d& t);

    //*pResult = inV(this) * t
	void transformInverse( const MexTransform3d& t, MexTransform3d* pResult ) const;

    //v = inV(this) * v
	void transformInverse( MexVec3* pVector ) const;

    //p = inV(this) * p
	void transformInverse( MexPoint3d* pPoint ) const;

    //pPoint = inV(this) * MexPoint3d( pPoint->x(), pPoint->y(), 0.0 );
	void transformInverse( MexPoint2d* pPoint ) const;

    //pLine = MexLine2d( inV(this) * pLine->end1(), inV(this) * pLine->end2() )
	void transformInverse( MexLine2d* pLine ) const;

	MexTransform3d inverse() const;

	bool operator ==( const MexTransform3d& rhs ) const;
	bool operator !=( const MexTransform3d& rhs ) const;
	bool operator <( const MexTransform3d& rhs ) const;
	bool operator >( const MexTransform3d& rhs ) const;
	bool operator <=( const MexTransform3d& rhs ) const;
	bool operator >=( const MexTransform3d& rhs ) const;

	bool operator ==( const MATHEX_SCALAR rhs[4][4] ) const;



    //set the transform to that implied by m
    void setTransform( const MexFloatMatrix3d m );

    //get the transform into m
    void getTransform( MexFloatMatrix3d m ) const;

    //Invert this
	void	invert( void );

    //Return inverted this in t
	void	invert( MexTransform3d* t ) const;

    //true iff zero translation and rotation
    bool    isIdentity( void ) const;

    // Returns the transform's current unique key
    const MexTransform3dKey& key( void ) const;

    void xBasis( MexVec3* ) const;
    void yBasis( MexVec3* ) const;
    void zBasis( MexVec3* ) const;

    MexVec3 xBasis( void ) const;
    MexVec3 yBasis( void ) const;
    MexVec3 zBasis( void ) const;

    //////////////////////////////////////////////////////////////////////

    friend ostream& operator <<( ostream&, const MexTransform3d& );

    void CLASS_INVARIANT;

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexTransform3d );
    PER_FRIEND_READ_WRITE( MexTransform3d );

private:

	friend class MexCompressedTransform3d;

	void inverseTranslate(MATHEX_SCALAR invT[3]) const;

    //result = a*b. The 4th columns are assumed to be (0,0,0,1) and are not
    //directly used or set
    static void multiplyMatrices( const MexTransformMatrix3d a, const MexTransformMatrix3d b,
                           MexTransformMatrix3d result );
//    static void multiplyMatrices( const MATHEX_SCALAR a[4][4], const MATHEX_SCALAR b[4][4],
//                           MATHEX_SCALAR result[4][4] );

    //Indicates the alignment of a single vector. It might be aligned with any
    //of the 3 principal axes in either direction, or not known to be aligned
    //with anything. (Even though it might actually be).
    enum AxisAlignment
    {
        NOT_ALIGNED = 0,
        POSITIVE_X = 0x2,
        NEGATIVE_X = 0x3,
        POSITIVE_Y = 0x4,
        NEGATIVE_Y = 0x5,
        POSITIVE_Z = 0x6,
        NEGATIVE_Z = 0x7
    };

    enum HasTranslation{ TRANSLATES = 0x0, NO_TRANSLATION = 0x1 };

    //Some useful values for the flags_ field described below
    enum FlagValues
    {
        GENERAL = 0,
        IDENTITY = 0x14d,
        TRANSLATE_ONLY = 0x14c,
        ROTATE_ONLY = 0x1
    };

    //Data members
	MATHEX_SCALAR   forward_[4][3];
    MexTransform3dKey  tkey_;

    //A bitwise flag field indicating what is known about the actual content
    //of the transformation. This can be used to optimise some operations.
    //The format of the bitfield is
    //  xxxyyyzzzt
    //where each of xxx, yyy, zzz is an AxisAlignment value for the 3 basis vectors,
    //and t is a HasTranslation value indicating whether any translation is defined.
    uint flags_;
};

PER_DECLARE_PERSISTENT( MexTransform3d );

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
	#include "mathex/transf3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif  /*      #ifndef _TRANSF3D_HPP        */

/* End TRANSF3D.HPP *****************************************************/




