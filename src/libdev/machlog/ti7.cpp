/*
 * T I 7 . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/map.hpp"
#include "ctl/map.ctp"
#include "stdlib/string.hpp"

class SimCondition;

void ti7()
{
    static ctl_map< string , SimCondition*, less< string > >		conditionMap;

}

/* End TI6.CPP *******************************************************/	
