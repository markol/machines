/*
 * P R O J E C T I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimProjectile

    A brief description of the class should go in here
*/

#ifndef _SIM_PROJECTI_HPP
#define _SIM_PROJECTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "sim/actor.hpp"

class SimProjectile : public SimActor
{
public:
    SimProjectile( SimProcess* pProcess, W4dEntity* pPhysObject );
    ~SimProjectile( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimProjectile& t );

    PER_MEMBER_PERSISTENT_ABSTRACT( SimProjectile );

private:
    // Operation deliberately revoked
    SimProjectile( const SimProjectile& );

    // Operation deliberately revoked
    SimProjectile& operator =( const SimProjectile& );

    // Operation deliberately revoked
    bool operator ==( const SimProjectile& );

};

PER_DECLARE_PERSISTENT( SimProjectile );
PER_READ_WRITE( SimProjectile );


#endif

/* End PROJECTI.HPP *************************************************/
