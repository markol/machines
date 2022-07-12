/*
 * C I R C L E 2 D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexCircle2d

    Represents a circle in 2d space
*/

#ifndef _MATHEX_CIRCLE2D_HPP
#define _MATHEX_CIRCLE2D_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

class MexPolygon2d;
class MexAlignedBox2d;

//Bitwise canonical
class MexCircle2d
{
public:
    //ctor. Define centre and radius
    MexCircle2d( const MexPoint2d& centre, MATHEX_SCALAR radius );
    //PRE( radius >= 0.0 )

    //dtor
    ~MexCircle2d();

    //Accessors
    const MexPoint2d& centre() const;
    MATHEX_SCALAR radius() const;

    //Returns aligned bounding box in pBox.
    void boundary( MexAlignedBox2d* pBox ) const;

    //True iff this intersects rhs. Touching alone may optionally be regarded
    //as intersection as defined by rule.
    bool intersects( const MexCircle2d& rhs,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //True iff this intersects polygon. Touching alone may optionally be regarded
    //as intersection as defined by rule.
    //Note that no bounding box test is performed - the client is advised to
    //do this before calling this function.
    bool intersects( const MexPolygon2d& polygon,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MexCircle2d& t );

    //Persistence
    PER_MEMBER_PERSISTENT( MexCircle2d );
    PER_FRIEND_READ_WRITE( MexCircle2d );

private:

    //MexCircle2d( PerConstructor );

    //data members
    MexPoint2d centre_;
    MATHEX_SCALAR radius_;
};

PER_DECLARE_PERSISTENT( MexCircle2d );

#ifdef _INLINE
    #include "mathex/circle2d.ipp"
#endif


#endif

/* End CIRCLE2D.HPP *************************************************/
