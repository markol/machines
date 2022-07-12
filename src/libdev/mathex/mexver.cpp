/*
* M E X V E R . C P P
* (c) Charybdis Limited, 1995. All Rights Reserved
*/

/*
	HISTORY

	01.00	First Release
			30 August 1995  Bob Archer

    01.01   Invert functions added to MexTransform3d and MexTransformRep
            4 September 1995  Bob Archer

    01.02   transformVector function added to MexTransform3d
            transformVector function added to MexTransformRep
            setVector function added to MexVec3
            4 September 1995  Bob Archer

    01.03   position method added to MexTransform3d to allow the position to be set
            position method added to MexTransformRep to allow the position to be set
            14 September 1995  Bob Archer

    01.04   intersects, isZeroVector and << added to MexVec3.
            14 September 1995  Bob Archer

    01.05   rotate methods added to MexTransform3d and MexTransformRep
            14 September 1995  Bob Archer

    01.06   << operator added to MexTransform3d
            15 September 1995  Bob Archer

    01.07   == operator for MexVec3 changed to use EPS rather than looking for
            exact equality.
            18 September 1995  Bob Archer

    01.08   New constructor added to MexTransform3d and MexTransformRep to
            allow them to be constructed from xBasis, yBasis, zBasis and
            position.
            19 September 1995  Bob Archer

    01.09   Bug in MexTransformRep::isIdentity fixed. The z value of the
            position vector was not being checked.
            
    01.10   MexTransformRep can now return its rotation as an EulerAngle.
            25 September 1995  Bob Archer
            
    01.11   MexBoundary3d and associated classes added.
            28 September 1995  Bob Archer
            
    01.12   MexPoint3d added as a boundary type.
            4 October 1995  Bob Archer
            
    01.13   MexHalfSpace3d::intersects( MexUnalignedBoundingBox3d ) function speeded up.
            MexPlane3d::distanceFrom function speeded up.
            5 October 1995  Bob Archer
            
    01.14   box3d.cpp, box3d.hpp & box3d.tpp split into two - abox3d & ubox3d.
            12 October 1995  Bob Archer
            
    01.15   MexTransformRep removed. MexTransform3d is no longer parameterised
            over MexTransformRep. MexPoint3d, MexLine3d and MexPlane3d are no
            longer derived from MexBoundary3d. ALl MexBoundary 3ds can now
            take a transform to update their position in the world (as their
            owning object's position changes).
            17 October 1995  Bob Archer

    01.16
        	const MexVec3< T >& operator =( const T * ) added
        	const MexPoint3d< T >& operator =( const T * ) added
            17 October 1995  Bob Archer
            
    01.17
            void transformVector( MexVec3< T > *v ) const; added
            17 October 1995  Bob Archer

    01.18   Strict version of the mathex library created.            
            20 October 1995  Bob Archer
            
    01.19   MexVec3 method 'argument' renamed to 'angleBetween'
            8 December 1995  Bob Archer
            
    01.20   MexBox2 and MexVec2 classes added
            17 January 1996  Bob Archer
            
    01.21   MexPoint3d class extended to allow x, y and z
            elements to be set directly.
            21 January 1996  Bob Archer

    01.22   MexVec2 now has a zeroVector function.
            MexBox2 has a void constructor.
            21 January 1996  Bob Archer

    01.23   lmo, rmo added to get the left most and right most bits set
            PI added
            PI_DIV_2 added
            SQRT_2 added
            LOG10_2 added
            EPSILON added
            round added
            isinteger added
            abs( long ) added
            abs( float ) added
            abs( double ) added
            abs( long double ) added
            sqr added
            cube added
            isodd added
            iseven added
            isprime added
            mod added
            divides added
            isprime added
            prime_ceil added
            isintegerpowerof2 added
            isintegerpowerof2 added
            integerpowerof2ceil added
            log2 added
            inIntervalOO added
            inIntervalCC added
            inIntervalOC added
            inIntervalCO added
            MexXCoord added
            MexYCoord added

            23 February 1996    Bob Archer

    01.24
            PI, PI_DIV_2, SQRT_2, LOG10_2 moved into Mathex namespace

            1 March 1996        Bob Archer

    01.25
            operator < added to MexTransform3d.
            
            4 March 1996        Bob Archer

    01.26
            Destructors for MexPoint3d and MexVec3 made virtual.
            5 March 1996        Bob Archer

    01.27   Constructor added to MexAlignedBox3d to allow it to be
            constructed from its centre point and its overall X, Y
            and Z dimensions.
            7 March 1996        Bob Archer

    01.28   rotateAboutAxis function added to MexPoint3d.
            8 March 1996        Bob Archer

    01.29   zeroBox function added to MexAlignedBox3d.
            8 March 1996        Bob Archer

	01.30	inlined functions restricted to simple functions
			13 March 1996		Olly Headey

    01.31   Fixed bug in rotateAboutAxis postconditions which would
            cause an incorrect assertion if the point to be rotated
            was on the axis of rotation.
            13 March 1996       Bob Archer
            
    01.32   lmo and rmo had been swapped over - they have now been
            defined to be the right way round.            
            19 March 1996       Bob Archer
            
    01.33   Interval functions in mathex.tpp now work properly:
                inIntervalOO
                inIntervalCC
                inIntervalOC
                inIntervalCO
            23 March 1996       Bob Archer

	01.34	Template files rearranged into new standard format
            ( .ctp, .itp, .ctf and .itf files )
            19 May 1996                         Bob Archer

	01.35	Minor error fixes (point3d.ctp to point3d.itp)
			x, y, x set function added to MexVec3 to add functionality
			and consistency with MexPoint3d.
			20 May 1996							Olly Headey

	01.36	class MexFix16Pt16 for signed fixed 15.16 point arithmetic
			added, together with corresponding template instantiation 
			file tifix.cpp.
			3 June 1996							Jerry Swan

	01.37	Increased use of forward references for interface 
			dependencies, to achieve template instantiations for
			float and MexFix16Pt16
			3 June 1996							Jerry Swan

	01.38	Replaced EPSILON with singleton template class MexEpsilon.
			Replaced all calls to fabs with call to Mathex::abs,
			which is overloaded for scalars and MexFix16Pt16.
			6 June 1996							Jerry Swan

    01.39   Fixed bug in MexVec3 class - the functions for setting y
            and z were actually setting the x value.
            26 June 1996                        Bob Archer

    01.40   #undef min and #undef max added to the top of fix.hpp to
            allow the mathex library to compile under windows.
            9 July 1996                        Bob Archer

    01.41   Compiled with new base library
            7 August 1996                       Bob Archer

    01.42   The width and height functions in MexAlignedBox2d have
            been changed so that they do not add one onto their return
            value. This is a reasonable thing to do for integer
            instantiations but not reasonable for floating point
            instantiations. The isEmpty function has been removed
            until we can agree on a reasonable interpretation of its
            results.

            In MexAlignedBox3d the width, height and isEmpty
            functions have all been removed.
            
            4th November 1996                       Bob Archer
			
	01.43	Templates have been removed from the 2D geometry classes.

			New 2D geometry classes were added: MexLine2d, MexHalfPlane2d, 
			MexPolygon2d and MexTriangle2d.
			
			Traingle-box and line-box intersection methods were added to 
			MexAlignedBox2d.
			
			Methods minCorner and maxCorner were added to MexAlignedBox2d.
			These replace topLeft and bottomRight which are now depreciated.
			
			Various stream insertion and extraction ops. were added.
			
			9 December 1996							Iain Cantlay

    01.44   MexAlignedBox2d::topLeft, MexAlignedBox2d::bottomRight,
            MexAlignedBox2d::width, MexAlignedBox2d::height all removed.
            Use minCorner instead of topLeft and maxCorner instead of
            bottomRight.
            
            10 December 1996                        Bob Archer

    01.45   MexIntersectData class removed.
            MexHalfSpace3d class temporarily removed.
            MexTransform3dKey class added.
            MexTransform3d class has MexTransform3dKey by value.
            Following methods added to MexTransform3d:
                void preTransform( const MexTransform3d& t);
                void transform( const MexTransform3d& t, MexTransform3d* pResult ) const;
                void setTransform( const MexFloatMatrix3d m );
                void getTransform( MexFloatMatrix3d m ) const;
                const MexTransform3dKey& key() const;
                void xBasis( MexVec3* ) const;
                void yBasis( MexVec3* ) const;
                void zBasis( MexVec3* ) const;
            MexBoundary3d, MexAlignedBox3d, MexUnalignedBox3d ::intersects and ::contains
            methods changed to take world transforms as arguments, instead of storing the
            world transform.
            MexAlignedBox3d has new methods makeSpinSafe() and realign() to enhance usefulness
            for rotated objects. Also, the way in which the world transform is applied was
            changed so that the centroid is transformed properly, and the box 'rotates' around
            the centroid to maintain alignment (if realign flag not set).

            11 March 1997                   Jonathan Green

    01.46   Added the MexBoundary3dPtr typedef (with instantiation) which provides a reference
            counted interface for MexBoundary3ds.
            Implemented double dispatching for MexBoundary3d::contains().

            13 March 1997                   Jonathan Green

    01.47   Added MexEulerParameters class.
            Extended MexTransform3d class to get/set rotation using MexEulerParameters

            25 March 1997                   Jonathan Green

    01.48   MexEulerAngles class removed temporarily as its rotations were working
            in the wrong direction. It has been romoved to ensure that any code
            using it breaks noisily. It will be introduced at a later date. In
            the meantime, use MexEulerParameters.
            3 April 1997                        Bob Archer

    01.49   MexVec3::cross_product renamed to crossProduct.    
            MexVec3::dot_product renamed to dotProduct.    
            5 April 1997                        Bob Archer

    01.50   The MexTransform3d constructor which takes a MexEulerParameters
            argument was not initialising the position values. This has now
            been fixed.
            9 April 1997                        Bob Archer
			
	01.51	Added a MexQuad3d class.
			11 April 1997						Iain Cantlay

    01.52   Reintroduced MexEulerAngles class
            16 April 1997                       Bob Archer

    01.53   Output operator added to MexEulerAngles class
            24 April 1997                       Bob Archer

    01.54   MexDegrees and MexRadians classes added.
            MexEulerParameters has been renamed to MexQuaternion. The
            class has been tidied up. The constructor from azimuth,
            elevation and roll angles has been removed.
            All references to angles now use MexRadians. This includes
            MexEulerAngles, MexQuaternions and MexPoint3d.

            MexTransform3d:
                const MATHEX_SCALAR* xBasis() const;
                const MATHEX_SCALAR* yBasis() const;
                const MATHEX_SCALAR* zBasis() const;
                const MATHEX_SCALAR* position() const;
                void position( MexVec3* pPos ) const;
                MexTransform3d( const MATHEX_SCALAR*, const MexQuaternion& );
                MexTransform3d( const MATHEX_SCALAR*, const MexEulerAngles& );
                MexTransform3d( const MATHEX_SCALAR* translation );
                MexTransform3d(
                    const MATHEX_SCALAR *xBasis,
                    const MATHEX_SCALAR *yBasis,
                    const MATHEX_SCALAR *zBasis,
                    const MATHEX_SCALAR *position );
                void position( const MATHEX_SCALAR* newPos );
                void translate( const MATHEX_SCALAR * );
                void transformVector( MATHEX_SCALAR* v ) const;
                all removed.

                void position( MexPoint3d* pPos ) const;
                MexTransform3d( const MexEulerAngles& );
                MexTransform3d( const MexPoint3d& );
                MexTransform3d( const MexQuaternion&, const MexPoint3d& );
                MexTransform3d( const MexEulerAngles&, const MexPoint3d& );
                void translate( const MexPoint3d& );
                void rotate( const MexEulerAngles& );
                void rotate( const MexQuaternion& );
                void position( const MexPoint3d& newPosition );
                MexVec3 xBasis() const;
                MexVec3 yBasis() const;
                MexVec3 zBasis() const;
                MexPoint3d  position() const;
                MexEulerAngles  rotationAsEulerAngles() const;
                MexQuaternion  rotationAsQuaternion() const;
                all added.

                void transformVector( MexVec3* pVector ) const;
                    changed to
                void transform( MexVec3* pVector ) const;
                
                void transformPoint( MexPoint3d* pPoint ) const;
                    changed to
                void transform( MexPoint3d* pPoint ) const;

                The inverse of a transform is no longer cached.

            MexVec3:
                MexVec3( const MATHEX_SCALAR* );
            	const MexVec3& operator =( const MATHEX_SCALAR* );
                operator MATHEX_SCALAR *();
                operator MATHEX_SCALAR const *() const;
                removed

                MATHEX_SCALAR EuclidianDistance( const MexVec3* vPtr ) const;
                    changed to
                MATHEX_SCALAR euclidianDistance( const MexVec3& ) const;

            MexPoint3d:
                MexPoint3d( const MATHEX_SCALAR * );
                const MexPoint3d& operator =( const MATHEX_SCALAR * );
                operator MATHEX_SCALAR *();
                operator MATHEX_SCALAR const *() const;
                removed

    01.55   Added Point2d class
            Added constructor for MexVec2 taking a MexPoint2d.
            Added constructor for MexVec2, to compute unit vector from 2 MexPoint2ds,
            with optional length divisor.
            Added method makeUnitVector() to MexVec2.
            Added method normal() to MexVec2.
            MexLine2d interface changed to use MexPoint2d not MexVec2.
            MexPolygon2d interface changed to use MexPoint2d not MexVec2.
            Added methods to MexPolygon2d to control caching of data,
            and test for intersection with a line joining 2 points.
            Removed method from MexPolygon2d returning non-const reference
            to a vertex, in case modifications to a vertex by the client
            violate the invariant of the polygon sub-class.
            Added MexConvexPolygon2d class.
            12 May 1997                       Jonathan Green

    01.56   Added new constructors to MexAlignedBox2d and MexConvexPolygon2d.            
            1 July 1997                       Jonathan Green

    01.57   Removed MexPoint3d::centre. Its implementation was meaningless.
            24 July 1997                        Bob Archer

    01.58   Added transform( MexPoint2d* pPoint ) const; method to MexTransform3d.
            2 September 1997                        Bob Archer

    01.59   Added MexSausage2d
            Added MexCircle2d
            New methods to Line2d and MexVec2d
            4 September 1997                        Jon Green

    01.60   Added MexConvexPolygon2d::MexConvexPolygon2d( const Points& points )
            constructor to allow a polygon to be created from a vector of
            points (as opposed to pointers to points )
            8 September 1997                        Bob Archer

    01.61   Copy constructor added to MexConvexPolygon2d.
            9 September 1997                        Bob Archer


    01.62   Added
                void MexConvexPolygon2d::transform( const MexTransform3d& transform )
                void MexTransform3d::transform( MexLine2d* pLine ) const
            10 September 1997                        Bob Archer

    01.63   Added
                bool operator ==( const MexQuaternion&, const MexQuaternion& );
            14 October 1997                        Bob Archer

    01.64   MexLine3d caches length using lazy evaluation
            MexLine3d exports unit direction vector, can be transformed
            MexVec3 has new constructors taking 2 MexPoint3d, with optional scale factor

    01.65   MexDouble class added. This allows double values to be +infinity or -infinity
            20 November 1997                        Bob Archer

    01.66   void MexTransform3d::transform( MexVec3 *v ) const changed so that the
            vector is transformed as a pure vector - i.e. only the rotational
            elements of the transform are taken into account, the position is
            completely ignored.
            28 November 1997                        Yueai Liu

    01.67   Fixed two preconditions  in MexDouble.
    		Added MexInt. Semantics as MexDouble.
    		Added MexUnsigned. Semantics as MexDouble, except that -infinity is 
    		not a permissible value.
            3 Febuary 1998                        Jerry Swan.


    01.68   New persistence mechanisn implemented.
            16 February 1998                        Bob Archer

    01.69
            Destructors for MexPoint3d and MexVec3 made non-virtual.
            23 February 1998        Bob Archer

    01.70   Persistence speedups put into MexTransform3d, MexVec3, MexPoint3d, MexDouble.
            5 March 1998            Bob Archer

    01.71   operator << added to MexLine2d.
            MexGrid2d class added
            21 May 1998             Bob Archer

    01.72   T1 mexInterpolate( const T1& inputValue,
                  const T1& inputLimit1, const T1& inputLimit2,
                  const T2& outputLimit1, const T2& outputLimit2 );
            added

            T1 mexClamp( const T1& inputValue,
              const T1& lowerLimit, const T1& upperLimit );
            added

            MexLine2d::segmentIntersects added.
            26 May 1998             Bob Archer

    01.73   MexLine2d::isPointOnLine now checks for points being within
            epsilon of the line.
            2 June 1998             Bob Archer

    01.74   MexVec3::parallel added.
            MexTransform3d::MexTransform3d(
              Axis axis,
              const MexVec3& axisDirection,
              Plane plane,
              const MexVec3& planeDefiningVector,
              const MexPoint3d& position ) added
            MexVec3 crossProduct( const MexVec3& ) const removed
            static MexVec3 crossProduct( const MexVec3&, const MexVec3& ) added
            3 June 1998             Bob Archer

    01.75   bool operator <( const MexVec3&, const MexVec3& ) added
            bool operator <( const MexPoint3d&, const MexPoint3d& ) added

    01.76   void MexTransform3d::rotation( const MexTransform3d& tx ) added
            8 June 1998             Bob Archer

    01.77   ostream& operator<<(ostream& os, const MexLine3d& line) added
            9 June 1998             Bob Archer

    01.78   MATHEX_SCALAR   MexPlane3d::x added
            MATHEX_SCALAR   MexPlane3d::y added
            MATHEX_SCALAR   MexPlane3d::z added
            MATHEX_SCALAR   MexLine2d::length() const added
            MexVec2         MexLine2d::unitDirectionVector() const  added
            MexPoint2d      MexLine2d::pointAtDistance( MATHEX_SCALAR distance ) const added

            10 June 1998            Bob Archer

    01.79   Removed silly method MexVec3::intersects( const MexVec3& ).
            Various intersect methods optionally distinguish touching from penetration .
            MexConvexPolygon2d ctor added to create a square by expanding from a point.
            MexSausage2d implementation supports zero length sausage.
            MexTransform3d  eliminated storage for redundant 4 matrix values.
            MexAlignedBox3d change to simple non-virtual class.
            MexUnalignedBox3d, MexBoundary3d, MexHalfSpace3d classes removed.
            11 June 1998            Jon Green

    01.80   Functions added:
                bool MexCircle2d::intersects( const MexPolygon2d& polygon,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

                void MexCircle2d::boundary( MexAlignedBox2d* pBox ) const;

            14 July 1998            Bob Archer

    01.81   MexBasicRandom added
            30 July 1998            Bob Archer

    01.82   MexUtility::quadraticRealRoots changed to take better account of
            the boundary case where a single root is very close to but not
            quite at zero.
            10 August 1998          Bob Archer

    01.83   MexEpsilon::sqrInstance() added
            17 August 1998          Bob Archer

    01.84   MexVec3::signedAngleBetween added
            17 August 1998          Bob Archer

    01.85   Added a check for a zero length vector in MexVec2::makeUnitVector
            24 August 1998

    01.86   Extra debug PRE_INFO and POST_INFO statements added to
            MexUtility::quadraticRealRoots.
            3 September 1998        Bob Archer

    01.87   MexEpsilon::clampTo and MexEpsilon::isWithinEpsilonOf added
            3 September 1998        Bob Archer

    01.88   Check for a zero length vector in MexVec2::makeUnitVector now checks
            for a genuinely zero length vector rather than a vector whose length
            is less than MexEpsilon.
            3 September 1998        Bob Archer

    01.89   MexAlignedBox3d::MexAlignedBox3d( const MexAlignedBox3d& rhs, const MexTransform3d& t )
            added.
            18 September 1998       Bob Archer

    01.90   Added seed_ member and get function to MexBasicRandom.
            Changed the mexRandom calls to take a pointer to an RNG.
            9 October 1998          Bob Archer

    01.91   Functions that are now recorded:
                MexBasicRandom::seedFromTime
                MexBasicRandom::constructSeededFromTime
            12 October 1998         Bob Archer

    01.92   MexVec2::rotate added
            MexVec2::operator -=( const MexVec2& ) added
            MexVec2::operator *=( MATHEX_SCALAR ) added
            MexVec2::operator /=( MATHEX_SCALAR ) added
            13 November 1998        Bob Archer

    01.93   void MexTransform3d::transform( MexAlignedBox2d* pBox ) const added
            6 January 1998          Bob Archer
*/

#define	LIBRARY	"MEX"
#define	VERSION	"01.93"

#pragma off(unreferenced);

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

#pragma on(unreferenced);

void printVersion()
{
}

/* End MEXVER.CPP ***************************************************/
