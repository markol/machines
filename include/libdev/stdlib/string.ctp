/*
 * S T R I N G . C T P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 *
 * MCR 26/3/96 added < operator (x3), changed inline -> _CODE_INLINE
 */

#include <stdlib.h>
#include <iostream.h>

#include "base/base.hpp"
#include "base/cballoc.hpp"

#ifndef _INLINE
    #include <string.itp>
#endif

//////////////////////////////////////////////////////////////////////

template< class T >
size_t ntcs_length( T *str )
{
    size_t i;
    
    for( i=0; str[ i ]; ++i )
	    ;
		       
    return i;            
}

//////////////////////////////////////////////////////////////////////

template< class T >
basic_string< T >::basic_string( size_t _N, capacity _C )
{
    _Tidy( false );
    _Res = _N;
    if( _C == default_size )
	assign( '\0', _N );
}
   
template< class T >
basic_string< T >& 
basic_string< T >::append( const basic_string< T >& str, size_t pos, size_t ns )
{
    if( str.length() < pos )
	_Xran();
	
    size_t n = str.length() - pos;
    
    if( n < ns )
	ns = n;
	
    if( NPOS - _Len <= ns )
	_Xlen();
	
    // append to make non-empty string        
    if( 0 < ns && _Grow( n = _Len + ns, false ) )
    {
	memcpy( _Ptr + _Len, &str.data()[ pos ], ns * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

template< class T >
basic_string< T >& 
basic_string< T >::append( const T *s, size_t ns )
{
    if( ns == NPOS )
	ns = ntcs_length( s );
	
    if( NPOS - _Len <= ns )
	_Xlen();
	
    size_t n;
    
    // append to make non-empty string        
    if( 0 < ns && _Grow( n = _Len + ns, false ) )
    {
	memcpy( _Ptr + _Len, s, ns * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;    
}

template< class T >
basic_string< T >& 
basic_string< T >::append( T ch, size_t nr )
{
    if( NPOS - _Len <= nr )
	_Xlen();
	
    size_t n;
    // append to make non-empty string
    if( 0 < nr && _Grow( n = _Len + nr, false ) )
    {
	memset( _Ptr + _Len, ch, nr * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;                
}

//////////////////////////////////////////////////////////////////////

template< class T >
basic_string< T >& 
basic_string< T >::assign( const basic_string< T >& str, size_t pos, size_t ns )
{
    if( str.length() < pos )
	_Xran();
	
    size_t n = str.length() - pos;
    
    if( ns < n )
	n = ns;
	
    if( this == &str )
    {
	remove( pos + n );
	remove( 0, pos );
    }
    else if( _Grow( n, 1 ) )
    {
	    memcpy( _Ptr, &str.data()[ pos ], n * sizeof( T ) );
	    _Ptr[ _Len = n ] = '\0';
	}
    
    return *this;
}

template< class T >
basic_string< T >& 
basic_string< T >::assign( const T *s, size_t n )
{
    if( n == NPOS )
	n = ::strlen( s );
	
    if( _Grow( n, 1 ) )
    {
	memcpy( _Ptr, s, n * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';    
    }
    
    return *this;
}

template< class T >
basic_string< T >& 
basic_string< T >::assign( T ch, size_t n )
{
    if( n == NPOS )
	_Xlen();
	
    if( _Grow( n, 1 ) )
    {
	memset( _Ptr, ch, n * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

//////////////////////////////////////////////////////////////////////

template< class T >
basic_string< T >& 
basic_string< T >::insert( size_t p0,
			    const basic_string< T >& str, 
			    size_t pos, size_t ns )
{
    if( _Len < p0 || str.length() < pos )
	_Xran();
	
    size_t n = str.length() - pos;        
    if( n < ns )
	ns = n;
	
    if( NPOS - _Len <= ns )
	_Xlen();
	
    // insert to make non-empty string                
    if( 0 < ns && _Grow( n = _Len + ns, false ) )
    {
	memmove( _Ptr + p0 + ns, _Ptr + p0, ( _Len - p0 ) * sizeof( T ) );    
	memcpy( _Ptr + p0, &str.data()[ pos ], ns  * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

template< class T >
basic_string< T >& basic_string< T >::insert( size_t p0, const T *s, size_t ns )
{
    if( _Len < p0 )
	_Xran();
	
    if( ns == NPOS )
	ns = ntcs_length( s );
	
    if( NPOS - _Len <= ns )
	_Xlen();

    size_t n;        
    // insert to make non-empty string                
    if( 0 < ns && _Grow( n = _Len + ns, false ) )
    {
	memmove( _Ptr + p0 + ns, _Ptr + p0, ( _Len - p0 ) * sizeof( T ) );    
	memcpy( _Ptr + p0, s, ns * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

template< class T >
basic_string< T >& basic_string< T >::insert( size_t p0, T ch, size_t nr )
{
    if( _Len < p0 )
	_Xran();
	    
    if( NPOS - _Len <= nr )
	_Xlen();

    size_t n;        
    // insert to make non-empty string        
    if( 0 < nr && _Grow( n = _Len + nr, false ) )
    {
	memmove( _Ptr + p0 + nr, _Ptr + p0, ( _Len - p0 ) * sizeof( T ) );    
	memset( _Ptr + p0, ch, nr * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

//////////////////////////////////////////////////////////////////////

template< class T >
basic_string< T >& basic_string< T >::remove( size_t p0, size_t nr )
{
    if( _Len < p0 )
	_Xran();
	    
    if( _Len - p0 < nr )
	nr = _Len - p0;
	
    // remove the substring
    if( 0 < nr ) 
    {
	memmove( _Ptr + p0, _Ptr + p0 + nr, ( _Len - p0 - nr ) * sizeof( T ) );
	size_t n = _Len - nr;

	if( _Grow( n, false ) )
	    _Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

//////////////////////////////////////////////////////////////////////

template< class T >
basic_string< T >&
basic_string< T >::replace( size_t p0, size_t n0,
				const basic_string< T >& str, size_t pos, size_t ns )
{
    if( _Len < p0 || str.length() < pos )
	_Xran();
	
    size_t n = str.length() - pos;        
    if( n < ns )
	ns = n;
	
    if( NPOS - ns <= _Len - n0 )
	_Xlen();
	
    size_t nm = _Len - n0 - p0;
    
    if( ns < n0 )
	memmove( _Ptr + p0 + ns, _Ptr + p0 + n0, nm * sizeof( T ) );

    // replace to make non-empty string        
    if( ( 0 < ns || 0 < n0 ) && _Grow( n = _Len + ns - n0, false ) )
    {
	if( n0 < ns )
	    memmove( _Ptr + p0 + ns, _Ptr + p0 + n0, nm * sizeof( T ) );    
	    
	memcpy( _Ptr + p0, &str.data()[ pos ], ns * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

template< class T >
basic_string< T >&
basic_string< T >::replace( size_t p0, size_t n0, const T *s, size_t ns )
{
    if( _Len < p0 )
	_Xran();

    if( ns == NPOS )
	ns = strlen( s );
	
    if( NPOS - ns <= _Len - n0 )
	_Xlen();
	
    size_t nm = _Len - n0 - p0;
    
    if( ns < n0 )
	memmove( _Ptr + p0 + ns, _Ptr + p0 + n0, nm * sizeof( T ) );

    size_t n;
    // replace to make non-empty string        
    if( ( 0 < ns || 0 < n0 ) && _Grow( n = _Len + ns - n0, false ) )
    {
	if( n0 < ns )
	    memmove( _Ptr + p0 + ns, _Ptr + p0 + n0, nm * sizeof( T ) );
	    
	memcpy( _Ptr + p0, s, ns * sizeof( T ) );
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

template< class T >
basic_string< T >&
basic_string< T >::replace( size_t p0, size_t n0, T ch, size_t nr )
{
    if( _Len < p0 )
	_Xran();

    if( _Len - p0 < n0 )
	n0 = _Len - p0;
	
    if( NPOS - nr <= _Len - n0 )
	_Xlen();
	
    size_t nm = _Len - n0 - p0;
    
    if( nr < n0 )
	memmove( _Ptr + p0 + nr, _Ptr + p0 + n0, nm * sizeof( T ) );

    size_t n;
    // replace to make non-empty string
    if( ( 0 < nr || 0 < n0 ) && _Grow( n = _Len + nr - n0, false ) )
    {
	if( n0 < nr )
	    memmove( _Ptr + p0 + nr, _Ptr + p0 + n0, nm * sizeof( T ) );
	    
	memset( _Ptr + p0, ch, nr * sizeof( T ) );            
	_Ptr[ _Len = n ] = '\0';
    }
    
    return *this;
}

//////////////////////////////////////////////////////////////////////

template< class T >
T basic_string< T >::get_at( size_t p0 ) const
{
    if( _Len <= p0 )
	_Xran();
	
    return _Ptr[ p0 ];        
}

template< class T >
void basic_string< T >::put_at( size_t p0, T ch )
{
    if( _Len <= p0 )
	_Xran();
    else if( _Len == p0 )
	append( ch );
    else               
	_Ptr[ p0 ] = ch;        
}

//////////////////////////////////////////////////////////////////////

template< class T >
void basic_string< T >::resize( size_t _N, T _C )
{
    if( _N <= _Len )
	remove( _N );
    else
	append( _C, _N - _Len );        
}

//////////////////////////////////////////////////////////////////////

template< class T >
size_t basic_string< T >::copy( T *s, size_t n, size_t p0 )
{
    if( _Len < p0 )
	_Xran();

    if( _Len - p0 < n )
	n = _Len - p0;
	
    memcpy( s, _Ptr + p0, n * sizeof( T ) );
    
    return n;     
}

//////////////////////////////////////////////////////////////////////

// size_t find( const basic_string& _X, size_t _P );
// ...
// basic_string substr( size_t _P = 0, size_t _N = NPOS ) const;

//////////////////////////////////////////////////////////////////////

template< class T >
int basic_string< T >::compare( const basic_string< T >& str,
				    size_t p0, size_t ns ) const
{
    if( _Len < p0 )
	_Xran();
	
    size_t n = _Len - p0;
    
    if( str.length() < ns )
	ns = str.length();
	
    size_t nlwr = n < ns ? n : ns;
    size_t ans = memcmp( _Ptr + p0, str.data(), nlwr * sizeof( T ) );
    
    if( ans != 0 )
	return ans;
	
    if( n < ns )
	return -1;
    else if( n == ns )
	return 0;
	
    return 1;                        
}

template< class T >
int basic_string< T >::compare( const T *s, size_t p0, size_t ns ) const
{
    if( _Len < p0 )
	_Xran();
	
    size_t n = _Len - p0;

    if( ns == NPOS )
	ns = ::strlen( s );
	       
    size_t nlwr = n < ns ? n : ns;
    size_t ans = memcmp( _Ptr + p0, s, nlwr * sizeof( T ) );
    
    if( ans != 0 )
	return ans;
	
    if( n < ns )
	return -1;
    else if( n == ns )
	return 0;
	
    return 1;                        
}

template< class T >
int basic_string< T >::compare( T ch, size_t p0, size_t ns ) const
{
    if( _Len < p0 )
	_Xran();
	
    size_t n = _Len - p0;
    size_t nlwr = n < ns ? n : ns;    
    for( const T *s = _Ptr + p0, *t = s + nlwr; s<t; ++s )
    {
	if( *s != ch )
	{
	    if( *(T *)s < (T)ch )
		return -1;
	    else
		return 1;
	}
    }                                        

    if( n < ns )
	return -1;

    if( n == ns )
	return 0;
	
    return 1;                               
}

//////////////////////////////////////////////////////////////////////

template< class T >
bool basic_string< T >::_Grow( size_t n, bool trim )
{
    size_t osize = _Ptr == 0 ? 0 : _Res;
    
    if( n == 0 )
    {
	if( trim && _GROW_MIN_SIZE < osize )
	    _Tidy( 1 );
	else if( _Ptr != 0 )
	    _Ptr[ _Len = 0 ] = T( '\0' );

	return false;                       
    }
    else if( n == osize || n < osize && !trim )
	return true;

    size_t size = _Ptr == 0 && n < _Res ? _Res : n;
    if( ( size |= _GROW_MIN_SIZE ) == NPOS )
	--size;

    char *s;
    if( ( s = (T *)cbRealloc( _Ptr, ( 1 + size ) * sizeof( T ) ) ) == 0 )
	    if( ( s = (T *)cbRealloc( _Ptr, ( 1 + ( size = n )  ) * sizeof( T ) ) ) == 0 )
	        exit( -1 );// _Nomemory();

    _Ptr = s;
    _Res = size;
    return true;            
}

template< class T >
void basic_string< T >::_Tidy( bool constructed )
{
    if( constructed && _Ptr != 0 )
	cbFree( _Ptr );

    _Ptr = 0;
    _Len = _Res = 0;        
}
    
//////////////////////////////////////////////////////////////////////

// #include "clibplus/assert.hpp"

template< class T >
void basic_string< T >::_Xlen() const
{
	_RAISE( length_error( "string too long" ) );
}

template< class T >
void basic_string< T >::_Xran() const
{
	_RAISE( out_of_range( "invalid string index" ) );
}

template< class T >
basic_string_iter< T >&  basic_string_iter< T >::operator ++( PREFIX )
{
    if( (*strPtr)[ index ] != T( '\0' ) )
	++index;
	
    return *this;
}

template< class T >
basic_string_iter< T > basic_string_iter< T >::operator ++( POSTFIX )
{
    basic_string_iter< T > prev( *this );
    ++( *this );
    return prev;
}


/* End STRING.CTP *************************************************/
