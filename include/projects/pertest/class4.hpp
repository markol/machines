/*
    Class containing pointers to objects of other classes
*/

#ifndef _PER_CLASS4_HPP
#define _PER_CLASS4_HPP

#include "base/persist.hpp"
#include "pertest/class3.hpp"

class Class4
{
public:
	Class4();
    virtual ~Class4();
    Class4( PerIstream& istr );

    enum Test { VALUE1, VALUE2 };
    
	Class1*	pC1_;
	Class2*	pC2_;
	Class3*	pC3_;
	Class3	c3_;
    Test    val_;

    friend bool operator ==( const Class4& a, const Class4& b );

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class4 );

    PER_FRIEND_ENUM_PERSISTENT( Test );

private:
};

ostream& operator<<( ostream& ostr, const Class4& t );

PER_READ_WRITE( Class4 );
PER_DECLARE_PERSISTENT( Class4 );

#endif