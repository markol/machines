#include "ctl/countptr.hpp"
//#include "ctl/countptr.ctp"
#include "phys/sclplan.hpp"

//These included to prevent compiler complaining
#include "mathex/mathex.hpp"
#include "mathex/transf3d.hpp"
#include "phys/motplan.hpp"
#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"

PhysDummyFunction19()
{
	static CtlCountedPtr<PhysScalarPlan> sPtr;
}
