/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/fixedvec.hpp"
#undef _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "ctl/pvector.hpp"
// #include "ctl/list.hpp"

// #include "ctl/vector.ctp"
// #include <ospace/stl/vector.cc>

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"
#include "world4d/compplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/valplan.hpp"

#include "render/material.hpp"
#include "machphys/tileboun.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/machphys.hpp"
#include "machphys/surfchk.hpp"

class W4dDomain;
class W4dLink;
class MachPhysTerrainTile;
class MachPhysFireball;

void MachDummyFunction1()
{
    static  ctl_vector< W4dDomain* >            dummyVectorW4dDomain;
    static  ctl_vector< ctl_vector< W4dDomain* > >  dummyVectorVectorW4dDomain;
    static  ctl_vector< MachPhysTerrainTile* >  dummyVectorTerrainTile;
    static  ctl_vector< ctl_vector< MachPhysTerrainTile* > >  dummyVectorVectorTerrainTile;
    static  ctl_vector< MachPhysFireball* > dummyVectorFireballs;

    static  ctl_vector<MATHEX_SCALAR>                       dummyVectorMATHEX_SCALAR;
    static  ctl_vector<MachPhysTileBoundary::TriangleData>  dummyVectorTriangleData;
    static  ctl_vector<MachPhysTileBoundary::CellPair>      dummyVectorCellPair;
    static  ctl_vector< W4dCompositePlanPtr > dummyCompositePlanPtrs;

    static  ctl_vector< MachPhys::Race >  dummy1;
    static  ctl_fixed_vector< W4dLink* >  dummy3(2, NULL);

    static  MachPhysTileData::IntersectingTriangles   dummy2;

    static  MachPhysPlanetSurfaceChecker::Triangles dummy10;

    static  ctl_pvector< W4dDomain >            dummyPVectorW4dDomain;

    static  ctl_vector< MexPoint2d > dummyPoints;
}

/* End TI.CPP *****************************************************/
