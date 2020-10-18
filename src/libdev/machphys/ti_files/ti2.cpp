/*
 * T I 9 . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "mathex/transf3d.hpp"
#include "machphys/smokplum.hpp"
#include "world4d/entity.hpp"
#include "world4d/visplan.hpp"
#include "world4d/valplan.hpp"
#include "render/texture.hpp"

class MachPhysEntrance;

void MachDummyFunction9()
{
    static MachPhysSmokePlume dummy2(
        NULL, MexTransform3d(), 0.0, 0.0, 0, 0.0, PUFF_1, 0.0 );

    static ctl_vector< MachPhysEntrance* >  dummy3;
    static ctl_vector< RenTexture >  dummy4;
}

/* End TI9.CPP *****************************************************/
