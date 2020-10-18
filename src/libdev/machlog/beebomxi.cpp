/*
 * B E E B O M X I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/beebomxi.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"


MachLogBeeBombExplosionImpl::MachLogBeeBombExplosionImpl( const MachPhysWeaponData* const pWeaponData )
:	pWeaponData_( pWeaponData ),
	pPhysBeeBombExplosion_( NULL )
{
    TEST_INVARIANT;
}

MachLogBeeBombExplosionImpl::~MachLogBeeBombExplosionImpl()
{
    TEST_INVARIANT;

}

void MachLogBeeBombExplosionImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogBeeBombExplosionImpl& t )
{

    o << "MachLogBeeBombExplosionImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogBeeBombExplosionImpl " << (void*)&t << " end" << std::endl;

    return o;
}




/* End BEEBOMXI.CPP *************************************************/
