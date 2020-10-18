/*
 * S A U S A G 2 D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexSausage2d

    Represents a shape defined by sweeping a circle of known radius along the
    straight line between 2 points.
*/

#ifndef _MATHEX_SAUSAG2D_HPP
#define _MATHEX_SAUSAG2D_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

//forward declarations
class MexPolygon2d;
class MexAlignedBox2d;
class MexCircle2d;

//Bitwise canonical
class MexSausage2d
{
public:
    //ctor. Sweep circle with specified radius from startPoint to endPoint.
    MexSausage2d( const MexPoint2d& startPoint, const MexPoint2d& endPoint,
                  MATHEX_SCALAR radius );

    //dtor.
    ~MexSausage2d();

    //Access methods
    const MexPoint2d& startPoint() const;
    const MexPoint2d& endPoint() const;
    MATHEX_SCALAR radius() const;
    MATHEX_SCALAR length() const; //Distance from startpoint to end point

    //Returns aligned bounding box in pBox.
    void boundary( MexAlignedBox2d* pBox ) const;

    //True iff this intersects polygon. Touching alone may optionally be regarded
    //as intersection as defined by rule.
    //Note that no bounding box test is performed - the client is advised to
    //do this before calling this function.
    bool intersects( const MexPolygon2d& polygon,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //True iff this intersects rhs. Touching alone may optionally be regarded
    //as intersection as defined by rule.
    //Note that no bounding box test is performed - the client is advised to
    //do this before calling this function.
    bool intersects( const MexSausage2d& rhs,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //True iff this intersects circle. Touching alone may optionally be regarded
    //as intersection as defined by rule.
    //Note that no bounding box test is performed - the client is advised to
    //do this before calling this function.
    bool intersects( const MexCircle2d& circle,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MexSausage2d& t );

    //Persistence
    PER_MEMBER_PERSISTENT( MexSausage2d );
    PER_FRIEND_READ_WRITE( MexSausage2d );

private:

    //MexSausage2d( PerConstructor );

    //data members
    MexPoint2d startPoint_; //First line point
    MexPoint2d endPoint_; //Second line point
    MATHEX_SCALAR radius_; //Radius of the circle
    MATHEX_SCALAR length_; //Caches distance between the 2 points
};

PER_DECLARE_PERSISTENT( MexSausage2d );

#ifdef _INLINE
    #include "mathex/sausag2d.ipp"
#endif


#endif

/* End SAUSAG2D.HPP *************************************************/
