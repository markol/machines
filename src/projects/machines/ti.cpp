/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

//#include <string.hpp>
//#include "ctl/vector.hpp"
#include "ctl/list.hpp"

class MotionControl;

void TerrainDemoDummyFunction1()
{
	static ctl_list<MotionControl*> motionList;
//    static ctl_vector< string >    dummyVectorString;
}

/* End TI.CPP *****************************************************/
