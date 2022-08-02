/*
    Template class
*/

#ifndef _PER_CLASS6_HPP
#define _PER_CLASS6_HPP

#include "base/persist.hpp"
#include "pertest/persist.hpp"

template< class T >
class Class6
{
public:
	Class6();
    Class6( PerIstream& istr );
	virtual ~Class6();

	T		i6_;
	double	d6_;

    friend bool operator ==( const Class6< T >& a, const Class6< T >& b );

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class6 );

private:

};

PER_READ_WRITE_T1( Class6 );

template< class T >
Class6< T >::Class6()
: i6_( randomInt() ),
  d6_( randomDouble() )
{
}

template< class T >
Class6< T >::Class6( PerIstream& istr )
{
    istr >> *this;
}

template< class T >
Class6< T >::~Class6()
{
}

template< class T >
ostream& operator<<( ostream& ostr, const Class6< T >& t )
{
	ostr << t.i6_ << " " << t.d6_ << endl;

	return ostr;
}

template< class T >
void perWrite( PerOstream& str, const Class6< T >& ob )
{
	str << ob.i6_;
	str << ob.d6_;
}

template< class T >
void perRead( PerIstream& str, Class6< T >& ob )
{
	str >> ob.i6_;
	str >> ob.d6_;
}

template< class T >
bool operator ==( const Class6< T >& a, const Class6< T >& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.i6_, b.i6_ );
    COMPARE_OBJECTS( a.d6_, b.d6_ );

    return result;
}

PER_DECLARE_PERSISTENT_T1( Class6 );

#endif


