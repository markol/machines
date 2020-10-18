/*
 * P E R S I S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/persist.hpp"

// Note: unfortunately there are #include order dependencies due to forward
// declarations of templates.
#include "phys/rampacce.hpp"		// order dependencies
#include "stdlib/string.hpp"	// order dependencies

#include "world4d/root.hpp"
#include "world4d/domain.hpp"
#include "world4d/portal.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/generic.hpp"
#include "world4d/spin.hpp"
#include "world4d/sprite3d.hpp"
#include "world4d/camera.hpp"
#include "world4d/custom.hpp"
#include "world4d/shadow.hpp"
#include "world4d/backgrnd.hpp"
#include "world4d/meshplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/matplan.hpp"
#include "world4d/light.hpp"
#include "world4d/link.hpp"

#include "world4d/internal/multitex.hpp"
#include "world4d/internal/uvdata.hpp"
#include "world4d/internal/multicol.hpp"
#include "world4d/internal/lightdat.hpp"
#include "world4d/internal/pendplan.hpp"

#include "mathex/transf3d.hpp"

#include "phys/motplan.hpp"

PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, PhysMotionPlanPtr );
PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, W4dMeshPlanPtr );
PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, W4dVisibilityPlanPtr );
PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, W4dScalePlanPtr );
PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, W4dUVPlanPtr );
PER_DEFINE_PERSISTENT_T1( W4dPendingPlan, W4dMaterialPlanPtr );

// static
W4dPersistence& W4dPersistence::instance()
{
    static  W4dPersistence  instance_;

    return instance_;
}

W4dPersistence::W4dPersistence()
{
    PER_REGISTER_DERIVED_CLASS( W4dRoot );
    PER_REGISTER_DERIVED_CLASS( W4dLink );
    PER_REGISTER_DERIVED_CLASS( W4dDomain );
    PER_REGISTER_DERIVED_CLASS( W4dGeneric );
    PER_REGISTER_DERIVED_CLASS( W4dGenericComposite );
    PER_REGISTER_DERIVED_CLASS( W4dSprite3d );
    PER_REGISTER_DERIVED_CLASS( W4dSpinSprite );
    PER_REGISTER_DERIVED_CLASS( W4dPortal );
    PER_REGISTER_DERIVED_CLASS( W4dCamera );
    PER_REGISTER_DERIVED_CLASS( W4dCustom );
    //PER_REGISTER_DERIVED_CLASS( W4dLight );
    PER_REGISTER_DERIVED_CLASS( W4dUniformLight );
    PER_REGISTER_DERIVED_CLASS( W4dDirectionalLight );
    PER_REGISTER_DERIVED_CLASS( W4dPointLight );
    PER_REGISTER_DERIVED_CLASS( W4dShadowProjected2d );
    PER_REGISTER_DERIVED_CLASS( W4dShadowFixed );
    PER_REGISTER_DERIVED_CLASS( W4dCycleMultiTextureData );
    PER_REGISTER_DERIVED_CLASS( W4dUVTranslateData );
    PER_REGISTER_DERIVED_CLASS( W4dMultiColourPulseData );
    PER_REGISTER_DERIVED_CLASS( W4dLightData );
}

W4dPersistence::~W4dPersistence()
{
}

void W4dPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dPersistence& t )
{

    o << "W4dPersistence " << (void*)&t << " start" << std::endl;
    o << "W4dPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PERSIST.CPP **************************************************/
