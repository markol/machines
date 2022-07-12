/*
 * C M O T C H U N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysCompressedMotionChunk

    A brief description of the class should go in here
*/

#ifndef _PHYS_CMOTCHUN_HPP
#define _PHYS_CMOTCHUN_HPP

#include "base/base.hpp"
#include "mathex/cpoint2d.hpp"
#include "mathex/double.hpp"
#include "phys/phys.hpp"
#include "phys/crampacc.hpp"

class PhysMotionChunk;

#pragma pack(push,1)
class PhysCompressedMotionChunk
// Canonical form revoked
{
public:
    PhysCompressedMotionChunk();
    PhysCompressedMotionChunk( const PhysMotionChunk& );
    ~PhysCompressedMotionChunk();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PhysCompressedMotionChunk& t );
	friend class PhysMotionChunk;

    MexCompressedPoint2d 			startPoint_; //start location
    MexCompressedPoint2d 			endPoint_; //end location
    PhysCompressedRampAcceleration 	motionProfile_; //Velocity profile for motion between points
    PhysCompressedAbsoluteTime 		createTime_; //Time at which object appears at start point
    PhysCompressedRelativeTime 		motionTimeOffset_; //Offset to start of motion
    PhysCompressedAbsoluteTime 		expiryTime_; //Offset from createTime to time at which object
                                        //disappears from end point
    float 							clearance_; // Radius required around object centre for collision avoidance
    bool 							hasExpiryTime_;

    MexDouble   minHeight_;
    MexDouble   maxHeight_;
};
#pragma pack(pop)

#endif

/* End CMOTCHUN.HPP *************************************************/
