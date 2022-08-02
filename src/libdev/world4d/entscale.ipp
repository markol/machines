/*
 * E N T S C A L E . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool W4dEntityScale::hasMeshScale() const
{
    return pScale_ != NULL;
}

_CODE_INLINE
const RenScale& W4dEntityScale::meshScale() const
{
    return *pScale_;
}

_CODE_INLINE
bool W4dEntityScale::scaleTransformFromParent() const
{
    return pUnscaledTransformPosition_ != NULL;
}

_CODE_INLINE
const MexPoint3d& W4dEntityScale::unscaledTransformPosition() const
{
    PRE( scaleTransformFromParent() );

    return *pUnscaledTransformPosition_;
}

/* End ENTSCALE.IPP *************************************************/
