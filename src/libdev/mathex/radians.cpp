/*
 * R A D I A N S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/radians.hpp"

#ifndef _INLINE
    #include "mathex/radians.ipp"
#endif

// MexRadians::MexRadians()
// {
// 
//     TEST_INVARIANT;
// }
// 
// MexRadians::~MexRadians()
// {
//     TEST_INVARIANT;
// 
// }
// 
// void MexRadians::CLASS_INVARIANT
// {
// }
// 
// ostream& operator <<( ostream& o, const MexRadians& t )
// {
// 
//     o << "MexRadians " << (void*)&t << " start" << endl;
//     o << "MexRadians " << (void*)&t << " end" << endl;
// 
//     return o;
// }

PER_DEFINE_PERSISTENT( MexRadians );

void perWrite( PerOstream& str, const MexRadians& t )
{
	const MexAngle& angle = t;
	perWrite( str, angle );
}

void perRead( PerIstream& str, MexRadians& t )
{
	MexAngle& angle = t;
	perRead( str, angle );
}

/* End RADIANS.CPP **************************************************/




