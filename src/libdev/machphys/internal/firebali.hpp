#ifndef _MACHPHYS_FIREBALL_INTERNAL_HPP
#define _MACHPHYS_FIREBALL_INTERNAL_HPP

#include "base/base.hpp"

#include "ctl/vector.hpp"

#include "render/render.hpp"

#include "world4d/visplan.hpp"
#include "world4d/matplan.hpp"

#include "machphys/machphys.hpp"
#include "stdlib/string.hpp"

class RenMaterial;

class MachPhysFireballImpl
{
public:
	MachPhysFireballImpl(
   		MachPhysFireballType fireballType,
   		MATHEX_SCALAR size,
   		const PhysRelativeTime& duration );

	MachPhysFireballImpl();

	~MachPhysFireballImpl();

    typedef ctl_vector< Ren::MaterialVecPtr > Materials;

    static  void addMaterial( Materials* pMaterials, const string& textureFileName );

    static  const Materials&   materials( MachPhysFireballType fireballType );
    static  const Materials&   materials1();
    static  const Materials&   materials2();
    static  const Materials&   materials3();
    static  const Materials&   materials4();

    static  RenMaterial initialMaterial( MachPhysFireballType );

    static  W4dMaterialPlanPtr materialPlanPtr( MachPhysFireballType type );
    static  W4dMaterialPlanPtr materialPlanPtr1();
    static  W4dMaterialPlanPtr materialPlanPtr2();
    static  W4dMaterialPlanPtr materialPlanPtr3();
    static  W4dMaterialPlanPtr materialPlanPtr4();
    static  const PhysRelativeTime timePerFrame();

    static const W4dVisibilityPlanPtr& visibilityPlanPtr( MachPhysFireballType fireballType );
    static const W4dVisibilityPlanPtr& visibilityPlanPtr1();
    static const W4dVisibilityPlanPtr& visibilityPlanPtr2();
    static const W4dVisibilityPlanPtr& visibilityPlanPtr3();
    static const W4dVisibilityPlanPtr& visibilityPlanPtr4();

    void constructEntityPlan(
        MachPhysFireballType fireballType,
        MATHEX_SCALAR dOffset,
        const PhysRelativeTime& duration );

    //Data members
    MATHEX_SCALAR   size_;
    PhysRelativeTime maxDuration_;
    MachPhysFireballType fireballType_;
};

#endif // _MACHPHYS_FIREBALL_INTERNAL_HPP
