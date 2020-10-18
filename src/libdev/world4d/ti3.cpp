#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/map.hpp"
#include "string.hpp";

W4dDummyFunction3( void )
{
	ctl_map< string, uint, less< string > > dummySoundIDMap;
}

//#include "ctl/vector.hpp"
//#include "ctl/vector.ctp"
//#include "ctl/countptr.hpp"
//#include "ctl/countptr.ctp"
//
//#include "phys/motplan.hpp"
//#include "world4d/matplan.hpp"
//#include "world4d/visplan.hpp"
//#include "world4d/objtrack.hpp"
//
//
//W4dDummyFunction3( void )
//{
//    static CtlCountedPtr<W4dMaterialPlan> dummy;
//    static CtlCountedPtr<W4dVisibilityPlan> dummy1;
//    static CtlCountedPtr<W4dObjectTracker> dummy2;
//    static ctl_vector<size_t> dummySize_t;
//}