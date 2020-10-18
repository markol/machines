/*
 * M A T S E Q P L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dMaterialSequencePlan

    A concrete class of W4dMaterialPlan that defines a vector of (counted pointers to)
    material vectors and a duration. Each material plan in the vector is used in sequence
    for an equal interval.
*/

#ifndef _WORLD4D_MATSEQPL_HPP
#define _WORLD4D_MATSEQPL_HPP

#include "base/base.hpp"
#include "render/texture.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "render/render.hpp"
#include "world4d/matplan.hpp"

//Forward refs
class W4dEntity;

//orthodox canonical (revoked)
class W4dMaterialSequencePlan : public W4dMaterialPlan
{
public:
    //Collection of (counted pointers to) material vectors
    typedef ctl_vector< Ren::MaterialVecPtr > MaterialVecPtrs;

    //ctor. Each element of materialVecPtrs is active in sequence for an equal interval
    //of duration. The highest level of detail for which the plan may be defined is maxLOD.
    //materialVecPtrs defines the plan for the highest LOD, LOD 0.
    W4dMaterialSequencePlan( const MaterialVecPtrs& materialVecPtrs,
                             const PhysRelativeTime& duration, W4dLOD maxLOD );

    //dtor
    virtual ~W4dMaterialSequencePlan();

    //Adds a plan for the next level of detail, lodId.
    void lodPlan( const MaterialVecPtrs& materialVecPtrs, W4dLOD lodId );
    //PRE( lodId > 0  and  lodId <= maxLOD() )
    //PRE( not isLODDefined( lodId ) )
    //PRE( isLODDefined( lodId - 1 ) )

    //True if the plan is defined for level of detail lodId.
    virtual bool isLODDefined( W4dLOD lodId ) const;

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId ) const;
    //PRE( isLODDefined( lodId ) )

	//make a material plan with the textures provided for the entity if one of the textures of pEntity is
	//found in the ctl_vector<RenTexture>. Hence return true. 
	static bool makePlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures, 
	                     const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, uint animId = 0);

	//find the fromtexture from the materials of pEntity, and change it to toTexture, return true if found
	static bool changeTexture( W4dEntity* pEntity, const RenTexture& fromTexture, const RenTexture& toTexture );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dMaterialSequencePlan& t );

private:
    // Operations deliberately revoked
    W4dMaterialSequencePlan( const W4dMaterialSequencePlan& );
    W4dMaterialSequencePlan& operator =( const W4dMaterialSequencePlan& );
    bool operator ==( const W4dMaterialSequencePlan& );

    //data members
    ctl_vector< MaterialVecPtrs > materialVecPtrsVec_; //The sequence of material vectors
                                                       //for each level of detail. 
};

#ifdef _INLINE
    #include "world4d/matseqpl.ipp"
#endif


#endif

/* End MATSEQPL.HPP *************************************************/
