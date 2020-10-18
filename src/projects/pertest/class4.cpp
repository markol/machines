#include "pertest/class4.hpp"
#include "pertest/persist.hpp"

PER_DEFINE_PERSISTENT( Class4 );


// PER_DEFINE_ENUM_PERSISTENT( Class4::Test );

Class4::Class4()
: pC3_( _NEW( Class3() ) )
{
    pC1_ = pC3_;
    pC2_ = &c3_;
}

Class4::~Class4()
{
    _DELETE( pC3_ );
}

ostream& operator<<( ostream& ostr, const Class4& t )
{
    ostr << "Class 4 start" << endl;
	ostr << (void*)t.pC1_ << endl;
    if( t.pC1_ )
        ostr << *t.pC1_;
	ostr << (void*)t.pC2_ << endl;
    if( t.pC2_ )
        ostr << *t.pC2_;
	ostr << (void*)t.pC3_ << endl;
    if( t.pC3_ )
        ostr << *t.pC3_;
	ostr << (void*)&t.c3_ << endl;
    ostr << t.c3_;
    ostr << "Class 4 end" << endl;

	return ostr;
}

void perWrite( PerOstream& str, const Class4& ob )
{
	str << ob.pC1_;
	PER_WRITE_RAW_POINTER( str, ob.pC2_ );
	str << ob.pC3_;
	str << ob.c3_;
    str << ob.val_;
}

void perRead( PerIstream& str, Class4& ob )
{
    _DELETE( ob.pC3_ );
    
	str >> ob.pC1_;
	PER_READ_RAW_POINTER( str, ob.pC2_ );
	str >> ob.pC3_;
	str >> ob.c3_;
    str >> ob.val_;
}

bool operator ==( const Class4& a, const Class4& b )
{
    bool    result = true;
    
    COMPARE_POINTERS( a.pC1_, b.pC1_ );
    COMPARE_POINTERS( a.pC2_, b.pC2_ );
    COMPARE_POINTERS( a.pC3_, b.pC3_ );
    COMPARE_OBJECTS( a.c3_, b.c3_ );
    
    return result;
}