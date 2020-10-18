/*
 * A R T E F A C T . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const W4dEntity& MachPhysArtefact::physEntity() const
{
	return *pModel_;
}

_CODE_INLINE
W4dEntity& MachPhysArtefact::physEntity()
{
	return *pModel_;
}
/* End ARTEFACT.IPP *************************************************/
