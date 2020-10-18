/*
 * T I _ H W . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//#include "utility/basfact.hpp"
//#include "utility/basfact.ctp"

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/hwlab.hpp"

typedef MachPhysHardwareLab::Id   MachPhysHardwareLabId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysHardwareLabId, MachPhysHardwareLab );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysHardwareLabId, MachPhysHardwareLab );

void MachDummyFunctionHardwareLab()
{
    static  MachPhysObjectFactory< MachPhysHardwareLab::Id, MachPhysHardwareLab >   dummyFactory( 1 );
}

/* End TI8.CPP *****************************************************/
