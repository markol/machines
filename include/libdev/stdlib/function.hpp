/*
 * F U N C T I O N . H P P
 * (c) Charybdis Limited, 1995 - 1997. All Rights Reserved.
 */

//#include "stdlib/private/ospace.hpp"
//#include "ospace/stl/function.h"

#ifndef _STDLIB_FUNCTION_HPP
#define _STDLIB_FUNCTION_HPP

/*
template< class RESULT, class T >
class mem_fun_t
: public unary_function< T *, RESULT >
{
public:

	typedef RESULT (T::*pmf)();
    mem_fun_t( pmf p )
	: pmf_( p ) {}

	RESULT operator ()( T * pT ) const
	{
		return (pT->*pmf_)();
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class RESULT, class T >
mem_fun_t< RESULT, T >
mem_fun( RESULT (T::*m)() )
{
	return mem_fun_t< RESULT, T >( m );
}

//////////////////////////////////////////////////////////////////////

template< class RESULT, class T, class ARG >
class mem_fun1_t
: public binary_function< T *, ARG, RESULT >
{
public:

	typedef RESULT (T::*pmf)( ARG );
    mem_fun1_t( pmf p )
	: pmf_( p ) {}

    RESULT operator ()( T *pT, ARG arg ) const
	{
		return (pT->*pmf_)( arg );
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class RESULT, class T, class ARG >
mem_fun1_t< RESULT, T, ARG >
mem_fun( RESULT (T::*m)( ARG ) )
{
	return mem_fun1_t< RESULT, T, ARG >( m );
}

/* //////////////////////////////////////////////////////////////// */
/*
template< class RESULT, class T >
class mem_fun_ref_t
: public unary_function< T&, RESULT >
{
public:

	typedef RESULT (T::*pmf)();
    mem_fun_ref_t( pmf p )
	: pmf_( p ) {}

    RESULT operator ()( T& t ) const
	{
		return (t.*pmf_)();
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class RESULT, class T >
mem_fun_ref_t< RESULT, T >
mem_fun_ref( RESULT (T::*m)() )
{
	return mem_fun_ref_t< RESULT, T >( m );
}

/* //////////////////////////////////////////////////////////////// */
/*
template< class RESULT, class T, class ARG >
class mem_fun1_ref_t
: public binary_function< T&, ARG, RESULT >
{
public:

	typedef RESULT (T::*pmf)( ARG );
    mem_fun1_ref_t( pmf p )
	: pmf_( p ) {}

    RESULT operator ()( T& t, ARG arg ) const
	{
		return (t.*pmf_)( arg );
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class RESULT, class T, class ARG >
mem_fun1_ref_t< RESULT, T, ARG >
mem_fun_ref( RESULT (T::*m)( ARG ) )
{
	return mem_fun1_ref_t< RESULT, T, ARG >( m );
}
*/
// =======================================================================

template< class T >
class mem_fun_t_void
: public std::unary_function< T *, void >
{
public:

	typedef void (T::*pmf)();
    mem_fun_t_void( pmf p )
	: pmf_( p ) {}

	void operator ()( T * pT ) const
	{
		(pT->*pmf_)();
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class T >
mem_fun_t_void< T >
mem_fun_void( void (T::*m)() )
{
	return mem_fun_t_void< T >( m );
}

//////////////////////////////////////////////////////////////////////

//template< class T, class ARG  >
template< class T, class ARG >
class mem_fun1_t_void
//: public std::binary_function< T *, ARG, RESULT >
: public std::binary_function< T *, ARG, bool >
{
public:

	typedef void (T::*pmf)( ARG );
    mem_fun1_t_void( pmf p )
	: pmf_( p ) {}

    void operator ()( T *pT, ARG arg ) const
	{
		(pT->*pmf_)( arg );
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class T, class ARG >
mem_fun1_t_void< T, ARG >
mem_fun_void( void (T::*m)( ARG ) )
{
	return mem_fun1_t_void< T, ARG >( m );
}

/* //////////////////////////////////////////////////////////////// */

template< class T >
class mem_fun_ref_t_void
: public std::unary_function< T&, void >
{
public:

	typedef void (T::*pmf)();
    mem_fun_ref_t_void( pmf p )
	: pmf_( p ) {}

    void operator ()( T& t ) const
	{
		(t.*pmf_)();
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class T >
mem_fun_ref_t_void< T >
mem_fun_ref_void( void (T::*m)() )
{
	return mem_fun_ref_t_void< T >( m );
}

/* //////////////////////////////////////////////////////////////// */

template< class T, class ARG >
class mem_fun1_ref_t_void
: public std::binary_function< T&, ARG, void >
{
public:

	typedef void (T::*pmf)( ARG );
    mem_fun1_ref_t_void( pmf p )
	: pmf_( p ) {}

    void operator ()( T& t, ARG arg ) const
	{
		(t.*pmf_)( arg );
	}

private:
	pmf pmf_;
};

///////////////////////////////////

template < class T, class ARG >
mem_fun1_ref_t_void< T, ARG >
mem_fun_ref_void( void (T::*m)( ARG ) )
{
	//return std::mem_fun1_ref_t< T, ARG >( m );
	return std::mem_fun1_ref_t< T, ARG, bool >( m );
}


#endif

/* End FUNCTION.HPP **************************************************/
