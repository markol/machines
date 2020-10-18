/*
 * P R O J E C T I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/projecti.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( SimProjectile )

SimProjectile::SimProjectile( SimProcess* pProcess, W4dEntity* pPhysObject )
: SimActor( pProcess, pPhysObject, UPDATE_EVERY_CYCLE )
{
    PRE( pProcess != NULL );
    PRE( pPhysObject != NULL );

    TEST_INVARIANT;
}

SimProjectile::~SimProjectile()
{
    TEST_INVARIANT;

}

void SimProjectile::CLASS_INVARIANT
{
	INVARIANT( this );
}

ostream& operator <<( ostream& o, const SimProjectile& t )
{

    o << "SimProjectile " << (void*)&t << " start" << std::endl;
    o << "SimProjectile " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const SimProjectile& projectile )
{
	const SimActor& base1 = projectile;

	ostr << base1;
}

void perRead( PerIstream& istr, SimProjectile& projectile )
{
	SimActor& base1 = projectile;

	istr >> base1;
}

SimProjectile::SimProjectile( PerConstructor con )
:	SimActor( con )
{
}

/* End PROJECTI.CPP *************************************************/
