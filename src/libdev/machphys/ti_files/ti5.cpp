/*
 * T I 1 2. C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "machphys/station.hpp"
#include "machphys/plansurf.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "render/texture.hpp"
#include <memory>
#include <memory.ctp>
 
void MachDummyFunction12()
{
    static ctl_vector< MachPhysStation >   dummy1;
    static ctl_vector< MexTransform3d >   dummy2;
    static ctl_vector< MexPoint3d >   dummy3;
    static ctl_vector< RenTexture >   dummy4;
    static ctl_vector< ctl_vector< RenTexture > >   dummy5;
    static auto_ptr< istream >      dummy6;
    static MachPhysPlanetSurface::Floors dummy7;
}

/* End TI12.CPP *****************************************************/
