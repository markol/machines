/*
 * C O N S D A T A . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MexAlignedBox2d& MachPhysConstructionData::localBoundary() const
{
    return localBoundary_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MachPhysConstructionData::Entrances& MachPhysConstructionData::entrances() const
{
    return entrances_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MachPhysConstructionData::Locations& MachPhysConstructionData::pickUpPoints() const
{
    return pickUpPoints_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MachPhysConstructionData::Locations& MachPhysConstructionData::putDownPoints() const
{
    return putDownPoints_;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/* End CONSDATA.IPP *************************************************/
