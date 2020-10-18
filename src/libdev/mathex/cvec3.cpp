/*
 * C V E C 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cvec3.hpp"
#include "mathex/vec3.hpp"

MexCompressedVec3::MexCompressedVec3()
{
	vec_[0]=0;
	vec_[1]=0;
	vec_[2]=0;
    TEST_INVARIANT;
}

MexCompressedVec3::MexCompressedVec3( const MexVec3& vec )
{
	vec_[0] = vec.x();
	vec_[1] = vec.y();
	vec_[2] = vec.z();
}

MexCompressedVec3::~MexCompressedVec3()
{
    TEST_INVARIANT;

}

void MexCompressedVec3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedVec3& t )
{

	o << " MexCVec3: ( " << t.vec_[0] << ", " << t.vec_[1] << ", " << t.vec_[2] << " )" << std::endl;

    return o;
}

/* End CVEC3.CPP ****************************************************/
