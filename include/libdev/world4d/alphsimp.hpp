/*
 * A L P H S I M P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSimpleAlphaPlan

    Defines a material plan in which the alpha value varies with time, and
    a single material is used which remains otherwise unchanged
*/

#ifndef _WORLD4D_ALPHSIMP_HPP
#define _WORLD4D_ALPHSIMP_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"
#include "phys/sclplan.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include "world4d/matplan.hpp"

//forward refs
class RenMaterial;
class W4dEntity;

class W4dSimpleAlphaPlan : public W4dMaterialPlan
// Canonical form revoked
{
public:
    //The single material to be controlled is mat. A non-shareable copy of this is made.
    //The value defined by alphaPlanPtr determines the alpha transparency of the material.
    //A material vector with nMaterialsInVector identical elements will be created.
    //The plan is applied up to and including LOD maxLOD.
    W4dSimpleAlphaPlan( const RenMaterial& mat, uint nMaterialsInVector,
                        const PhysScalarPlanPtr& alphaPlanPtr, W4dLOD maxLOD );
    
    //dtor
    virtual ~W4dSimpleAlphaPlan();

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

	//makes  a W4dSimpleAlphaPlan with alphaPlanPtr, then apply to entity from startTime
	static void makePlan( W4dEntity* pEntity, const PhysAbsoluteTime& startTime, const PhysScalarPlanPtr& alphaPlanPtr, W4dLOD maxLOD );  

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dSimpleAlphaPlan& t );

private:
    W4dSimpleAlphaPlan( const W4dSimpleAlphaPlan& );
    W4dSimpleAlphaPlan& operator =( const W4dSimpleAlphaPlan& );
    bool operator ==( const W4dSimpleAlphaPlan& );

    //data members
    RenMaterial material_; //Non-shareable material to be adjusted
    PhysScalarPlanPtr alphaPlanPtr_; //Scalar plan defining alpha value
    Ren::MaterialVecPtr materialVecPtr_; //The vector of RenMaterials to use
};


#endif

/* End ALPHSIMP.HPP *************************************************/
