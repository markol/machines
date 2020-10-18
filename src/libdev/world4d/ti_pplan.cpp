#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"

#include "ctl/list.hpp"
#include "ctl/list.ctp"

// The files marked "accidental" shouldn't be necessary.  They must be included
// because Watcom is trying to instantiate everything it sees.
#include "mathex/transf3d.hpp" 		// accidental
#include "phys/rampacce.hpp"		// accidental
#include "phys/motplan.hpp"			// accidental
#include "render/mesh.hpp"			// accidental
#include "render/matvec.hpp"		// accidental
#include "render/uvanim.hpp"		// accidental

#include "world4d/compplan.hpp"
#include "world4d/valplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/meshplan.hpp"

#include "internal/pendplan.hpp"

typedef W4dPendingPlan< PhysMotionPlanPtr >		PendingMotionPlan;
typedef W4dPendingPlan< W4dVisibilityPlanPtr >	PendingVisibilityPlan;
typedef W4dPendingPlan< W4dMeshPlanPtr >		PendingMeshPlan;
typedef W4dPendingPlan< W4dScalePlanPtr >	    PendingScalePlan;
typedef W4dPendingPlan< W4dUVPlanPtr >			PendingUVPlan;
typedef W4dPendingPlan< W4dMaterialPlanPtr >	PendingMaterialPlan;

typedef ctl_list<PendingMotionPlan> 			PendingMotionPlans;
typedef ctl_list<PendingMeshPlan>				PendingMeshPlans;
typedef ctl_list<PendingVisibilityPlan>			PendingVisibilityPlans;
typedef ctl_list<PendingScalePlan> 				PendingScalePlans;
typedef ctl_list<PendingUVPlan>					PendingUVPlans;
typedef ctl_list<PendingMaterialPlan>			PendingMaterialPlans;

W4dDummyFunctionPlanPtrs()
{
    static CtlCountedPtr<W4dCompositePlan>		dummy3;
    static CtlCountedPtr<W4dFloatValuePlan>		dummy4;
    static CtlCountedPtr<W4dVisibilityPlan>		dummy2;
    static CtlCountedPtr<W4dMaterialPlan>		dummy1;
    static CtlCountedPtr<W4dUVPlan>				dummy5;
    static CtlCountedPtr<W4dScalePlan>			dummy7;
    static CtlCountedPtr<W4dMeshPlan>			dummy6;

    static PendingMotionPlans      absoluteMotionPlans_;
    static PendingMeshPlans        meshPlans_;
    static PendingVisibilityPlans  visibilityPlans_;
    static PendingScalePlans       scalePlans_;
    static PendingUVPlans	        uvPlans_;
    static PendingMaterialPlans    materialPlans_;
}
