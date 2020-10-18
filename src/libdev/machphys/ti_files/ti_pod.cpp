/*
 * T I _ P O D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

//#include "utility/basfact.hpp"
//#include "utility/basfact.ctp"

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"

#include "ctl/fixedvec.hpp"
//#include "ctl/fixedvec.ctp"

#include "machphys/pod.hpp"

typedef MachPhysPod::Id   MachPhysPodId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysPodId, MachPhysPod );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysPodId, MachPhysPod );

void MachDummyFunctionPod()
{
    static  MachPhysObjectFactory< MachPhysPod::Id, MachPhysPod >   dummyFactory( 1 );
}

/* End TI8.CPP *****************************************************/
