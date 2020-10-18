/*
 * T I 1 3. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */	  

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "ctl/set.hpp"
#include "ctl/countptr.hpp"
#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/mccospec.hpp"
#include "machphys/mexpdata.hpp"

class MachPhysTerrainTile;
class W4dCompositeMaterialVecChanger;

void MachDummyFunction13()
{
	static ctl_map< string, MachPhysTerrainTile*, less<string> > dummy1;
    static CtlCountedPtr< MachPhysTileData > dummy2;
    static ctl_map< MachPhysMachineSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysMachineSpec_Less > dummy3;
    static ctl_map< MachPhysConstructionSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysConstructionSpec_Less > dummy4;
    static ctl_map< MachPhysWeaponSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysWeaponSpec_Less > dummy5;
    static ctl_set< MexPoint3d, less< MexPoint3d > >    dummy6;
    static CtlCountedPtr< MachPhysMachineExplosionData > dummy7;
}

/* End TI13.CPP *****************************************************/
