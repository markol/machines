/*
 * T I _ D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/fixedvec.hpp"

class MachPhysMachineData;
class MachPhysConstructionData;
class MachPhysAdministratorData;
class MachPhysAggressorData;
class MachPhysConstructorData;
class MachPhysGeoLocatorData;
class MachPhysAPCData;
class MachPhysResourceCarrierData;
class MachPhysSpyLocatorData;
class MachPhysTechnicianData;

void MachDummyFunctionAllData()
{
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysMachineData* > >    dummy1;
    static  ctl_fixed_vector< MachPhysConstructionData* >   dummy2;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysAdministratorData* > >  dummy3;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysAggressorData* > >  dummy4;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysConstructorData* > >    dummy5;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysGeoLocatorData* > > dummy6;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysAPCData* > >   dummy7;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysResourceCarrierData* > >    dummy8;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysSpyLocatorData* > > dummy9;
    static  ctl_fixed_vector< ctl_fixed_vector< MachPhysTechnicianData* > > dummy10;
}

/* End TI_DATA.CPP *****************************************************/
