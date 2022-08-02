/*
 * E N T Y I T E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dEntityIter

    Can be used to iterate through an entity and all its descendants including
    composite links and held objects.
    Not fast enough to be used for per-frame operations like rendering.
*/

#ifndef _WORLD4D_ENTYITER_HPP
#define _WORLD4D_ENTYITER_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"


//forward refs
class W4dEntity;

class W4dEntityIter
// Canonical form revoked
{
public:
    //ctor. The node to be iterated down is pEntity
    W4dEntityIter( W4dEntity* pEntity );

    //dtor.
    ~W4dEntityIter();

    //True if iteration has completed
    bool isFinished() const;

    //Advance to next entity
    W4dEntityIter& operator++ ();
    //PRE( not isFinished() );

    //The entity currently refeernced by the iterator
    W4dEntity& operator* ();
    //PRE( not isFinished() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dEntityIter& t );

private:
    W4dEntityIter( const W4dEntityIter& );
    W4dEntityIter& operator =( const W4dEntityIter& );
    bool operator ==( const W4dEntityIter& );

    //data members
    ctl_pvector< W4dEntity > stack_; //The current stack of entities
};


#endif

/* End ENTYITER.HPP *************************************************/
