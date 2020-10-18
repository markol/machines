/*
 * E U L E R A N G . H P P
 * (c) Charybdis Limited, 1995 - 1997. All Rights Reserved
 */

/*
    MexEulerAngles

    Represents a rotation as a set of euler angles - azimuth,
    elevation and roll. Azimuth is a rotation about
    the Z axis, elevation is about the Y axis, roll is about the X axis.
    All three rotations are positive in a clockwise direction when looking
    at the origin from a point on the positive axis in question.
    Azimuth is performed first, then elevation, then roll.
*/
 
#ifndef _MEX_EULER_HPP
#define _MEX_EULER_HPP

#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/radians.hpp"

//////////////////////////////////////////////////////////////////////

class MexEulerAngles
{
public:

    MexEulerAngles( const MexRadians& azimuth=0, const MexRadians& elevation=0, const MexRadians& roll=0 );
    MexEulerAngles( const MexEulerAngles& );

    ~MexEulerAngles();

    const MexEulerAngles& operator =( const MexEulerAngles& );

    bool operator ==( const MexEulerAngles& ) const;

    const MexRadians&   azimuth( void ) const;
    const MexRadians&   elevation( void ) const;
    const MexRadians&   roll( void ) const;

    void    azimuth( const MexRadians& );
    void    elevation( const MexRadians& );
    void    roll( const MexRadians& );

    static const MexEulerAngles& zeroAngles();
    friend ostream& operator <<( ostream& o, const MexEulerAngles& t );
    
    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexEulerAngles );
    PER_FRIEND_READ_WRITE( MexEulerAngles );
    
private:

    MexRadians azimuth_;
    MexRadians elevation_;
    MexRadians roll_;
}; 

PER_DECLARE_PERSISTENT( MexEulerAngles );

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/eulerang.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif  /* #ifndef _MEX_EULER_HPP */

/* End EULER.HPP *****************************************************/
