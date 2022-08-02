/*
 * E X P P O I N T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    ExplosionPoint

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_EXPPOINT_HPP
#define _MACHPHYS_EXPPOINT_HPP

#include "base/base.hpp"

class ExplosionPoint
{
public:
    //  Only required for vector
    ExplosionPoint();

    ExplosionPoint(
        const MexPoint3d&      position,
        MATHEX_SCALAR   maxFireballOffset,
        size_t          minFireballs,
        size_t          maxFireballs,
        MATHEX_SCALAR   minTimeFactor,
        MATHEX_SCALAR   maxTimeFactor,
        MATHEX_SCALAR   size,           //  The approximate size of the fireballs to produce
        MATHEX_SCALAR   depthOffset );
    // PRE( 0.0 <= minTimeFactor and minTimeFactor <= 1.0 );
    // PRE( 0.0 <= maxTimeFactor and maxTimeFactor <= 1.0 );

    ~ExplosionPoint();

    const MexPoint3d& position() const;

    //  How far from the given position can we place fireballs?
    MATHEX_SCALAR   maxFireballOffset() const;

    size_t minFireballs() const;
    size_t maxFireballs() const;

    //  As a proportion of the total explosion time
    MATHEX_SCALAR   minTimeFactor() const;
    // POST( 0.0 <= result and result <= 1.0 );

    //  As a proportion of the total explosion time
    MATHEX_SCALAR   maxTimeFactor() const;
    // POST( 0.0 <= result and result <= 1.0 );

    MATHEX_SCALAR   size() const;
    MATHEX_SCALAR   depthOffset() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const ExplosionPoint& t );

private:
    // Operation deliberately revoked
    bool operator ==( const ExplosionPoint& );

    //  Default assignment and copy constructor sufficient
    // ExplosionPoint& operator =( const ExplosionPoint& );
    // ExplosionPoint( const ExplosionPoint& );

    MexPoint3d      position_;
    MATHEX_SCALAR   maxFireballOffset_;
    size_t          minFireballs_;
    size_t          maxFireballs_;
    MATHEX_SCALAR   minTimeFactor_; //  As a proportion of the total explosion time
    MATHEX_SCALAR   maxTimeFactor_; //  As a proportion of the total explosion time
    MATHEX_SCALAR   size_;
    MATHEX_SCALAR   depthOffset_;
};


#endif

/* End EXPPOINT.HPP *************************************************/
