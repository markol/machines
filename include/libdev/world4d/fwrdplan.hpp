/*
 * F W R D P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _WORLD4D_FWRDPLAN_HPP
#define _WORLD4D_FWRDPLAN_HPP

// The miniumum forward declarations required for all plan pointers.
// These are required in more than one header file, so they're grouped
// together simply to avoid duplicating code.
template <class T> class CtlCountedPtr;

class PhysMotionPlan;
class W4dMeshPlan;
class W4dMaterialPlan;
class W4dVisibilityPlan;
class W4dScalePlan;
class W4dUVPlan;
class W4dCompositePlan;

typedef CtlCountedPtr<W4dMeshPlan>			W4dMeshPlanPtr;
typedef CtlCountedPtr<W4dMaterialPlan>		W4dMaterialPlanPtr;
typedef CtlCountedPtr<W4dVisibilityPlan>	W4dVisibilityPlanPtr;
typedef CtlCountedPtr<W4dScalePlan>	        W4dScalePlanPtr;
typedef CtlCountedPtr<W4dUVPlan>	        W4dUVPlanPtr;
typedef CtlCountedPtr<W4dCompositePlan>		W4dCompositePlanPtr;
typedef CtlCountedPtr<PhysMotionPlan>		PhysMotionPlanPtr;

#endif

/* End FWRDPLAN.HPP *************************************************/
