/*
 * B A S E . H P P
 * (c) Charybdis Limited, 1995,1996. All Rights Reserved.
 */

#ifndef _BASE_HPP
#define _BASE_HPP

//////////////////////////////////////////////////////////////////////////

#define PREFIX
#define POSTFIX int

#include "base/compiler.hpp"
#include "base/diag.hpp"

//////////////////////////////////////////////////////////////////////

typedef signed char     schar;
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

typedef char*           c_string;

//////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>


// #defines //////////////////////////////////////////////////////////

#define WHERE_STR   __FILE__ << "  " << std::dec << __LINE__
#define WHERE       Diag::instance().whereStream() << WHERE_STR << std::endl

#define INSPECT_ON( ostream, var )   ( ostream << #var " :\t" << (var) << std::endl )

///////////////////////////////////

#define _STR( x )   _VAL( x )
#define _VAL( x )   #x

///////////////////////////////////

const size_t NPOS = (size_t)(-1);

//////////////////////////////////////////////////////////////////////

#define bitsizeof( T ) ( sizeof( (T) ) * CHAR_BIT )

//////////////////////////////////////////////////////////////////////

#include "base/prepost.hpp"

#include "base/memchk.hpp"

//////////////////////////////////////////////////////////////////////////

// extensions to iso646 operators

#define nor( a, b )     ( not ( (a) or (b) ) )
#define nand( a, b )    ( not ( (a) and (b) ) )
//#define xor( a, b )     (  (a) ? !(b) : !!(b) )
#define iff( a, b )     ( ((bool)(a)) == ((bool)(b)) )
#define implies( a, b ) ( ((bool)(a)) ? (bool)(b) : true )

// #defines for documentation purposes
#define _ABSTACT
#define _INSTANCE
#define _LEAF

// array must be statically allocated storage
#define elementsof( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )

//////////////////////////////////////////////////////////////////////

#ifndef NO_EXCEPTIONS

	// exceptions implemented

	#define _TRY_BEGIN      try {
	#define _CATCH_ALL      catch( ... ) {
	#define _CATCH_END      }
	#define _RAISE( x )     throw( x )
	#define _RERAISE( x )   throw

#else

	// exceptions revoked

	#define _TRY_BEGIN      {
	#define _CATCH_ALL      {
	#define _CATCH_END      }
	#define _RAISE( x )
	#define _RERAISE( x )

#endif	/* #ifndef NO_EXCEPTIONS	*/

//////////////////////////////////////////////////////////////////////

#ifndef NO_ENUM_OVERLOAD

	#define _BITMASK( E, T )                    \
	E& operator &=( E& _X, E& _Y )              \
	{                                           \
    	_X = (E)( _X & _Y );                    \
	    return (_X);                            \
	}                                           \
	\
	E& operator |=( E& _X, E& _Y )              \
	{                                           \
	    _X = (E)( _X | _Y );                    \
	    return (_X);                            \
	}                                           \
	\
	E& operator ^=( E& _X, E& _Y )              \
	{                                           \
	    _X = (E)( _X ^ _Y );                    \
	    return (_X);                            \
	}                                           \
	\
	E operator &( E _X, E _Y )                  \
	{                                           \
    	return ((E)(_X & _Y );                  \
	}                                           \
	\
	E  operator |( E _X, E _Y )                 \
	{                                           \
    	return ((E)(_X | _Y );                  \
	}                                           \
	\
	E operator &( E _X, E _Y )                  \
	{                                           \
    	return ((E)(_X & _Y );                  \
	}                                           \
	typedef E T

#endif	/* #ifndef NO_ENUM_OVERLOAD	*/

//////////////////////////////////////////////////////////////////////

#ifndef NO_STANDARD_CASTS

    #define _CONST_CAST( t , o )       (const_cast< t >(o))
    #define _STATIC_CAST( t , o )      (static_cast< t >(o))
    #define _REINTERPRET_CAST( t , o ) (reinterpret_cast< t >(o))

#else

    #define _CONST_CAST( t , o )       ((t)(o))
    #define _STATIC_CAST( t , o )      ((t)(o))
    #define _REINTERPRET_CAST( t , o ) ((t)(o))

#endif	/* #ifndef NO_STANDARD_CASTS */

//////////////////////////////////////////////////////////////////////

#ifndef NO_MUTABLE_KEYWORD

	#define mutable     mutable
    #define _MUTABLE_CAST( t, o )     (o)

#else

    #define mutable
    #define _MUTABLE_CAST( t, o )     _CONST_CAST( t , o )

#endif	/*	#ifndef NO_MUTABLE_KEYWORD	*/

//////////////////////////////////////////////////////////////////////

#ifndef NO_EXPLICIT_KEYWORD

	#define explicit    explicit

#else

    #define explicit

#endif	/*	#ifndef NO_EXPLICIT_KEYWORD	*/

//////////////////////////////////////////////////////////////////////

//#define namespace struct

//////////////////////////////////////////////////////////////////////


#ifdef NO_NEW_FOR_SCOPE

    #ifdef __WATCOMC__

        //  Disable "conditional expression in if statement is always false" warning

        #pragma warning 368 9;

    #endif

    #define for if( 0 ) ; else for

#endif

// Macros make switching between pImpl_ and no pImpl_ very easy.
#define CB_DEPIMPL( vartype, varname ) \
	PRE( pImpl_ ) \
	[[gnu::unused]] vartype& varname = pImpl_->varname;

// If data member is "int var[10]" then pass vartype as int.
#define CB_DEPIMPL_ARRAY( vartype, varname ) \
	PRE( pImpl_ ) \
	[[gnu::unused]] vartype* varname = pImpl_->varname;

#define CB_DEPIMPL_2D_ARRAY( vartype, varname, secondDimension ) \
	PRE( pImpl_ ); \
	[[gnu::unused]] vartype (*varname)[secondDimension] = pImpl_->varname;


//////////////////////////////////////////////////////////////////////

#endif  /*  #ifndef BASE_HPP    */
