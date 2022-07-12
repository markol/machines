/*
 * D A T A I M P L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysDataImplementation

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_DATAIMPL_HPP
#define _MACHPHYS_DATAIMPL_HPP

#include "base/base.hpp"
#include "ctl/fixedvec.hpp"

#include "machphys/machphys.hpp"

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

class MachPhysDataImplementation
{
public:
    //  Singleton class
    static MachPhysDataImplementation& instance();
    ~MachPhysDataImplementation();

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
	const MachPhysHardwareLabData& hardwareLabData( MachPhys::HardwareLabSubType subType, size_t hwLevel ) const;
	const MachPhysSmelterData& smelterData( size_t hwLevel ) const;
	const MachPhysFactoryData& factoryData( MachPhys::FactorySubType, size_t hwLevel ) const;
	const MachPhysMissileEmplacementData& missileEmplacementData( MachPhys::MissileEmplacementSubType, size_t hwLevel ) const;
	const MachPhysGarrisonData& garrisonData( size_t hwLevel ) const;
	const MachPhysMineData& mineData( size_t hwLevel ) const;
	const MachPhysBeaconData& beaconData( size_t hwLevel ) const;
	const MachPhysPodData& podData( size_t hwLevel ) const;

	//Specials
	const MachPhysOreHolographData& oreHolographData() const;
    const MachPhysWeaponData& weaponData( MachPhys::WeaponType type ) const;
    const MachPhysGeneralData& generalData();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysDataImplementation& t );

private:
    // Operation deliberately revoked
    MachPhysDataImplementation( const MachPhysDataImplementation& );

    // Operation deliberately revoked
    MachPhysDataImplementation& operator =( const MachPhysDataImplementation& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysDataImplementation& );

    MachPhysDataImplementation();

    enum { MAX_HARDWARE_LEVELS = 5 };
    enum { MAX_SOFTWARE_LEVELS = 5 };
    
    enum { STORE_HARDWARE_SIZE = MAX_HARDWARE_LEVELS + 1 };
    enum { STORE_SOFTWARE_SIZE = MAX_SOFTWARE_LEVELS + 1 };

    friend class MachPhysDataParser;
    
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysAdministratorData* > >      AdministratorDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysAggressorData* > >          AggressorDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysConstructorData* > >        ConstructorDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysGeoLocatorData* > >         GeoLocatorDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysAPCData* > >   APCDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysResourceCarrierData* > >    ResourceCarrierDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysSpyLocatorData* > >         SpyLocatorDataStore;
    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysTechnicianData* > >         TechnicianDataStore;
    
    AdministratorDataStore      administratorBossData_;
    AdministratorDataStore      administratorOverseerData_;
    AdministratorDataStore      administratorCommanderData_;
    AggressorDataStore          aggressorGruntData_;
    AggressorDataStore          aggressorAssassinData_;
    AggressorDataStore          aggressorKnightData_;
    AggressorDataStore          aggressorBallistaData_;
    AggressorDataStore          aggressorNinjaData_;
    ConstructorDataStore        constructorDozerData_;
    ConstructorDataStore        constructorBuilderData_;
    ConstructorDataStore        constructorBehemothData_;
    GeoLocatorDataStore         geoLocatorData_;
    APCDataStore                APCData_;
    ResourceCarrierDataStore    resourceCarrierData_;
    SpyLocatorDataStore         spyLocatorData_;
    TechnicianDataStore         technicianLabTechData_;
    TechnicianDataStore         technicianTechBoyData_;
    TechnicianDataStore         technicianBrainBoxData_;

    typedef ctl_fixed_vector< MachPhysHardwareLabData* >        HardwareLabDataStore;
    typedef ctl_fixed_vector< MachPhysSmelterData* >            SmelterDataStore;
    typedef ctl_fixed_vector< MachPhysMineData* >               MineDataStore;
    typedef ctl_fixed_vector< MachPhysGarrisonData* >           GarrisonDataStore;
    typedef ctl_fixed_vector< MachPhysBeaconData* >             BeaconDataStore;
    typedef ctl_fixed_vector< MachPhysPodData* >                PodDataStore;
    typedef ctl_fixed_vector< MachPhysFactoryData* >            FactoryDataStore;
    typedef ctl_fixed_vector< MachPhysMissileEmplacementData* > MissileEmplacementDataStore;

    HardwareLabDataStore        hardwareLabCivilianData_;
    HardwareLabDataStore        hardwareLabMilitaryData_;
    SmelterDataStore            smelterData_;
    MineDataStore               mineData_;
    GarrisonDataStore           garrisonData_;
    BeaconDataStore             beaconData_;
    PodDataStore                podData_;
    FactoryDataStore            factoryCivilianData_;
    FactoryDataStore            factoryMilitaryData_;
    MissileEmplacementDataStore missileEmplacementTurretData_;
    MissileEmplacementDataStore missileEmplacementSentryData_;
    MissileEmplacementDataStore missileEmplacementLauncherData_;
    MissileEmplacementDataStore missileEmplacementICBMData_;

    typedef ctl_fixed_vector< ctl_fixed_vector< MachPhysMachineData* > >        MachineDataStore;
    typedef ctl_fixed_vector< MachPhysConstructionData* >                       ConstructionDataStore;

 	MachPhysOreHolographData* 	pOreHolographData_;

    const AggressorDataStore* pDataStore( MachPhys::AggressorSubType subType ) const;
    const AdministratorDataStore* pDataStore( MachPhys::AdministratorSubType subType ) const;
    const ConstructorDataStore* pDataStore( MachPhys::ConstructorSubType subType ) const;
    const TechnicianDataStore* pDataStore( MachPhys::TechnicianSubType subType ) const;
    
    AggressorDataStore* pDataStore( MachPhys::AggressorSubType subType );
    AdministratorDataStore* pDataStore( MachPhys::AdministratorSubType subType );
    ConstructorDataStore* pDataStore( MachPhys::ConstructorSubType subType );
    TechnicianDataStore* pDataStore( MachPhys::TechnicianSubType subType );
    
    const FactoryDataStore* pDataStore( MachPhys::FactorySubType subType ) const;
    const HardwareLabDataStore* pDataStore( MachPhys::HardwareLabSubType subType ) const;
    const MissileEmplacementDataStore* pDataStore( MachPhys::MissileEmplacementSubType subType ) const;

    FactoryDataStore* pDataStore( MachPhys::FactorySubType subType );
    HardwareLabDataStore* pDataStore( MachPhys::HardwareLabSubType subType );
    MissileEmplacementDataStore* pDataStore( MachPhys::MissileEmplacementSubType subType );

    //data for weapons. No hw or sw subtypes involved.
    typedef ctl_fixed_vector< MachPhysWeaponData* > WeaponDataStore;
    WeaponDataStore weaponData_;

    MachPhysWeaponData* pWeaponData( MachPhys::WeaponType );
    WeaponDataStore* pWeaponDataStore();

    MachPhysGeneralData* pGeneralData_; //Stores general data
    MachPhysGeneralData** pGeneralDataStore();
};


#endif

/* End DATAIMPL.HPP *************************************************/
