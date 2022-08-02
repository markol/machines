#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool W4dEntityImpl::hasAnimationData() const
{
    return pAnimationDataPtrs_ != NULL || pAnimationLightDataPtrs_ != NULL;
}

_CODE_INLINE
bool W4dEntityImpl::hasMesh( ) const
{
    return hasMesh_;
}

_CODE_INLINE
void W4dEntityImpl::passId( ulong id )
{
    passId_ = id;
}

_CODE_INLINE
ulong W4dEntityImpl::passId( void ) const
{
    return passId_;
}

_CODE_INLINE
bool W4dEntityImpl::hasParent( void ) const
{
    return pParent_ != NULL;
}

_CODE_INLINE
W4dEntity* W4dEntityImpl::pParent() const
{

    PRE( hasParent() );
    
    W4dEntity* result = pParent_;

    POST( result != NULL );

    return result;
}

_CODE_INLINE
bool W4dEntityImpl::hasLightList() const
{
    return pLocalLightList_!=NULL;
}

_CODE_INLINE
void W4dEntityImpl::lightList(W4dLocalLightList* pList)
{
        pLocalLightList_ = pList;
}

_CODE_INLINE
const W4dLocalLightList& W4dEntityImpl::lightList() const
{
        PRE(hasLightList());
        return *pLocalLightList_;
}

_CODE_INLINE
void W4dEntityImpl::isComposite( bool isIt )
{
    isComposite_ = isIt;
}

_CODE_INLINE
bool W4dEntityImpl::isComposite() const
{
    return isComposite_;
}

_CODE_INLINE
void W4dEntityImpl::doNotLight(bool lightIt)
{
	doNotLight_ = lightIt;
}

_CODE_INLINE
bool W4dEntityImpl::doNotLight() const
{
        return doNotLight_;
}

_CODE_INLINE
void W4dEntityImpl::clientData( int newValue )
{
	clientData_ = newValue;
}

_CODE_INLINE
int W4dEntityImpl::clientData() const
{
	return clientData_;
}


