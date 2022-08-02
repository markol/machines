/*
 * A R T F D A T A . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MATHEX_SCALAR MachPhysArtefactData::obstacleHeight() const
{
	return height_;
}

_CODE_INLINE
const MexConvexPolygon2d& MachPhysArtefactData::obstaclePolygon() const
{
	return *pPolygon_;
}

_CODE_INLINE
int MachPhysArtefactData::subType() const
{
	return subType_;
}

_CODE_INLINE
uint MachPhysArtefactData::resourceStringId() const
{
	return resourceStringId_;
}
/* End ARTFDATA.IPP *************************************************/
