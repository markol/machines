/*
 * S C L P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysScalarPlan  is
   an abstract class. It defines scalar as a function of time.
*/

#ifndef _PHYS_SCLPLAN_HPP
#define _PHYS_SCLPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"

template <class T> class CtlCountedPtr;

class PhysScalarPlan
// Canonical form revoked
{
public:
	PhysScalarPlan();
    PhysScalarPlan( const PhysRelativeTime& duration );
    virtual ~PhysScalarPlan( void );

	const PhysRelativeTime& duration() const; 
	virtual MATHEX_SCALAR scalar(const PhysRelativeTime& timeOffset) const = 0; 

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysScalarPlan& t );

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( PhysScalarPlan );
    PER_FRIEND_READ_WRITE( PhysScalarPlan );
    
protected:
	void duration(const PhysRelativeTime& time);  

private:
    PhysScalarPlan( const PhysScalarPlan& );
    PhysScalarPlan& operator =( const PhysScalarPlan& );
    bool operator ==( const PhysScalarPlan& );

	PhysRelativeTime duration_;
};

PER_DECLARE_PERSISTENT( PhysScalarPlan );

typedef CtlCountedPtr< PhysScalarPlan > PhysScalarPlanPtr;

#endif

/* End SCLPLAN.HPP **************************************************/
