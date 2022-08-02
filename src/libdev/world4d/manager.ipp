/*
 * M A N A G E R . I P P 
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
void W4dManager::time( const PhysAbsoluteTime& newTime )
{
	currentTime_ = newTime;
    generateGeneralPositionId();
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysAbsoluteTime& W4dManager::time( void ) const
{
	return (hasArtificialTime_ ? artificialTime_ : currentTime_);
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void W4dManager::sceneManager( W4dSceneManager* pSceneManager )
{
	PRE( pSceneManager != NULL );

	pSceneManager_ = pSceneManager;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
W4dSceneManager* W4dManager::sceneManager( void ) const
{
	PRE( pSceneManager_ != NULL );

	return pSceneManager_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool W4dManager::hasSceneManager( void ) const
{
	return pSceneManager_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
ulong W4dManager::generateGeneralPositionId()
{
        return ++generalPositionId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
ulong W4dManager::generalPositionId() const
{
        return generalPositionId_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void W4dManager::artificialTime( const PhysAbsoluteTime& newTime )
{
    hasArtificialTime_ = true;
    artificialTime_ = newTime;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void W4dManager::clearArtificialTime()
{
    hasArtificialTime_ = false;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool W4dManager::hasArtificialTime() const
{
    return hasArtificialTime_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void W4dManager::clearSceneManager()
{
	pSceneManager_ = NULL;
}

/* End MANAGER.IPP **************************************************/
