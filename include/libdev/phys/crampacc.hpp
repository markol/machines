/*
 * C R A M P A C C . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysCompressedRampAcceleration

    A brief description of the class should go in here
*/

#ifndef _PHYS_CRAMPACC_HPP
#define _PHYS_CRAMPACC_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"

class PhysRampAcceleration;

#pragma pack(push,1)
class PhysCompressedRampAcceleration
// Canonical form revoked
{
public:
    PhysCompressedRampAcceleration();
    PhysCompressedRampAcceleration( const PhysRampAcceleration& );
    ~PhysCompressedRampAcceleration( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PhysCompressedRampAcceleration& t );
	friend class PhysRampAcceleration;

    //data members
    float 						startSpeed_; //Initial speed
    float 						firstAcceleration_; //Signed
    float 						secondAcceleration_; //Signed
    PhysCompressedRelativeTime 	totalTime_; //Cumulative time of all 3 sections
    PhysCompressedRelativeTime 	firstAccelerationTime_; //Time of 1st acceleration period
    PhysCompressedRelativeTime 	secondAccelerationTime_; //Time of 2nd acceleration period
};
#pragma pack(pop)

#endif

/* End CRAMPACC.HPP *************************************************/
