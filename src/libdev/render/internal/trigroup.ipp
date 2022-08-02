/*
 * T R I G R O U P . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const RenMaterial& RenIMaterialGroup::material() const
{
	return material_;
}

_CODE_INLINE
void RenIMaterialGroup::material(const RenMaterial& mat)
{
	material_ = mat;
}

/* End TRIGROUP.IPP ****************************************************/
