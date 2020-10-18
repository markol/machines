/*
 * A L P H S I M P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSimpleIntensityPlan

    Defines a material plan in which the alpha value varies with time, and
    a single material is used which remains otherwise unchanged
*/

#ifndef _WORLD4D_SIMINTPL_HPP
#define _WORLD4D_SIMINTPL_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"
#include "phys/sclplan.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include "world4d/matplan.hpp"

//forward refs
class RenMaterial;


class W4dSimpleIntensityPlan : public W4dMaterialPlan
// Canonical form revoked
{
public:
    //The single material to be controlled is mat. A non-shareable copy of this is made.
    //The value defined by intensityPlanPtr determines the intensity  of the material.
    //A material vector with nMaterialsInVector identical elements will be created.
    //The plan is applied up to and including LOD maxLOD.
    W4dSimpleIntensityPlan( const RenMaterial& mat, uint nMaterialsInVector,
                        const PhysScalarPlanPtr& intensityPlanPtr, W4dLOD maxLOD );
    
    //dtor
    virtual ~W4dSimpleIntensityPlan();

    ///////////////////////////////////////////////////
    //Inherited from W4dMaterialPlan

    //True if the plan is defined for level of detail lodId.
    virtual bool isLODDefined( W4dLOD lodId ) const;

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId ) const;
    //PRE( isLODDefined( lodId ) );
    ///////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dSimpleIntensityPlan& t );

private:
    W4dSimpleIntensityPlan( const W4dSimpleIntensityPlan& );
    W4dSimpleIntensityPlan& operator =( const W4dSimpleIntensityPlan& );
    bool operator ==( const W4dSimpleIntensityPlan& );

    //data members
    RenMaterial material_; //Non-shareable material to be adjusted
    PhysScalarPlanPtr intensityPlanPtr_; //Scalar plan defining intensity value
    Ren::MaterialVecPtr materialVecPtr_; //The vector of RenMaterials to use
};


#endif

/* End ALPHSIMP.HPP *************************************************/
