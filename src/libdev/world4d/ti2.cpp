#include "ctl/list.hpp"
#include "render/matmap.hpp"

#include "ctl/list.ctp"
#include "ctl/set.ctp"
#include "ctl/map.ctp"

class W4dD3DDriver;

W4dDummyFunction2()
{
	static ctl_list<const W4dD3DDriver*>	driverList;
    static RenMaterialSet               	dummySet;
}
