/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"


class MachPhysComplexityBooleanItem;
class MachPhysComplexityChoiceItem;

void MachDummyFunctionComp()
{
    static  ctl_vector< MachPhysComplexityChoiceItem* > 	dummyVectorComplexityChoiceItem;
    static  ctl_vector< MachPhysComplexityBooleanItem* >	dummyVectorComplexityBooleanItem;
}

/* End TI.CPP *****************************************************/
