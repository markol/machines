#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
W4dComposite* W4dLink::pComposite( void ) const
{
    return pComposite_;
}


