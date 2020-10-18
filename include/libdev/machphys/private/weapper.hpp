/*
 * W E A P P E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWeaponPersistence

    Perist all of the physical weapon models
*/

#ifndef _MACHPHYS_WEAPPER_HPP
#define _MACHPHYS_WEAPPER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/root.hpp"

#include "machphys/machphys.hpp"

class MachPhysPlasmaWeapon;
class MachPhysFlameThrower;
class MachPhysBolter;
class MachPhysVirusSpreader;
class MachPhysElectroCharger;
class MachPhysMultiLauncher;
class MachPhysPulseRifle;
class MachPhysPulseCannon;
class MachPhysPulseBlob;
class MachPhysTreacheryWeapon;
class MachPhysLargeMissile;
class MachPhysVortexWeapon;
class MachPhysNuclearWeapon;
class MachPhysIonWeapon;
class MachPhysSuperCharger;
class MachPhysPunchWeapon;
class MachPhysObjDemolish;
class MachPhysBeeBomber;
class MachPhysLightStingWeapon;
class MachPhysMetalStingWeapon;
class MachPhysWeaponPersistenceImpl;

class W4dGeneric;

class MachPhysWeaponPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysWeaponPersistence& instance( void );
    ~MachPhysWeaponPersistence( void );

    W4dRoot* pRoot();

    const MachPhysPlasmaWeapon& plasmaWeaponExemplar( MachPhys::WeaponType type );
    const MachPhysFlameThrower& flameThrowerExemplar( MachPhys::WeaponType type );
    const MachPhysBolter& bolterExemplar( MachPhys::WeaponType type );

    const W4dGeneric& bolterFlashExemplar( MachPhys::WeaponType type );

    const MachPhysVirusSpreader& virusSpreaderExemplar( MachPhys::Mounting );
    const MachPhysElectroCharger& electroChargerExemplar( MachPhys::Mounting );
    const MachPhysMultiLauncher& multiLauncherExemplar( MachPhys::WeaponType type );
    const MachPhysPulseRifle& pulseRifleExemplar();
    const MachPhysPulseCannon& pulseCannonExemplar();
	const MachPhysPulseBlob& pulseBlobExemplar(MachPhys::WeaponType type);
    const MachPhysTreacheryWeapon& treacheryWeaponExemplar();
    const MachPhysLargeMissile& largeMissileExemplar();
    const MachPhysVortexWeapon& vortexExemplar();
    const MachPhysNuclearWeapon& nuclearExemplar();
    const MachPhysIonWeapon& ionExemplar(MachPhys::WeaponType type);
    const MachPhysSuperCharger& superChargerExemplar(MachPhys::WeaponType type);
    const MachPhysPunchWeapon& punchExemplar();
	const MachPhysBeeBomber& beeExemplar();
	const MachPhysLightStingWeapon& lightStingExemplar();
	const MachPhysMetalStingWeapon& metalStingExemplar();

    const MachPhysObjDemolish& objDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& hardwareLabDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& smelterDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& factoryDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& missileEmplacementDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& garrisonDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& mineDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& beaconDemolishExemplar( MachPhys::DemolitionType type );
    const MachPhysObjDemolish& podDemolishExemplar( MachPhys::DemolitionType type );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWeaponPersistence& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysWeaponPersistence );
    PER_FRIEND_READ_WRITE( MachPhysWeaponPersistence );

private:
    MachPhysWeaponPersistence( const MachPhysWeaponPersistence& );
    MachPhysWeaponPersistence& operator =( const MachPhysWeaponPersistence& );
    bool operator ==( const MachPhysWeaponPersistence& );

    MachPhysWeaponPersistence( void );

    W4dRoot root_;
    MachPhysWeaponPersistenceImpl* pImpl_;   
};


PER_DECLARE_PERSISTENT( MachPhysWeaponPersistence );

#endif

/* End WEAPPER.HPP **************************************************/
