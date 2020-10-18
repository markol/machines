/*
 * T I G U I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "machgui/map.hpp"
#include "mathex/point2d.hpp"

class GuiDisplayable;

static void tiGui()
{
	static ctl_vector< MexPoint2d >			dummy1;
	static ctl_vector< GuiDisplayable * >	dummy2;
	static ctl_vector< MachContinentMap::ActorPosInfo > dummy3;
}

/* End TIGUI.CPP ****************************************************/

