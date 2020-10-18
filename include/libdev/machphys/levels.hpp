/*
 * L E V E L S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysLevels

    This is the master reference for all hardware / software levels -
    everything else should use the functions provided in this class.
*/

#ifndef _MACHPHYS_LEVELS_HPP
#define _MACHPHYS_LEVELS_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machphys/levindmp.hpp"
#include "stdlib/string.hpp"

class MachPhysLevels
{
public:
    //  Singleton class
    static MachPhysLevels& instance( void );
    ~MachPhysLevels( void );

    size_t  nHardwareLevels( MachPhys::MachineType type ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    size_t  nHardwareLevels( MachPhys::AdministratorSubType subType ) const;
    size_t  nHardwareLevels( MachPhys::AggressorSubType subType ) const;
    size_t  nHardwareLevels( MachPhys::ConstructorSubType subType ) const;
    size_t  nHardwareLevels( MachPhys::TechnicianSubType subType ) const;

    size_t  nHardwareLevels( MachPhys::ConstructionType type ) const;
    //  PRE( type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    size_t  nHardwareLevels( MachPhys::FactorySubType subType ) const;
    size_t  nHardwareLevels( MachPhys::HardwareLabSubType subType ) const;
    size_t  nHardwareLevels( MachPhys::MissileEmplacementSubType subType ) const;

    size_t  nSoftwareLevels( MachPhys::MachineType type ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    size_t  nSoftwareLevels( MachPhys::AdministratorSubType subType ) const;
    size_t  nSoftwareLevels( MachPhys::AggressorSubType subType ) const;
    size_t  nSoftwareLevels( MachPhys::ConstructorSubType subType ) const;
    size_t  nSoftwareLevels( MachPhys::TechnicianSubType subType ) const;

    //  Convert an index (which starts at zero and is contiguous) into the
    //  actual hardware level.
    size_t  hardwareLevel( MachPhys::MachineType type, size_t index ) const;
    //  PRE( index < nHardwareLevels( type ) );
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    size_t  hardwareLevel( MachPhys::AdministratorSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::AggressorSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::ConstructorSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::TechnicianSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );

    size_t  hardwareLevel( MachPhys::ConstructionType type, size_t index ) const;
    //  PRE( type == HARDWARE_LAB or type == SOFTWARE_LAB or type == WEAPONS_LAB or type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::FactorySubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::HardwareLabSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );
    size_t  hardwareLevel( MachPhys::MissileEmplacementSubType subType, size_t index ) const;
    //  PRE( index < nHardwareLevels( subType ) );


    //  Convert an index (which starts at zero and is contiguous) into the
    //  actual software level.
    size_t  softwareLevel( MachPhys::MachineType type, size_t index ) const;
    //  PRE( index < nSoftwareLevels( type ) );
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    size_t  softwareLevel( MachPhys::AdministratorSubType subType, size_t index ) const;
    //  PRE( index < nSoftwareLevels( subType ) );
    size_t  softwareLevel( MachPhys::AggressorSubType subType, size_t index ) const;
    //  PRE( index < nSoftwareLevels( subType ) );
    size_t  softwareLevel( MachPhys::ConstructorSubType subType, size_t index ) const;
    //  PRE( index < nSoftwareLevels( subType ) );
    size_t  softwareLevel( MachPhys::TechnicianSubType subType, size_t index ) const;
    //  PRE( index < nSoftwareLevels( subType ) );

    bool    levelValid( MachPhys::MachineType type, size_t hardwareLevel, size_t softwareLevel ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    bool    levelValid( MachPhys::MachineType type, size_t hardwareLevel ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    bool    levelValid( MachPhys::AdministratorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const;
    bool    levelValid( MachPhys::AdministratorSubType subType, size_t hardwareLevel ) const;
    bool    levelValid( MachPhys::AggressorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const;
    bool    levelValid( MachPhys::AggressorSubType subType, size_t hardwareLevel ) const;
    bool    levelValid( MachPhys::ConstructorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const;
    bool    levelValid( MachPhys::ConstructorSubType subType, size_t hardwareLevel ) const;
    bool    levelValid( MachPhys::TechnicianSubType subType, size_t hardwareLevel, size_t softwareLevel ) const;
    bool    levelValid( MachPhys::TechnicianSubType subType, size_t hardwareLevel ) const;

    bool    levelValid( MachPhys::ConstructionType type, size_t hardwareLevel ) const;
    //  PRE( type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    bool    levelValid( MachPhys::FactorySubType subType, size_t hardwareLevel ) const;
    bool    levelValid( MachPhys::HardwareLabSubType subType, size_t hardwareLevel ) const;
    bool    levelValid( MachPhys::MissileEmplacementSubType subType, size_t hardwareLevel ) const;

    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::MachineType type ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::AdministratorSubType subType ) const;
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::AggressorSubType subType ) const;
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::ConstructorSubType subType ) const;
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::TechnicianSubType subType ) const;

    MachPhysLevelIndexMap   softwareLevelIndexMap( MachPhys::MachineType type ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    MachPhysLevelIndexMap   softwareLevelIndexMap( MachPhys::AdministratorSubType subType ) const;
    MachPhysLevelIndexMap   softwareLevelIndexMap( MachPhys::AggressorSubType subType ) const;
    MachPhysLevelIndexMap   softwareLevelIndexMap( MachPhys::ConstructorSubType subType ) const;
    MachPhysLevelIndexMap   softwareLevelIndexMap( MachPhys::TechnicianSubType subType ) const;

    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::ConstructionType type ) const;
    //  PRE( type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::FactorySubType subType ) const;
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::HardwareLabSubType subType ) const;
    MachPhysLevelIndexMap   hardwareLevelIndexMap( MachPhys::MissileEmplacementSubType subType ) const;

    //  The uniqueHardwareIndex and nHardwareIndices functions allow each
    //  hardware level of each type
    //  of machine or construction
    //  to be referenced using a unique index. This makes it possible to set up a
    //  vector for all aggressors and index into that using the aggressor's
    //  subtype, hardware and software levels.

    size_t  uniqueHardwareIndex( MachPhys::MachineType type, size_t hardwareLevel ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    //  PRE( levelValid( type, hardwareLevel ) );
    //  POST( result < nHardwareIndices( type ) );
    size_t  uniqueHardwareIndex( MachPhys::AdministratorSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::ADMINISTRATOR ) );
    size_t  uniqueHardwareIndex( MachPhys::AggressorSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::AGGRESSOR ) );
    size_t  uniqueHardwareIndex( MachPhys::ConstructorSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::CONSTRUCTOR ) );
    size_t  uniqueHardwareIndex( MachPhys::TechnicianSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::TECHNICIAN ) );

    size_t  uniqueHardwareIndex( MachPhys::ConstructionType type, size_t hardwareLevel ) const;
    //  PRE( type == HARDWARE_LAB or type == SOFTWARE_LAB or type == WEAPONS_LAB or type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( type ) );
    size_t  uniqueHardwareIndex( MachPhys::FactorySubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::FACTORY ) );
    size_t  uniqueHardwareIndex( MachPhys::HardwareLabSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::HARDWARE_LAB ) );
    size_t  uniqueHardwareIndex( MachPhys::MissileEmplacementSubType subType, size_t hardwareLevel ) const;
    //  PRE( levelValid( subType, hardwareLevel ) );
    //  POST( result < nHardwareIndices( MachPhys::MISSILE_EMPLACEMENT ) );

    size_t  nHardwareIndices( MachPhys::MachineType type ) const;
    size_t  nHardwareIndices( MachPhys::ConstructionType type ) const;

    //  Return the maximum number of software indices for any machine type
    size_t  nSoftwareIndices( void ) const;

    //  Return the name used to reference this particular type / subtype
    //  as part of the filename for models, icons etc.
    string  name( MachPhys::MachineType type ) const;
    string  name( MachPhys::AdministratorSubType subType ) const;
    string  name( MachPhys::AggressorSubType subType ) const;
    string  name( MachPhys::ConstructorSubType subType ) const;
    string  name( MachPhys::TechnicianSubType subType ) const;

    string  name( MachPhys::ConstructionType type ) const;
    string  name( MachPhys::FactorySubType subType ) const;
    string  name( MachPhys::HardwareLabSubType subType ) const;
    string  name( MachPhys::MissileEmplacementSubType subType ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLevels& t );

