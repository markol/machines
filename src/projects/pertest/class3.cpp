#include "pertest/class3.hpp"
#include "pertest/persist.hpp"
#include "pertest/per.hpp"

PER_DEFINE_PERSISTENT( Class3 );

Class3::Class3()
: i3_( randomInt() ),
  d3_( randomDouble() )
{
}

Class3::Class3( PerIstream& istr )
{
    istr >> *this;
}

Class3::~Class3()
{
}

ostream& operator<<( ostream& ostr, const Class3& t )
{
    const Class1& c1 = t;
    const Class2& c2 = t;
	ostr << c1;
	ostr << c2;
	ostr << t.i3_ << "  ";
	ostr << t.d3_ << std::endl;
    ostr << t.da3() << std::endl;

	return ostr;
}

void perWrite( PerOstream& str, const Class3& ob )
{
    const Class1& c1 = ob;
    const Class2& c2 = ob;
	str << c1;
	str << c2;
	str << ob.i3_;
	str << ob.d3_;
    str << ob.da3;
}

void perRead( PerIstream& str, Class3& ob )
{
    Class1& c1 = ob;
    Class2& c2 = ob;
	str >> c1;
	str >> c2;
	str >> ob.i3_;
	str >> ob.d3_;
    str >> ob.da3;
}

bool operator ==( const Class3& a, const Class3& b )
{
    bool    result = true;

    const Class1& c1a = a;
    const Class1& c1b = b;
    const Class2& c2a = a;
    const Class2& c2b = b;

    COMPARE_OBJECTS( c1a, c1b );
    COMPARE_OBJECTS( c2a, c2b );
    COMPARE_OBJECTS( a.i3_, b.i3_ );
    COMPARE_OBJECTS( a.d3_, b.d3_ );
    COMPARE_OBJECTS( a.da3, b.da3 );

    return result;
}

bool operator !=( const Class3& a, const Class3& b )
{
    return !(a == b);
}

