/*
 * E L E C T R O . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogElectro

*/

#ifndef _MACHLOG_ELECTRO_HPP
#define _MACHLOG_ELECTRO_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysElectro;
class MachPhysLinearProjectile;
class MachActor;

class MachLogElectro : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogElectro(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogElectro( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogElectro& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogElectro );
	PER_FRIEND_READ_WRITE( MachLogElectro );

private:
	void doBeDestroyed();

    MachLogElectro( const MachLogElectro& );
    MachLogElectro& operator =( const MachLogElectro& );
    bool operator ==( const MachLogElectro& );

};

PER_DECLARE_PERSISTENT( MachLogElectro );


#endif

/* End PULSEBLB.HPP *************************************************/