private:
    // Operation deliberately revoked
    MachPhysLevels( const MachPhysLevels& );

    // Operation deliberately revoked
    MachPhysLevels& operator =( const MachPhysLevels& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysLevels& );




    void  hardwareLevelData( MachPhys::MachineType type, size_t** pDataArray, size_t* pNElements ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    void  hardwareLevelData( MachPhys::AdministratorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  hardwareLevelData( MachPhys::AggressorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  hardwareLevelData( MachPhys::ConstructorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  hardwareLevelData( MachPhys::TechnicianSubType subType, size_t** pDataArray, size_t* pNElements ) const;

    void  hardwareLevelData( MachPhys::ConstructionType type, size_t** pDataArray, size_t* pNElements ) const;
    //  PRE( type == SMELTER or
    //       type == GARRISON or type == MINE or type == BEACON or type == POD );
    void  hardwareLevelData( MachPhys::FactorySubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  hardwareLevelData( MachPhys::HardwareLabSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  hardwareLevelData( MachPhys::MissileEmplacementSubType subType, size_t** pDataArray, size_t* pNElements ) const;

    void  softwareLevelData( MachPhys::MachineType type, size_t** pDataArray, size_t* pNElements ) const;
    //  PRE( type == GEO_LOCATOR or type == SPY_LOCATOR or
    //       type == RESOURCE_CARRIER or type == APC );
    void  softwareLevelData( MachPhys::AdministratorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  softwareLevelData( MachPhys::AggressorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  softwareLevelData( MachPhys::ConstructorSubType subType, size_t** pDataArray, size_t* pNElements ) const;
    void  softwareLevelData( MachPhys::TechnicianSubType subType, size_t** pDataArray, size_t* pNElements ) const;

    bool  levelFound( size_t level, const size_t* levels, size_t nLevels ) const;

    bool  subTyped( MachPhys::MachineType type ) const;
    bool  subTyped( MachPhys::ConstructionType type ) const;

    MachPhysLevels( void );
};


#endif

/* End LEVELS.HPP ***************************************************/
