/*
 * M A T F R M P L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dMaterialFramePlan

    A concrete class of W4dMaterialPlan that defines a vector of (counted pointers to)
    material vectors, a duration, and a frame offset.
    Each material vector in the vector is used in sequence, advancing by one each frame.
    Also has a frame offset, which can be used to offset the frame cycling.
    This plan is only applied at the highest level of detail.
*/

#ifndef _WORLD4D_MATFRMPL_HPP
#define _WORLD4D_MATFRMPL_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "render/render.hpp"
#include "render/texture.hpp"
#include "world4d/matplan.hpp"

//Forward refs
class W4dEntity;
template <class T> class ctl_vector;

//orthodox canonical (revoked)
class W4dMaterialFramePlan : public W4dMaterialPlan
{
public:
    //Collection of (counted pointers to) material vectors
    typedef ctl_vector< Ren::MaterialVecPtr > MaterialVecPtrs;
    typedef CtlCountedPtr< MaterialVecPtrs > MaterialVecPtrsPtr;
    typedef ctl_vector< MaterialVecPtrsPtr > MaterialVecPtrsPtrVec;
	
    //ctor. Each material vector is used in sequence, changing every nFrames frames.
    //The offset applied to each frame is frameOffset. The plan's life time is duration.
    W4dMaterialFramePlan( const MaterialVecPtrsPtr& materialVecPtrsPtr,
                          uint frameOffset,
                          const PhysRelativeTime& duration, uint nFrames = 1, W4dLOD maxLOD=0 );

    //dtor
    virtual ~W4dMaterialFramePlan();

    //True if the plan is defined for level of detail lodId.
    virtual bool isLODDefined( W4dLOD lodId ) const;

    //Adds a plan for the next level of detail, lodId.
    void lodPlan( const MaterialVecPtrsPtr& materialVecPtrsPtr, W4dLOD lodId );
    //PRE( lodId > 0  and  lodId <= maxLOD() )
    //PRE( not isLODDefined( lodId ) )
    //PRE( isLODDefined( lodId - 1 ) )

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId) const;
    //PRE( isLODDefined( lodId ) )

	//make a material plan with the textures provided for the entity if one of the textures of pEntity is
	//found in the ctl_vector<RenTexture>. Hence return true. 
	static bool makePlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures, 
	                     const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, 
	                     uint frameReps = 1, uint startFrame = 0, W4dLOD maxLOD = 0);

	// same as makePlan but do not apply the plan to pEntity
	static W4dMaterialFramePlan* createPlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures, 
	                     const PhysRelativeTime& duration, 
	                     uint frameReps = 1, uint startFrame = 0, W4dLOD maxLOD = 0);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dMaterialFramePlan& t );

private:
    // Operations deliberately revoked
    W4dMaterialFramePlan( const W4dMaterialFramePlan& );
    W4dMaterialFramePlan& operator =( const W4dMaterialFramePlan& );
    bool operator ==( const W4dMaterialFramePlan& );

    //data members
    uint frameOffset_; 
	MaterialVecPtrsPtrVec materialVecPtrsPtrVec_;
	uint nFrames_; 
};

#endif

/* End MATFRMPL.HPP *************************************************/
