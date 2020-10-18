/*
 * T I _ F A C E . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machphys/ofactory.hpp"
#include "machphys/ofactory.ctp"
// 
// #include "ctl/map.hpp"
// #include "ctl/map.ctp"

// #include <ospace/stl/map.cc>

#include "machphys/machine.hpp"

#include "machphys/facepl.hpp"

void MachDummyFunctionFacePlate()
{
    static  MachPhysObjectFactory< size_t, MachPhysFacePlate >   dummyFactory8( 1 );
}

/* End TI_FACE.CPP *****************************************************/
