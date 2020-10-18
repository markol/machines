/*
 * D E S T E N T Y . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimDestructW4dEntityEvent

    Intended to perform a scheduled destruction of a W4dEntity. This is implemented
    by storing a CtlCountedPtr to the entity, which simply goes out of scope when
    the event object is deleted immediately on execution. If the reference count
    to the entity is null, it will be destructed by the CtlCountedPtr.
*/

#ifndef _SIM_DESTENTY_HPP
#define _SIM_DESTENTY_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"

#include "sim/disevent.hpp"

//forward refs
class W4dEntity;

//orthodox canonical revoked
class SimDestructW4dEntityEvent : public SimDiscreteEvent
{
public:
    //ctor. entityPtr refs the entity likely to be destroyed when the counted ptr
    //is destructed at time.
    SimDestructW4dEntityEvent( const CtlConstCountedPtr< W4dEntity >& entityPtr,
                               const PhysAbsoluteTime& time );

    //dtor
    ~SimDestructW4dEntityEvent( void );

    virtual void execute();

    void CLASS_INVARIANT;

    virtual void doOutputOperator( ostream& ) const;

private:
    // Operation deliberately revoked
    SimDestructW4dEntityEvent( const SimDestructW4dEntityEvent& );
    SimDestructW4dEntityEvent& operator =( const SimDestructW4dEntityEvent& );
    bool operator ==( const SimDestructW4dEntityEvent& );

    //Data members
    CtlConstCountedPtr< W4dEntity > entityPtr_; //The entity likely to be destructed
};


#endif

/* End DESTENTY.HPP *************************************************/
