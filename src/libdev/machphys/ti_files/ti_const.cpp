/*
 * T I _ C O N S T . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/construc.hpp"

typedef MachPhysConstructor::Id   MachPhysConstructorId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysConstructorId, MachPhysConstructor );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysConstructorId, MachPhysConstructor );

void MachDummyFunctionConstructor()
{
    static  MachPhysObjectFactory< MachPhysConstructor::Id, MachPhysConstructor >   dummyFactory7( 1 );
}

/* End TI_CONST.CPP *****************************************************/
