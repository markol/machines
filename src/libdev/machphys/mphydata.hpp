/*
 * M P H Y D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysData

	Controls the access to the default machine/constructions/ILF(?) data
	Access may be gained to a specific data elements with the appropriate function call.
*/

#ifndef _MACHPHYS_MPHYDATA_HPP
#define _MACHPHYS_MPHYDATA_HPP

#include "machphys/machphys.hpp"
#include "ctl/fixedvec.hpp"

class MachPhysAggressorData;
class MachPhysAdministratorData;
class MachPhysAPCData;
class MachPhysResourceCarrierData;
class MachPhysGeoLocatorData;
class MachPhysSpyLocatorData;
class MachPhysTechnicianData;
class MachPhysConstructorData;

class MachPhysHardwareLabData;
class MachPhysSmelterData;
class MachPhysFactoryData;
class MachPhysMissileEmplacementData;
class MachPhysGarrisonData;
class MachPhysMineData;
class MachPhysBeaconData;
class MachPhysPodData;

class MachPhysOreHolographData;

class MachPhysMachineData;
class MachPhysConstructionData;
class MachPhysWeaponData;
class MachPhysGeneralData;

class MachPhysData
{
public:
    //  Singleton class
    static MachPhysData& instance();
    ~MachPhysData();

	//Machine Types
	const MachPhysAggressorData& aggressorData( MachPhys::AggressorSubType, size_t hwLevel , size_t swLevel ) const;
	const MachPhysAdministratorData& administratorData( MachPhys::AdministratorSubType, size_t hwLevel , size_t swLevel ) const;
	const MachPhysAPCData& APCData( size_t hwLevel , size_t swLevel ) const;
	const MachPhysResourceCarrierData& resourceCarrierData( size_t hwLevel , size_t swLevel ) const;
	const MachPhysGeoLocatorData& geoLocatorData( size_t hwLevel , size_t swLevel ) const;
	const MachPhysSpyLocatorData& spyLocatorData( size_t hwLevel , size_t swLevel ) const;
	const MachPhysTechnicianData& technicianData( MachPhys::TechnicianSubType, size_t hwLevel , size_t swLevel ) const;
	const MachPhysConstructorData& constructorData( MachPhys::ConstructorSubType, size_t hwLevel , size_t swLevel ) const;

	//Construction Types
	const MachPhysHardwareLabData& hardwareLabData( MachPhys::HardwareLabSubType, size_t hwLevel ) const;
	const MachPhysSmelterData& smelterData( size_t hwLevel ) const;
	const MachPhysFactoryData& factoryData( MachPhys::FactorySubType, size_t hwLevel ) const;
	const MachPhysMissileEmplacementData& missileEmplacementData( MachPhys::MissileEmplacementSubType, size_t hwLevel ) const;
	const MachPhysGarrisonData& garrisonData( size_t hwLevel ) const;
	const MachPhysMineData& mineData( size_t hwLevel ) const;
	const MachPhysBeaconData& beaconData( size_t hwLevel ) const;
	const MachPhysPodData& podData( size_t hwLevel ) const;
    const MachPhysConstructionData& constructionData( MachPhys::ConstructionType type, int subType, int hwLevel ) const;

	//Specials
	const MachPhysOreHolographData& oreHolographData() const;
    const MachPhysWeaponData& weaponData( MachPhys::WeaponType type ) const;
    const MachPhysGeneralData& generalData() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysData& t );

private:
    // Operation deliberately revoked
    MachPhysData( const MachPhysData& );

    // Operation deliberately revoked
    MachPhysData& operator =( const MachPhysData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysData& );

    MachPhysData();

};


#endif

/* End MPHYDATA.HPP *************************************************/
