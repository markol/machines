/*
 * C O N S T R A I . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _UTL_CONSTRAI_HPP
#define _UTL_CONSTRAI_HPP

#include "base/base.hpp"

//////////////////////////////////////////////////////////////////////

template < class T >
class UtlTypeBase
// memberwise cannonical according as T
{
public:

	UtlTypeBase( const T& t )
	: value_( t )
	{
		// Intentionally Empty
	}

protected:

	T	value_;
};

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

template < class PREDICATE >
class UtlConstrainedType
: private UtlTypeBase< typename PREDICATE::argument_type >
// memberwise cannonical according as PREDICATE::argument_type
{
public:

	typedef typename PREDICATE::argument_type T;

	UtlConstrainedType( const T& t )
	: UtlTypeBase< T >::UtlTypeBase( t )
	{
		PRE_INFO( t );
		PRE( isValid( t ) );
	}

	operator const T&() const
	{
		PRE_INFO( UtlTypeBase< T >::value_ );
		PRE( isValid( UtlTypeBase< T >::value_ ) );
		return UtlTypeBase< T >::value_;
	}

	bool isValid( const T& t ) const
	{
		return PREDICATE( t );
	}

	UtlConstrainedType& operator =( const T& t )
	{
		PRE_INFO( t );
		PRE( isValid( t ) );
		UtlTypeBase< T >::value_ = t;
		return *this;
	}
};

#endif // #ifndef NDEBUG

#ifndef NDEBUG
	#define UTL_CONSTRAINED_TYPE( TYPE, PREDICATE ) UtlConstrainedType< PREDICATE >
#else
	#define UTL_CONSTRAINED_TYPE( TYPE, PREDICATE ) TYPE
#endif	// #ifndef NDEBUG

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

template < class T >
class UtlSubclassableConstrainedType
: private UtlTypeBase< T >
// memberwise cannonical according as T
{
public:

	typedef UtlSubclassableConstrainedType ConstrainedTypeBase;

	virtual ~UtlSubclassableConstrainedType() {}

	operator const T&() const
	{
		PRE_INFO( UtlTypeBase< T >::value_ );
		PRE( isValid( UtlTypeBase< T >::value_ ) );
		return UtlTypeBase< T >::value_;
	}

	bool isValid( const T& t ) const
	{
		return fn_( t );
	}

protected:

	typedef bool (*b_fn_T)( const T& );

	UtlSubclassableConstrainedType( const T& t, b_fn_T fn )
	: UtlTypeBase< T >::UtlTypeBase( t ),
	  fn_( fn )
	{
		PRE_INFO( t );
		PRE( isValid( t ) );
	}

private:

	b_fn_T	fn_;
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************

// example client code:

#include <ctype.h>
#include <string.hpp>

class UtlIdentifier
: public UtlSubclassableConstrainedType< string >
{
public:

	UtlIdentifier( const string& s )
	: ConstrainedTypeBase( s, isIdentifier )
	{
	}

	static bool isIdentifier( const string& s )
	{
		bool result = s.length() >= 1 and isalpha( s[ 0 ] );
		for( size_t i=1; i<s.length() and result; ++i )
			result = isalnum( s[ i ] );

		return result;
	}
};

//////////////////////////////////////////////////////////////////////

#include <function.hpp>

class isEven
{
public:

	typedef int argument_type;

	isEven( const int& i )
	: value_( i ) {}

	operator bool() const
	{
		return ( value_ % 2 ) == 0;
	}

private:

	int	value_;
};

//////////////////////////////////////////////////////////////////////

int main()
{
	cout << "1" << endl;
	UtlIdentifier id1( "a" );
	cout << "2" << endl;
	UTL_CONSTRAINED_TYPE( int, isEven ) i = 2;
	cout << "3" << endl;
//	UTL_CONSTRAINED_TYPE( int, isEven ) j = 3;
	UtlIdentifier id( "a:" );
	cout << "4" << endl;
	return 0;
}

*********************************************************************/

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _UTL_SUBTYPE_HPP	*/

/* End SUBTYPE.HPP **************************************************/
