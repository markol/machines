/*
 * S C A L P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dScalePlan

    ABC for scale plans, which define a RenScale as a function of time
*/

#ifndef _WORLD4D_SCALPLAN_HPP
#define _WORLD4D_SCALPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

//Forward refs
template <class T> class CtlCountedPtr;
class MexTransform3d;
class RenUniformScale;
class RenNonUniformScale;

//orthodox canonical( revoked )
class W4dScalePlan
{
public:
    //dtor
    virtual ~W4dScalePlan();

    //the plan's duration
    const PhysRelativeTime& duration() const;
    
    //true iff the scale being applied is non-uniform
    virtual bool isNonUniform() const = 0;

    //return the defined uniform scale at timeOffset in pScale
    void scale( const PhysRelativeTime& timeOffset, RenUniformScale* pScale ) const;
    //PRE( not isNonUniform() )

    //return the defined non-uniform scale at timeOffset in pScale
    void scale( const PhysRelativeTime& timeOffset, RenNonUniformScale* pScale ) const;
    //PRE( isNonUniform() )

    //Return a new plan allocated on the heap but modified to take account of the offset
    //transform.
    W4dScalePlan* transformClone( const MexTransform3d& offsetTransform ) const;
    //PRE( isNonUniform() )

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const W4dScalePlan& t );

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dScalePlan );
    PER_FRIEND_READ_WRITE( W4dScalePlan );
    
protected:
    //ctor
    W4dScalePlan( const PhysRelativeTime& duration );

private:
    // Operations deliberately revoked
    W4dScalePlan( const W4dScalePlan& );
    W4dScalePlan& operator =( const W4dScalePlan& );
    bool operator ==( const W4dScalePlan& );

    //return the defined uniform scale at timeOffset in pScale
    virtual void doScale( const PhysRelativeTime& timeOffset, RenUniformScale* pScale ) const = 0;

    //return the defined non-uniform scale at timeOffset in pScale
    virtual void doScale( const PhysRelativeTime& timeOffset, RenNonUniformScale* pScale ) const = 0;

    //Return a new plan allocated on the heap but modified to take account of the offset
    //transform.
    virtual W4dScalePlan* doTransformClone( const MexTransform3d& offsetTransform ) const = 0;

    //data members
    PhysRelativeTime duration_;
};

PER_DECLARE_PERSISTENT( W4dScalePlan );

typedef CtlCountedPtr< W4dScalePlan > W4dScalePlanPtr;

#ifdef _INLINE
    #include "world4d/scalplan.ipp"
#endif


#endif

/* End SCALPLAN.HPP *************************************************/
