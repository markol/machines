/*
 * E U L E R A N G . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const MexRadians&   MexEulerAngles::azimuth( void ) const

{
    return azimuth_;
}

_CODE_INLINE
const MexRadians&   MexEulerAngles::elevation( void ) const

{
    return elevation_;
}
 
_CODE_INLINE
const MexRadians&   MexEulerAngles::roll( void ) const
{
    return roll_;
}
 
_CODE_INLINE
void    MexEulerAngles::azimuth( const MexRadians& newAzimuth )
{
    azimuth_ = newAzimuth;
}

_CODE_INLINE
void    MexEulerAngles::elevation( const MexRadians& newElevation )
{
    elevation_ = newElevation;
}

_CODE_INLINE
void    MexEulerAngles::roll( const MexRadians& newRoll )

{
    roll_ = newRoll;
}

// static
_CODE_INLINE
const MexEulerAngles& MexEulerAngles::zeroAngles()
{
    static MexEulerAngles zero_ = MexEulerAngles( 0, 0, 0 );

    return zero_;        
}
