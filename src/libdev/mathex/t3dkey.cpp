/*
 * T 3 D K E Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <iostream>

#include "mathex/t3dkey.hpp"

#ifndef _INLINE
    #include "mathex/t3dkey.ipp"
#endif

//class variables
ulong MexTransform3dKey::lastKey_ = 0;

//---------------------------------------------------------------------
void MexTransform3dKey::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
ostream& operator <<( ostream& o, const MexTransform3dKey& t )
{

    o << "MexTransform3dKey " << t.key_ << std::endl;

    return o;
}
//---------------------------------------------------------------------
/*
PER_DEFINE_PERSISTENT( MexTransform3dKey );

void perWrite( PerOstream& str, const MexTransform3dKey& t )
{
    str << t.key_;
}

void perRead( PerIstream& str, MexTransform3dKey& t )
{
    str >> t.key_;
}

*/
/* End T3DKEY.CPP ***************************************************/
