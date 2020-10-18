#include "pertest/persist.hpp"
#include "pertest/class1.hpp"
#include "pertest/per.hpp"

Class1::Class1()
: i_( randomInt() ),
  d_( randomDouble() )
{
}

Class1::~Class1()
{
}

ostream& operator<<( ostream& ostr, const Class1& t )
{
	ostr << t.i_ << "  ";
	ostr << t.d_ << std::endl;

	return ostr;
}

void perWrite( PerOstream& ostr, const Class1& ob )
{
WHERE;
std::cout << (void*)&ob.i_ << std::endl;
	ostr << ob.i_;
	ostr << ob.d_;
}

void perRead( PerIstream& istr, Class1& ob )
{
	istr >> ob.i_;
	istr >> ob.d_;
}

bool operator ==( const Class1& a, const Class1& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.i_, b.i_ );
    COMPARE_OBJECTS( a.d_, b.d_ );

    return result;
}

bool operator !=( const Class1& a, const Class1& b )
{
    return ! (a == b);
}

PER_DEFINE_PERSISTENT( Class1 );
