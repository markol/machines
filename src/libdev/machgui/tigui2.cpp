/*
 * T I G U I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

// #include <memory.hpp>

#include "ctl/list.hpp"

// #include "phys/motplan.hpp"

// #include "machgui/command.hpp"

// #include "world4d/meshplan.hpp"
// #include "world4d/matplan.hpp"
// #include "world4d/visplan.hpp"
// #include "world4d/valplan.hpp"

class MachGuiCommand;

static void tiGui()
{
//	static ctl_list< auto_ptr< MachGuiCommand > >	dummy1;
	static ctl_list< MachGuiCommand * >	dummy1;
}

/* End TIGUI.CPP ****************************************************/

