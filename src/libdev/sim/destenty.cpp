/*
 * D E S T E N T Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/destenty.hpp"

SimDestructW4dEntityEvent::SimDestructW4dEntityEvent
(
    const CtlConstCountedPtr< W4dEntity >& entityPtr, const PhysAbsoluteTime& time
)
:   SimDiscreteEvent( time ),
    entityPtr_( entityPtr )
{

    TEST_INVARIANT;
}

SimDestructW4dEntityEvent::~SimDestructW4dEntityEvent()
{
    TEST_INVARIANT;

    //Nothing to do - the entity will be deleted if the counted ptr holds
    //the only reference to it
}

// virtual
void SimDestructW4dEntityEvent::execute()
{
    //Nothing to do - the entity will be deleted if the counted ptr holds
    //the only reference to it
}

void SimDestructW4dEntityEvent::CLASS_INVARIANT
{
}

//virtual
void SimDestructW4dEntityEvent::doOutputOperator( ostream& o ) const
{
    o << "SimDestructW4dEntityEvent: entity = " << (void*)&(*entityPtr_) << std::endl;
}
/* End DESTENTY.CPP *************************************************/
