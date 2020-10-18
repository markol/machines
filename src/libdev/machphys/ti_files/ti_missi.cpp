/*
 * T I _ M I S S I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"

#include "machphys/missile.hpp"

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, size_t, MachPhysMissile );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, size_t, MachPhysMissile );

void MachDummyFunctionMissile()
{
    static  MachPhysObjectFactory< size_t, MachPhysMissile > dummyFactory5( 1 );
}

/* End TI_ADMIN.CPP *****************************************************/
