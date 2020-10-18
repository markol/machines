/*
 * T I _ T E C H . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/technici.hpp"

typedef MachPhysTechnician::Id   MachPhysTechnicianId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysTechnicianId, MachPhysTechnician );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysTechnicianId, MachPhysTechnician );

void MachDummyFunctionTechnician()
{
    static  MachPhysObjectFactory< MachPhysTechnician::Id, MachPhysTechnician >    dummyFactory6( 1 );
}

/* End TI_TECH.CPP *****************************************************/
