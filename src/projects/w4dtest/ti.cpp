/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "ctl/list.hpp"
#include "ctl/list.ctp"

class MotionControl;
class ModelData;

void TerrainDemoDummyFunction1()
{
	static ctl_list<MotionControl*> motionList;
    static  ctl_vector<ModelData*>    dummyModels;
}

/* End TI.CPP *****************************************************/
