/*
 * U V P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _WORLD4D_UVPLAN_HPP
#define _WORLD4D_UVPLAN_HPP

#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/world4d.hpp"

class RenUVTransform;
class MexVec2;

// An abstract base class which defines a texture co-ordinate animation as a
// function of time.
class W4dUVPlan
{
public:
	virtual const Ren::UVTransformPtr& transform(const PhysRelativeTime& t) = 0;
	virtual void print(ostream&) const = 0;
	virtual ~W4dUVPlan();

    const PhysRelativeTime& duration() const;

    //The maximum level of detail at which the transform is to be applied.
    W4dLOD maxLOD() const;

    //True iff the duration of the plan is <= timeOffset
    bool isDone(const PhysRelativeTime& timeOffset) const;

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dUVPlan );
    PER_FRIEND_READ_WRITE( W4dUVPlan );
    
protected:
    //ctor. Only to be applied to meshes up to level of detail maxLOD.
    W4dUVPlan(const PhysRelativeTime& duration, W4dLOD maxLOD);

private:
	PhysRelativeTime duration_;
    W4dLOD maxLOD_;
};

PER_DECLARE_PERSISTENT( W4dUVPlan );

ostream& operator<<(ostream&, const W4dUVPlan&);

template <class T> class CtlCountedPtr;
typedef CtlCountedPtr<W4dUVPlan> W4dUVPlanPtr;

#endif

/* End UVPLAN.HPP ***************************************************/
