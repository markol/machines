/*
 * T I _ A D M I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/administ.hpp"

typedef MachPhysAdministrator::Id   MachPhysAdministratorId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAdministratorId, MachPhysAdministrator );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAdministratorId, MachPhysAdministrator );


void MachDummyFunctionAdministrator()
{
    static  MachPhysObjectFactory< MachPhysAdministrator::Id, MachPhysAdministrator > dummyFactory5( 1 );
}

/* End TI_ADMIN.CPP *****************************************************/
