/*
 * M E X I D A T A . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif


_CODE_INLINE
MATHEX_SCALAR   MachPhysIMachineExplosionData::maxSize( void ) const
{
    return maxSize_;
}


_CODE_INLINE
size_t  MachPhysIMachineExplosionData::maxToShootOff( void ) const
{
    return maxToShootOff_;
}

_CODE_INLINE
void  MachPhysIMachineExplosionData::maxToShootOff( size_t newMax )
{
    maxToShootOff_=newMax;
}

_CODE_INLINE
const MexPoint3d& MachPhysIMachineExplosionData::explosionCenter( void ) const
{
  return explosionCenter_;
}

_CODE_INLINE
void MachPhysIMachineExplosionData::explosionCenter( const MexPoint3d& newExplosionCenter )
{
  explosionCenter_=newExplosionCenter;
}

_CODE_INLINE
size_t  MachPhysIMachineExplosionData::minToShootOff( void ) const
{
    return minToShootOff_;
}

_CODE_INLINE
void    MachPhysIMachineExplosionData::minToShootOff( size_t newMin )
{
    minToShootOff_ = newMin;
}



/* End MACHDATI.IPP *************************************************/


