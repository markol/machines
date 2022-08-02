/*
 * E F F E C T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysEffects

    A utility class for implementing various effects
*/

#ifndef _MACHPHYS_EFFECTS_HPP
#define _MACHPHYS_EFFECTS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "world4d/matplan.hpp"

//forward refs
class RenMaterial;
class RenTexture;
class RenColour;
class W4dEntity;
class W4dMaterialSequencePlan;
template< class T > class ctl_vector;

class MachPhysEffects
// Canonical form revoked
{
public:

    //Apply a material plan to pEntity and all its descendants over the period
    //defined by startTime and duration, such that all meshes just use mat.
    //The plan is applied to all levels of detail up to and including maxLOD
    static void flashObject( W4dEntity* pEntity, const RenMaterial& mat,
                             const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,
                             W4dLOD maxLOD );

    //A solid white material
    static const RenMaterial& solidWhiteMaterial();

    typedef ctl_vector< RenTexture > Textures;

    //A vector of widely used smoke textures
    static const Textures& smokeTextures();

    //returns a material sequence plan based on smokeTextures() with material colour
    //defined by the enumeration
    enum SmokeColour { GREY, BLACK, WHITE, PURPLE, YELLOW, RUST, CIVILIAN_WHITE, MILITARY_RED };
    static const W4dMaterialPlanPtr& smokeMaterialPlan( SmokeColour smokeColour );

    //returns a material sequence plan based on the 3 sparks textures
    static const W4dMaterialPlanPtr& sparksMaterialPlan();

    //A vector of flame ball textures
    static const Textures& flameBallTextures();

    //A material sequence plan based on the flameball textures
    static const W4dMaterialPlanPtr& flameBallMaterialPlan();

    //A vector of flame (fire) textures
    static const Textures& flameTextures();

    //constructs a new material sequence plan based on bseMaterial,
    //but cycling through the textures.
    //Each RenMaterialVec contains nMaterialsPerVector copies of the material.
    //Plan duration is supplied. Also maximum level of detail
    static W4dMaterialSequencePlan* createMaterialSequencePlan
    (
        const RenMaterial& baseMaterial, const Textures& textures,
        uint nMaterialsPerVector,
        const PhysAbsoluteTime& duration, W4dLOD maxLOD
    );

private:
    MachPhysEffects();
    ~MachPhysEffects();
    MachPhysEffects( const MachPhysEffects& );
    MachPhysEffects& operator =( const MachPhysEffects& );
    bool operator ==( const MachPhysEffects& );

    //Creates a new material plan based on the smoke textures and colour
    static W4dMaterialPlan* createSmokeMaterialPlan( const RenColour& colour );

    //Create new material plan using the sparks textures
    static W4dMaterialPlan* createSparksMaterialPlan();

    //Creates a new material plan based on the flameBall textures and colour
    static W4dMaterialPlan* createFlameBallMaterialPlan();
};

#endif

/* End EFFECTS.HPP **************************************************/
