/*
 * D A T A P A R S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysDataParser

    
	Internal implementation class for MachPhysData. Parse in the parmdata file.
*/

#ifndef _MACHPHYS_DATAPARS_HPP
#define _MACHPHYS_DATAPARS_HPP

#include "machphys/machphys.hpp"
#include "machphys/internal/dataimpl.hpp"

#include "stdlib/string.hpp"

class SysPathName;
class UtlLineTokeniser;
class MachPhysLevelIndexMap;
class MachPhysDataImplementation;


class MachPhysDataParser
{
public:
    //  Singleton class
    static MachPhysDataParser& instance();
    ~MachPhysDataParser();

    void read( const SysPathName& pathname, MachPhysDataImplementation* pData );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysDataParser& t );

private:
    // Operation deliberately revoked
    MachPhysDataParser( const MachPhysDataParser& );

    // Operation deliberately revoked
    MachPhysDataParser& operator =( const MachPhysDataParser& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysDataParser& );

    MachPhysDataParser();

    typedef MachPhysDataImplementation::AggressorDataStore        AggressorDataStore;
    typedef MachPhysDataImplementation::AdministratorDataStore    AdministratorDataStore;
    typedef MachPhysDataImplementation::APCDataStore              APCDataStore;
    typedef MachPhysDataImplementation::ResourceCarrierDataStore  ResourceCarrierDataStore;
    typedef MachPhysDataImplementation::GeoLocatorDataStore       GeoLocatorDataStore;
    typedef MachPhysDataImplementation::SpyLocatorDataStore       SpyLocatorDataStore;
    typedef MachPhysDataImplementation::TechnicianDataStore       TechnicianDataStore;
    typedef MachPhysDataImplementation::ConstructorDataStore      ConstructorDataStore;

    typedef MachPhysDataImplementation::HardwareLabDataStore      HardwareLabDataStore;
    typedef MachPhysDataImplementation::SmelterDataStore          SmelterDataStore;
    typedef MachPhysDataImplementation::MineDataStore             MineDataStore;
    typedef MachPhysDataImplementation::GarrisonDataStore         GarrisonDataStore;
    typedef MachPhysDataImplementation::BeaconDataStore           BeaconDataStore;
    typedef MachPhysDataImplementation::PodDataStore              PodDataStore;
    typedef MachPhysDataImplementation::FactoryDataStore          FactoryDataStore;
    typedef MachPhysDataImplementation::MissileEmplacementDataStore   MissileEmplacementDataStore;

    typedef MachPhysDataImplementation::MachineDataStore          MachineDataStore;
    typedef MachPhysDataImplementation::ConstructionDataStore     ConstructionDataStore;

	void readParameterisedDataFile( const SysPathName& pathname );

	void parseAggressorBlock( UtlLineTokeniser* );
	void parseAdministratorBlock( UtlLineTokeniser* );
	void parseAPCBlock( UtlLineTokeniser* );
	void parseResourceCarrierBlock( UtlLineTokeniser* );
	void parseEPPBlock( UtlLineTokeniser* pParser );
	void parseGeoLocatorBlock( UtlLineTokeniser* );
	void parseSpyLocatorBlock( UtlLineTokeniser* );
	void parseTechnicianBlock( UtlLineTokeniser* );
	void parseConstructorBlock( UtlLineTokeniser* );

	void parseHardwareLabBlock( UtlLineTokeniser* );
	void parseSmelterBlock( UtlLineTokeniser* );
	void parseFactoryBlock( UtlLineTokeniser* );
	void parseMissileEmplacementBlock( UtlLineTokeniser* );
	void parseGarrisonBlock( UtlLineTokeniser* );
	void parseMineBlock( UtlLineTokeniser* );
	void parseBeaconBlock( UtlLineTokeniser* );
	void parsePodBlock( UtlLineTokeniser* );

//	bool isCommonMachineToken( UtlLineTokeniser* );
//	bool isCommonConstructionToken( UtlLineTokeniser* );

 	bool parseCommonMachineToken(
         UtlLineTokeniser*,
         const MachineDataStore& store,
         const MachPhysLevelIndexMap& hardwareLevelIndexMap,
         const MachPhysLevelIndexMap& softwareLevelIndexMap );

    bool parseCommonConstructionToken(
        UtlLineTokeniser* pParser,
        const ConstructionDataStore& store,
        const MachPhysLevelIndexMap& levelIndexMap );

    void parseInteriorBoundary(
        UtlLineTokeniser* pParser,
        const ConstructionDataStore& store,
        const MachPhysLevelIndexMap& levelIndexMap );
    void parseInteriorObstacle(
        UtlLineTokeniser* pParser,
        const ConstructionDataStore& store,
        const MachPhysLevelIndexMap& levelIndexMap );
    void parseInteriorStation(
        UtlLineTokeniser* pParser,
        const ConstructionDataStore& store,
        const MachPhysLevelIndexMap& levelIndexMap );
    
    MachPhys::LocomotionType parseLocomotionType( const string& token );
	bool parseNVGStatus( const string& token );

    MachineDataStore    machineDataStore( const AggressorDataStore& ) const;
    MachineDataStore    machineDataStore( const AdministratorDataStore& ) const;
    MachineDataStore    machineDataStore( const APCDataStore& ) const;
    MachineDataStore    machineDataStore( const ResourceCarrierDataStore& ) const;
    MachineDataStore    machineDataStore( const GeoLocatorDataStore& ) const;
    MachineDataStore    machineDataStore( const SpyLocatorDataStore& ) const;
    MachineDataStore    machineDataStore( const TechnicianDataStore& ) const;
    MachineDataStore    machineDataStore( const ConstructorDataStore& ) const;

    ConstructionDataStore   constructionDataStore( const HardwareLabDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const SmelterDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const MineDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const GarrisonDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const BeaconDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const PodDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const FactoryDataStore& ) const;
    ConstructionDataStore   constructionDataStore( const MissileEmplacementDataStore& ) const;

	void parseWeaponBlock( UtlLineTokeniser*, MachPhys::WeaponType type );
    void parseGeneralData( UtlLineTokeniser* pParser );

    void initialiseDataStores();

    //  Return true iff the data is valid for each construction data in the store
    bool constructionDataValid( const ConstructionDataStore& store ) const;

    MachPhysDataImplementation* pData_;
};


#endif

/* End DATAPARS.HPP *************************************************/
