/*
 * T I _ P C A R R . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"

#include "ctl/map.hpp"
//#include "ctl/map.ctp"

#include "machphys/apc.hpp"

typedef MachPhysAPC::Id   MachPhysAPCId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAPCId, MachPhysAPC );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAPCId, MachPhysAPC );


void MachDummyFunctionPersonnelCarrier()
{
    static  MachPhysObjectFactory< size_t, MachPhysAPC >   dummyFactory( 1 );
}

/* End TI2.CPP *****************************************************/
