/*
 * T I G U I 3. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES


#include "ctl/map.hpp"
#include "stdlib/string.hpp"


class MachGuiDbElement;

static void tiGui3()
{
	static ctl_map< string, MachGuiDbElement*, std::less< string > >	dummy1;
}

/* End TIGUI3.CPP ****************************************************/

