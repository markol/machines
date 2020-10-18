/*
    Multiply inherited template class
*/

#ifndef _PER_CLASS7_HPP
#define _PER_CLASS7_HPP

#include "base/persist.hpp"

#include "pertest/class5.hpp"
#include "pertest/class6.hpp"

template< class T1, class T2 >
class Class7 : public Class5< T1 >, public Class6< T2 >
{
public:
	Class7();
    Class7( PerIstream& istr );
	virtual ~Class7();

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class7 );

    friend bool operator ==( const Class7< T1, T2 >&, const Class7< T1, T2 >& );

private:
};

PER_READ_WRITE_T2( Class7 );

template< class T1, class T2 >
Class7< T1, T2 >::Class7()
{
}

template< class T1, class T2 >
Class7< T1, T2 >::Class7( PerIstream& istr )
{
    istr >> *this;
}

template< class T1, class T2 >
Class7< T1, T2 >::~Class7()
{
}

template< class T1, class T2 >
ostream& operator<<( ostream& ostr, const Class7< T1, T2 >& t )
{
	const Class5< T1 >& c5 = t;
	const Class6< T2 >& c6 = t;
	ostr << c5;
	ostr << c6;

	return ostr;
}

template< class T1, class T2 >
void perWrite( PerOstream& str, const Class7< T1, T2 >& ob )
{
    const Class5< T1 >& c5 = ob;
    const Class6< T2 >& c6 = ob;
    
    str << c5;
    str << c6;
}

template< class T1, class T2 >
void perRead( PerIstream& str, Class7< T1, T2 >& ob )
{
    Class5< T1 >&   c5 = ob;
    Class6< T2 >&   c6 = ob;
    
	str >> c5;
	str >> c6;
}

template< class T1, class T2 >
bool operator ==( const Class7< T1, T2 >& a, const Class7< T1, T2 >& b )
{
	const Class5< T1 >& c5a = a;
	const Class5< T1 >& c5b = b;
	const Class6< T2 >& c6a = a;
	const Class6< T2 >& c6b = b;

    bool    result = true;
    
    COMPARE_OBJECTS( c5a, c5b );
    COMPARE_OBJECTS( c6a, c6b );

    return result;
}

PER_DECLARE_PERSISTENT_T2( Class7 );

#endif