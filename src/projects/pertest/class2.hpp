/*
    Test inclusion of one class in another and pointer to class persistence
    Test counted pointers persistence
*/

#ifndef _PER_CLASS2_HPP
#define _PER_CLASS2_HPP

#include "base/persist.hpp"
#include "pertest/class1.hpp"
#include "ctl/countptr.hpp"
#include "stdlib/string.hpp"

class Class2
{
public:
	Class2();
	Class2( PerIstream& istr );
    virtual ~Class2();

	int		i2_;
	double	d2_;
	Class1	c1_;
	Class1*	pC1_;

    string  rawString1_;

    CtlCountedPtr< int >    p1_;
    CtlCountedPtr< int >    p2_;

    CtlCountedPtr< int >    nullPtr_;

    CtlConstCountedPtr< int >    cPtr1_;
    CtlConstCountedPtr< int >    cPtr2_;

    CtlConstCountedPtr< int >    cNullPtr_;

    friend bool operator ==( const Class2&, const Class2& );
    friend bool operator !=( const Class2&, const Class2& );

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class2 );

private:
};

ostream& operator<<( ostream& ostr, const Class2& t );

PER_READ_WRITE( Class2 );
PER_DECLARE_PERSISTENT( Class2 );

#endif
