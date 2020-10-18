/*
 * M A C H D A T I . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MachPhys::LocomotionType    MachPhysIMachineData::locomotionType() const
{
    return locomotionType_;
}

_CODE_INLINE
void MachPhysIMachineData::locomotionType( MachPhys::LocomotionType type )
{
    locomotionType_ = type;
}

/* End MACHDATI.IPP *************************************************/
