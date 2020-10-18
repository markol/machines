/*
 * M E M F N . I P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#include <limits.h>

//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////

_CODE_INLINE
void * operator new( size_t, void *p )
{
	return p;
}

//////////////////////////////////////////////////////////////////////

// _CODE_INLINE
// void destroy( char * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( unsigned char * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( short * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( unsigned short * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( int * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( unsigned int * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( long * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( unsigned long * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( float * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( double * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE
// void destroy( char * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned char * * ) 
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( short * * ) 
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned short * * ) 
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( int * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned int * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( long * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned long * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( float * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( double * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( char *, char * ) 
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned char *, unsigned char * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( short *, short * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned short *, unsigned short * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( int *, int * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned int *, unsigned int * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( long *, long * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned long *, unsigned long * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( float *, float * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( double *, double * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( char * *, char * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned char * *, unsigned char * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( short * *, short * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned short * *, unsigned short * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( int * *, int * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned int * *, unsigned int * * ) 
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( long * *, long * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( unsigned long * *, unsigned long * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( float * *, float * * )
// {
// 	/* Intentionally Empty	*/
// }
// 
// _CODE_INLINE 
// void destroy( double * *, double * * )
// {
// 	/* Intentionally Empty	*/
// }

//////////////////////////////////////////////////////////////////////

/* End MEMFN.IPP ***************************************************/
