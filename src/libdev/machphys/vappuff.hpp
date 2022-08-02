/*
 * V A P P U F F . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysVapourPuff

    A smoke puff attached to a vapour trail
*/

#ifndef _MACHPHYS_VAPPUFF_HPP
#define _MACHPHYS_VAPPUFF_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"

#include "world4d/sprite3d.hpp"
#include "render/material.hpp"
#include "render/render.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/visplan.hpp"
#include "phys/linemoti.hpp"

class MachPhysVapourPuff : public W4dSprite3d
{
public:
    MachPhysVapourPuff(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MATHEX_SCALAR size, size_t missile_level );
    ~MachPhysVapourPuff();

    static  size_t  nFrames( size_t missile_level );
    static  void preload();

    void startPuff();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysVapourPuff& t );

    PER_MEMBER_PERSISTENT( MachPhysVapourPuff );
	PER_FRIEND_READ_WRITE( MachPhysVapourPuff );

private:
    // Operation deliberately revoked
    MachPhysVapourPuff( const MachPhysVapourPuff& );
    MachPhysVapourPuff& operator =( const MachPhysVapourPuff& );
    bool operator ==( const MachPhysVapourPuff& );

    //Animation id for plans created to model wisp
    enum {WISP_ANIMATION_ID = 1};

    static  W4dScalePlanPtr scalePlanPtr( size_t missile_level );
    static  W4dVisibilityPlanPtr visibilityPlanPtr( size_t missile_level );

    typedef ctl_vector< Ren::MaterialVecPtr > Materials;
    static  W4dMaterialPlanPtr materialPlanPtr( size_t missile_level  );
    static  RenMaterial initialMaterial( size_t missile_level  );
    static  const Materials&  materials( size_t missile_level );
    static  Materials  createMaterials( size_t missile_level );

    static  void        addMaterial(
        Materials* pMaterials,
        const RenTexture& texture,
        MATHEX_SCALAR alpha,
        const RenColour& colour );

    static  const PhysRelativeTime timePerFrame();

	size_t missile_level_;
};

PER_DECLARE_PERSISTENT( MachPhysVapourPuff );

#endif

/* End VAPPUFF.HPP **************************************************/
