/*
 * C A M S H A K E . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const MexPoint3d& W4dCameraShake::epicCentre() const
{
	return 	epicCentre_;
}

_CODE_INLINE
const W4dCameraShake::QuakeScale& W4dCameraShake::scale() const
{
	return scale_;
}

_CODE_INLINE
const PhysAbsoluteTime& W4dCameraShake::startTime() const
{
	return startTime_;
}

_CODE_INLINE
const PhysRelativeTime& W4dCameraShake::duration() const
{
	return duration_;
}

/* End CAMSHAKE.IPP *************************************************/
