#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
int compare( int a, int b )
{
	return a < b ? -1 : ( a == b ? 0 : 1 );
}
