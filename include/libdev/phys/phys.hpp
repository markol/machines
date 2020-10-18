/*
 * M A T H E X . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _PHYS_PHYS_HPP
#define _PHYS_PHYS_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

//Represents a level of detail value for use in eg bounding volume
//intersection tests.
typedef uint PhysLOD;

//Represents the time of an event
typedef MATHEX_SCALAR PhysRelativeTime;
typedef MATHEX_SCALAR PhysAbsoluteTime;

typedef float PhysCompressedRelativeTime;
typedef float PhysCompressedAbsoluteTime;

typedef int   PhysPathFindingPriority;

class Phys
{
public:
	static PhysAbsoluteTime time();
    static PhysPathFindingPriority     defaultPathFindingPriority();
};

//Forward declarations
class PhysMotionPlan;
template < class T > class CtlCountedPtr;
typedef CtlCountedPtr<PhysMotionPlan> PhysMotionPlanPtr;

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
//    #include "phys/phys.ipp"
//    #include "phys/phys.itf"
#endif

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
//    #include "phys/phys.ctf"
#endif

//////////////////////////////////////////////////////////////////////

#endif  /*  #ifndef _MEX_MATHEX_HPP    */

/* End MATHEX.HPP *****************************************************/
