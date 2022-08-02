/*
 * C L A S S 1 . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Non-template
    Public data members
    Default constructor
    Not part of inheritance hierarchy
    Not inline
*/

#ifndef _PER_CLASS1_HPP
#define _PER_CLASS1_HPP

#include "base/persist.hpp"

class Class1
{
public:
	Class1();
	~Class1();

	int		i_;
	double	d_;

    friend bool operator ==( const Class1& a, const Class1& b );
    friend bool operator !=( const Class1& a, const Class1& b );

	PER_MEMBER_PERSISTENT_DEFAULT( Class1 );

private:

};

ostream& operator<<( ostream& ostr, const Class1& t );

PER_READ_WRITE( Class1 );
PER_DECLARE_PERSISTENT( Class1 );

#endif
