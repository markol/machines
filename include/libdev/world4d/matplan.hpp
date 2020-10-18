/*
 * M A T P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dMaterialPlan

    Allows the materials used by a mesh to be modified by implementing a function
    of time which returns a material vector.
    This is an ABC for such classes.
*/

#ifndef _WORLD4D_MATPLAN_HPP
#define _WORLD4D_MATPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/world4d.hpp"

//orthodox canonical (revoked)
class W4dMaterialPlan
{
public:
    //ctor. duration specifies the period of time over which the plan is defined.
    //maxLOD is the highest LOD that can be defined for the plan.
    W4dMaterialPlan( const PhysRelativeTime& duration, W4dLOD maxLOD );
    //PRE( maxLOD >= 0 )

    //dtor.
    virtual ~W4dMaterialPlan();

    //The duration
    const PhysRelativeTime& duration() const;

    //The maximum allowed level of detail
    W4dLOD maxLOD() const;

    //True if the plan is defined for level of detail lodId.
    virtual bool isLODDefined( W4dLOD lodId ) const = 0;

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId ) const = 0;

    //True iff the duration of the plan is <= timeOffset
    bool isDone( const PhysRelativeTime& timeOffset ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dMaterialPlan& t );

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dMaterialPlan );
    PER_FRIEND_READ_WRITE( W4dMaterialPlan );
protected:
    //set the duration of the plan to newDuration
    void duration( const PhysRelativeTime& newDuration);

private:
    // Operation deliberately revoked
    W4dMaterialPlan( const W4dMaterialPlan& );
    W4dMaterialPlan& operator =( const W4dMaterialPlan& );
    bool operator ==( const W4dMaterialPlan& );

    // Data members
    PhysRelativeTime duration_; // The maximum time at which the mapping is defined
    W4dLOD maxLOD_;//The highest LOD that can be defined in the plan
};

PER_DECLARE_PERSISTENT( W4dMaterialPlan );

typedef CtlCountedPtr< W4dMaterialPlan > W4dMaterialPlanPtr;

#ifdef _INLINE
    #include "world4d/matplan.ipp"
#endif

#endif

/* End MATPLAN.HPP **************************************************/
