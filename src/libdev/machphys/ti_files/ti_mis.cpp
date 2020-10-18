/*
 * T I _ M I S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

//#include "utility/basfact.hpp"
//#include "utility/basfact.ctp"

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/missemp.hpp"

typedef MachPhysMissileEmplacement::Id   MachPhysMissileEmplacementId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysMissileEmplacementId, MachPhysMissileEmplacement );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysMissileEmplacementId, MachPhysMissileEmplacement );


void MachDummyFunctionMissileEmplacement()
{
    static  MachPhysObjectFactory< MachPhysMissileEmplacement::Id, MachPhysMissileEmplacement >   dummyFactory( 1 );
}

/* End TI8.CPP *****************************************************/
