/*
 * T I _ F A . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//#include "utility/basfact.hpp"
//#include "utility/basfact.ctp"

#include "machphys/ofactory.hpp"
//#include "machphys/ofactory.ctp"
#include "machphys/subid.hpp"
//#include "machphys/subid.ctp"

#include "machphys/factory.hpp"

typedef MachPhysFactory::Id   MachPhysFactoryId;

PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysFactoryId, MachPhysFactory );
PER_PRIVATE_READ_WRITE_PERSISTENT_T2( MachPhysObjectFactory, MachPhysFactoryId, MachPhysFactory );

void MachDummyFunctionFactory()
{
    static  MachPhysObjectFactory< MachPhysFactory::Id, MachPhysFactory >   dummyFactory( 1 );
}

/* End TI8.CPP *****************************************************/
