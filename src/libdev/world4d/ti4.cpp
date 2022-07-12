// These don't appear to be required?
//#include "ctl/vector.hpp"
//#include "ctl/vector.ctp"

#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"
#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "world4d/compplan.hpp"


W4dDummyFunction4()
{
    static CtlCountedPtr< W4dCompositePlan >   dummyPtr;
    static vector< CtlCountedPtr< W4dCompositePlan > >  dummy1;
}