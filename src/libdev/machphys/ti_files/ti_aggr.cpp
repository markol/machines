/*
 * T I _ A G G R . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/aggressr.hpp"

typedef MachPhysAggressor::Id   MachPhysAggressorId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAggressorId, MachPhysAggressor );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysAggressorId, MachPhysAggressor );

void MachDummyFunctionAggressor()
{
    static  MachPhysObjectFactory< MachPhysAggressor::Id, MachPhysAggressor >     dummyFactory4( 1 );
}

/* End TI_AGGR.CPP *****************************************************/
