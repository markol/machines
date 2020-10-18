/*
 * A N G L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/angle.hpp"
#include <iostream>

#ifndef _INLINE
    #include "mathex/angle.ipp"
#endif

// MexAngle::MexAngle()
// {
//
//     TEST_INVARIANT;
// }
//
MexAngle::~MexAngle()
{
    TEST_INVARIANT;

}

void MexAngle::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}

ostream& operator<<(ostream& str, const MexAngle& t)
{
	str << t.angleInRadians_;
	return str << std::endl;
}

PER_DEFINE_PERSISTENT( MexAngle );

void perWrite( PerOstream& stream, const MexAngle& t )
{
	stream << t.angleInRadians_;
}

void perRead( PerIstream& stream, MexAngle& t )
{
	stream >> t.angleInRadians_;

}

/* End ANGLE.CPP **************************************************/
