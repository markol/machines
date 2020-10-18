/*
 * M I S S H O L D . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/manager.hpp"
#include "world4d/entity.hpp"
#include "world4d/garbcoll.hpp"
#include "machlog/misshold.hpp"
#include "machphys/missile.hpp"

MachLogMissileHolder::MachLogMissileHolder( SimProcess* pProcess, W4dEntity* pPhysObject, const PhysAbsoluteTime& destroyAtTime )
:	SimProjectile( pProcess, pPhysObject ),
	destroyAtTime_ ( destroyAtTime ),
	pPhysMissile_( _STATIC_CAST( MachPhysMissile*, pPhysObject ) )
{

    TEST_INVARIANT;
}

MachLogMissileHolder::~MachLogMissileHolder()
{
    TEST_INVARIANT;

}

//virtual 
PhysRelativeTime MachLogMissileHolder::update( const PhysRelativeTime& ,
                         MATHEX_SCALAR )
{
	pPhysMissile_->update();
    if( not isDead() )
    {
		PhysAbsoluteTime now = SimManager::instance().currentTime();
        if( now > destroyAtTime_ )
        {
            isDead( true );
        }
    }
	return 0.0;
}


void MachLogMissileHolder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMissileHolder& t )
{

    o << "MachLogMissileHolder " << (void*)&t << " start" << std::endl;
    o << "MachLogMissileHolder " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MISSHOLD.CPP *************************************************/
