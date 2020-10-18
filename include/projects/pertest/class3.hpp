/*
    Test persistence of a multiply derived class
*/

#ifndef _PER_CLASS3_HPP
#define _PER_CLASS3_HPP

#include "base/persist.hpp"
#include "pertest/class1.hpp"
#include "pertest/class2.hpp"
#include "utility/property.hpp"

class Class3 : public Class1, public Class2
{
public:
	Class3();
    Class3( PerIstream& istr );
    virtual ~Class3();

	int		i3_;
	double	d3_;

    UtlProperty< double >   da3;

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class3 );

    friend bool operator ==( const Class3&, const Class3& );
    friend bool operator !=( const Class3&, const Class3& );

private:
};

ostream& operator<<( ostream& ostr, const Class3& t );

PER_READ_WRITE( Class3 );
PER_DECLARE_PERSISTENT( Class3 );


#endif
