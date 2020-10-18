/*
 * D E G R E E S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/degrees.hpp"

#ifndef _INLINE
    #include "mathex/degrees.ipp"
#endif

// MexDegrees::MexDegrees()
// {
// 
//     TEST_INVARIANT;
// }
// 
// MexDegrees::~MexDegrees()
// {
//     TEST_INVARIANT;
// 
// }
// 
// void MexDegrees::CLASS_INVARIANT
// {
// }
// 
PER_DEFINE_PERSISTENT( MexDegrees );

void perWrite( PerOstream& str, const MexDegrees& t )
{

	//PER_WRITE_BASE_CLASS( str, MexAngle, t );

	const MexAngle& angle = t;
	perWrite( str, angle );
}

void perRead( PerIstream& str, MexDegrees& t )
{
	//str >> _STATIC_CAST( MexAngle&, t );

	MexAngle& angle = t;
	perRead( str, angle );
}

/* End DEGREES.CPP **************************************************/
