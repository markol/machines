/*
 * S I M P L E E M C O L P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSimpleEmColPlan

    Implement a mapping from time to a visibility flag
*/

#ifndef _WORLD4D_EMCOLPLAN_HPP
#define _WORLD4D_EMCOLPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/countptr.hpp"

#include "ctl/vector.hpp"

#include "phys/phys.hpp"

#include "render/render.hpp"

#include "render/material.hpp"

#include "world4d/colodata.hpp"
#include "world4d/matplan.hpp"

class RenMaterial;

class W4dSimpleEmColPlan : public W4dMaterialPlan
{
public:
    //  Specify the visibility at time zero
    W4dSimpleEmColPlan( const RenMaterial& mat, uint nMaterialsInVector,
    const W4dColourPlanDataPtr& colourDataPtr, W4dLOD maxLOD );

    virtual ~W4dSimpleEmColPlan( void );

    virtual bool isLODDefined( W4dLOD lodId ) const;
    
    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId ) const;

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    W4dSimpleEmColPlan( const W4dSimpleEmColPlan& );

    // Operation deliberately revoked
    W4dSimpleEmColPlan& operator =( const W4dSimpleEmColPlan& );

    // Operation deliberately revoked
    bool operator ==( const W4dSimpleEmColPlan& );

    RenMaterial material_; //Non-shareable material to be adjusted
    W4dColourPlanDataPtr colourPlanPtr_; // Collection defining color value value
    Ren::MaterialVecPtr materialVecPtr_; // The vector of RenMaterials to use
};

#endif

/* End EMCOLPLAN.HPP **************************************************/
