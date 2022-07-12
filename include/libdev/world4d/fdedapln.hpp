/*
 * F D E D A P L N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSolidFadedAlphaPlan

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_FDEDAPLN_HPP
#define _WORLD4D_FDEDAPLN_HPP

#include "base/base.hpp"

#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include "world4d/matplan.hpp"

//forward refs
class RenMaterial;

class W4dSolidFadedAlphaPlan  : public W4dMaterialPlan
// Canonical form revoked
{
public:

    W4dSolidFadedAlphaPlan( uint nFadedFrames,
                            uint nSolidFrames,
                            uint nMaterialsInVector,
							const RenMaterial& mat,
                            W4dLOD maxLOD,
							PhysRelativeTime duration,
                            MATHEX_SCALAR minAlpha,
                            MATHEX_SCALAR maxAlpha );

    virtual ~W4dSolidFadedAlphaPlan();

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

private:
    friend ostream& operator <<( ostream& o, const W4dSolidFadedAlphaPlan& t );

    W4dSolidFadedAlphaPlan( const W4dSolidFadedAlphaPlan& );
    W4dSolidFadedAlphaPlan& operator =( const W4dSolidFadedAlphaPlan& );

	bool isFaded() const;

	uint nFadedFrames_;
	uint nFrames_;				  //faded + solid
	uint nMaterialsInVector_;		
    MATHEX_SCALAR minAlpha_;
    MATHEX_SCALAR alphaDifference_;
	Ren::MaterialVecPtr materialVecPtr_;
	RenMaterial material_;
};


#endif

/* End FDEDAPLN.HPP *************************************************/
