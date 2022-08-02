/*
    Simple Template class
*/

#ifndef _PER_CLASS5_HPP
#define _PER_CLASS5_HPP

#include "base/persist.hpp"
#include "pertest/persist.hpp"

template< class T >
class Class5
{
public:
	Class5();
    Class5( PerIstream& istr );
	virtual ~Class5();

	T		i_;
	double	d_;

    friend ostream& operator <<( ostream&, const Class5< T >& );
    friend bool operator ==( const Class5< T >& a, const Class5< T >& b );
    
	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class5 );

private:
};

PER_READ_WRITE_T1( Class5 );

template< class T >
Class5< T >::Class5()
{
}

template< class T >
Class5< T >::Class5( PerIstream& istr )
{
    istr >> *this;
}

template< class T >
Class5< T >::~Class5()
{
}

template< class T >
ostream& operator<<( ostream& ostr, const Class5< T >& t )
{
	ostr << t.i_ << " " << t.d_ << endl;

	return ostr;
}

template< class T >
void perWrite( PerOstream& str, const Class5< T >& ob )
{
	str << ob.i_;
	str << ob.d_;
}

template< class T >
void perRead( PerIstream& str, Class5< T >& ob )
{
	str >> ob.i_;
	str >> ob.d_;
}

template< class T >
bool operator ==( const Class5< T >& a, const Class5< T >& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.i_, b.i_ );
    COMPARE_OBJECTS( a.d_, b.d_ );

    return result;
}

PER_DECLARE_PERSISTENT_T1( Class5 );

#endif
