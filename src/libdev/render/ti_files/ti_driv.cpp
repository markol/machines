#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"

#include "ctl/list.hpp"
#include "ctl/list.ctp"

#include "render/driv.hpp"

//class RenDriver;

void W4dDummyFunctionDriv()
{
    static CtlCountedPtr< RenDriver >   dummyDriverPtr;

    static ctl_list< CtlCountedPtr< RenDriver > >  dummyDrivers;
}
