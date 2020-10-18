#include "pertest/class2.hpp"
#include "pertest/persist.hpp"
#include "pertest/per.hpp"

Class2::Class2()
: i2_( randomInt() ),
  d2_( randomDouble() ),
  pC1_( NULL )
{
    p1_ = _NEW( int );
    *p1_ = randomInt();
    p2_ = p1_;

    cPtr1_ = _NEW( int( randomInt() ) );
    cPtr2_ = p1_;

    rawString1_ = "raw string 1";
}

Class2::Class2( PerIstream& istr )
{
    istr >> *this;
}

Class2::~Class2()
{
    _DELETE( pC1_ );
}

ostream& operator<<( ostream& ostr, const Class2& t )
{
	ostr << t.i2_ << "  ";
	ostr << t.d2_ << std::endl;
	ostr << t.c1_;
	ostr << (void*)&t.c1_ << std::endl;
	ostr << (void*)t.pC1_ << std::endl;
    if( t.p1_.isDefined() )
        ostr << *t.p1_ << std::endl;
    if( t.p2_.isDefined() )
        ostr << *t.p2_ << std::endl;
    if( t.cPtr1_.isDefined() )
        ostr << *t.cPtr1_ << std::endl;
    if( t.cPtr2_.isDefined() )
        ostr << *t.cPtr2_ << std::endl;

    ostr << t.rawString1_ << std::endl;

	return ostr;
}

void perWrite( PerOstream& str, const Class2& ob )
{
WHERE;
std::cout << (void*)&ob.i2_ << std::endl;
	str << ob.i2_;
WHERE;
	str << ob.d2_;
WHERE;
	str << ob.pC1_;
WHERE;
	str << ob.c1_;
WHERE;
    str << ob.p1_;
WHERE;
    str << ob.p2_;
WHERE;
    str << ob.nullPtr_;
WHERE;
    str << ob.cPtr1_;
WHERE;
    str << ob.cPtr2_;
WHERE;
    str << ob.cNullPtr_;
WHERE;
    //  Check that it is possible to write out
    //  two raw data objects at the same address without problems.

    PER_WRITE_RAW_OBJECT( str, ob.rawString1_ );
    PER_WRITE_RAW_OBJECT( str, ob.rawString1_ );
}

void perRead( PerIstream& str, Class2& ob )
{
    _DELETE( ob.pC1_ );

	str >> ob.i2_;
	str >> ob.d2_;
	str >> ob.pC1_;
	str >> ob.c1_;
    str >> ob.p1_;
    str >> ob.p2_;
    str >> ob.nullPtr_;
    str >> ob.cPtr1_;
    str >> ob.cPtr2_;
    str >> ob.cNullPtr_;
    PER_READ_RAW_OBJECT( str, ob.rawString1_ );
    PER_READ_RAW_OBJECT( str, ob.rawString1_ );
}

bool operator ==( const Class2& a, const Class2& b )
{
    bool result = true;

    COMPARE_OBJECTS( a.i2_, b.i2_ );
    COMPARE_OBJECTS( a.d2_, b.d2_ );
    COMPARE_OBJECTS( a.c1_, b.c1_ );
    COMPARE_POINTERS( a.pC1_, b.pC1_ );
    COMPARE_COUNTED_POINTERS( a.p1_, b.p1_ );
    COMPARE_COUNTED_POINTERS( a.p2_, b.p2_ );
    COMPARE_COUNTED_POINTERS( a.nullPtr_, b.nullPtr_ );
    COMPARE_COUNTED_POINTERS( a.cPtr1_, b.cPtr1_ );
    COMPARE_COUNTED_POINTERS( a.cPtr2_, b.cPtr2_ );
    COMPARE_COUNTED_POINTERS( a.cNullPtr_, b.cNullPtr_ );
    COMPARE_OBJECTS( a.rawString1_, b.rawString1_ );

    return result;
}

bool operator !=( const Class2& a, const Class2& b )
{
    return !(a == b);
}

PER_DEFINE_PERSISTENT( Class2 );
