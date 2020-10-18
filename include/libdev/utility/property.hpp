/*
 * P R O P E R T Y . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

/*
    UtlProperty

    Provide simple set and get methods for a member object of a class. E.g.

    class TestClass
    {
    public:
        UtlProperty< int > testId;
    }

    TestClass   t;

    t.testId( 50 );
    cout << t.testId() << endl;

*/

#ifndef _UTL_PROPERTY_HPP
#define _UTL_PROPERTY_HPP

#include "base/persist.hpp"

//////////////////////////////////////////////////////////////////////
template < class T > class UtlProperty;
template < class T >
T& utlGetPropertyReference( UtlProperty< T >& );


template < class T >
class UtlProperty
{
public:

	UtlProperty();
	UtlProperty( const T& );

	// 'get' method
	const T& operator ()( void ) const;

	// 'set' method
	void operator ()( const T& );

    PER_MEMBER_PERSISTENT_DEFAULT( UtlProperty );

    // hmmm
	bool operator !=( const UtlProperty< T >& a)
    {
        return a.property_ != property_;
    }

private:

    //  Compiler generated copy constructor is OK
    //	UtlProperty( const UtlProperty< T >& );

	// revoked...
	UtlProperty< T >& operator =(  const UtlProperty< T >& );

	template<class T1>
	friend bool operator ==( const UtlProperty< T1 >&, const UtlProperty< T1 >& );
	//friend bool operator !=( const UtlProperty< T >&, const UtlProperty< T >& );

    friend T& utlGetPropertyReference< T >( UtlProperty< T >& );

	T	property_;

};

PER_DEFINE_PERSISTENT_INLINE_T1( UtlProperty );

//////////////////////////////////////////////////////////////////////

template < class T >
inline
UtlProperty< T >::UtlProperty()
{
	/* Intentionally Empty	*/
}

template < class T >
inline
UtlProperty< T >::UtlProperty( const T& t )
: property_( t )
{
	/* Intentionally Empty	*/
}

template < class T >
inline
const T& UtlProperty< T >::operator ()( void ) const
{
	return property_;
}

template < class T >
inline
void UtlProperty< T >::operator ()( const T& t )
{
	property_ = t;
}

template < class T >
inline
bool operator ==( const UtlProperty< T >& a, const UtlProperty< T >& b )
{
    return a.property_ == b.property_;
}

template < class T >
inline
bool operator !=( const UtlProperty< T >& a, const UtlProperty< T >& b )
{
    //return !(a == b);
    return a != b;
}

//  This is a proxy function designed to allow perRead
//  to get a reference to the internal propery_ member of
//  UtlProperty with it having to be a friend itself. If I make
//  operator >> a friend of UtlProperty then every class that
//  UtlProperty is instantiated over will have to be persistent
//  and I don't want that.

// friend
template < class T >
inline
T& utlGetPropertyReference( UtlProperty< T >& ob )
{
    return ob.property_;
}

template < class T >
inline
void perWrite( PerOstream& ostr, const UtlProperty< T >& ob )
{
    ostr << ob();
}

template < class T >
inline
void perRead( PerIstream& istr, UtlProperty< T >& ob )
{
    istr >> utlGetPropertyReference< T >( ob );
}


//////////////////////////////////////////////////////////////////////////

#endif  /*	#ifndef _UTL_PROPERTY_HPP	*/
