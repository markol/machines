/*
 * T I P C H . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define PRODUCTION

#pragma off(unreferenced)

#include "stdlib/tipch.hpp";

#include "ctl/vector.hpp"

#include "stdlib/string.hpp"
#include "phys/motplan.hpp"
#include "mathex/point3d.hpp"

#include "render/material.hpp"

#include "world4d/compplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/valplan.hpp"

#include "machphys/tiledata.hpp"
#include "machphys/mccospec.hpp"
#include "machphys/tileboun.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/machphys.hpp"
#include "machphys/mccospec.hpp"

class W4dDomain;
class MachPhysTerrainTile;
class MachPhysFireball;
class W4dCompositeMaterialVecChanger;

void MachDummyFunction1()
{
    static  StdPrecompiledHeader< W4dDomain* >            dummyVectorW4dDomain;
    static  StdPrecompiledHeader< ctl_vector< W4dDomain* > >  dummyVectorVectorW4dDomain;
    static  StdPrecompiledHeader< MachPhysTerrainTile* >  dummyVectorTerrainTile;
    static  StdPrecompiledHeader< ctl_vector< MachPhysTerrainTile* > >  dummyVectorVectorTerrainTile;
    static  StdPrecompiledHeader< MachPhysFireball* > dummyVectorFireballs;

    static  StdPrecompiledHeader<MATHEX_SCALAR>                       dummyVectorMATHEX_SCALAR;
    static  StdPrecompiledHeader<MachPhysTileBoundary::TriangleData>  dummyVectorTriangleData;
    static  StdPrecompiledHeader<MachPhysTileBoundary::CellPair>      dummyVectorCellPair;
    static  StdPrecompiledHeader< W4dCompositePlanPtr > dummyCompositePlanPtrs;

    static  StdPrecompiledHeader< MachPhys::Race >  dummy1;

	static StdPrecompiledHeader< string > dummy1a;
	static StdPrecompiledHeader< MachPhysTerrainTile* > dummy1b;
	 
    //static CtlCountedPtr< MachPhysTileData > dummy2;
    static StdPrecompiledHeader< MachPhysMachineSpec > dummy3a;
    static StdPrecompiledheader< W4dCompositeMaterialVecChanger* > dummy3b;
    static StdPrecompiledHeader< MachPhysConstructionSpec > dummy4a;
    static StdPrecompiledHeader< W4dCompositeMaterialVecChanger* > dummy4b;
    static StdPrecompiledHeader< MachPhysWeaponSpec > dummy5a;
    static Stdprecompiledheader< W4dCompositeMaterialVecChanger* > dummy5b;
    static StdPrecompiledHeader< MexPoint3d >    dummy6;
//    static  MachPhysTileData::IntersectingTriangles   dummy2;
}

#pragma on(unreferenced)

/* End TI.CPP *****************************************************/
