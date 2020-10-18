/*
 * A C T B M P N M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachActorBitmapNames

    Maps from actor type, subtype and hardware level to the icon bitmap name
*/

#ifndef _MACHGUI_ACTBMPNM_HPP
#define _MACHGUI_ACTBMPNM_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"

//static public methods only
class MachActorBitmaps
{
public:
    //The bitmap name
    static string name( MachLog::ObjectType objectType, int subType, int hwLevel, MachPhys::WeaponCombo, MachPhys::Race );
	static string name( MachLog::ObjectType objectType, int subType, int hwLevel, MachPhys::WeaponCombo wc, MachPhys::Race race, bool inFirstPerson );

private:
    // Operation deliberately revoked
    MachActorBitmaps( void );
    ~MachActorBitmaps( void );
    MachActorBitmaps( const MachActorBitmaps& );
    MachActorBitmaps& operator =( const MachActorBitmaps& );
    bool operator ==( const MachActorBitmaps& );

    //Type methods
    static const char* administratorName( MachPhys::AdministratorSubType subType );
    static const char* aggressorName( MachPhys::AggressorSubType subType );
    static const char* geoLocatorName( void );
    static const char* spyLocatorName( void );
    static const char* constructorName( MachPhys::ConstructorSubType subType );
    static const char* technicianName( MachPhys::TechnicianSubType subType );
    static const char* resourceCarrierName( void );
    static const char* apcName( void );
    static const char* beaconName( void );
    static const char* podName( void );
    static const char* hwLabName( MachPhys::HardwareLabSubType subType );
    static const char* swLabName( void );
    static const char* smelterName( void );
    static const char* factoryName( MachPhys::FactorySubType subType );
    static const char* missileEmplacementName( MachPhys::MissileEmplacementSubType subType );
    static const char* garrisonName( void );
    static const char* mineName( void );
	static void appendHwLevel( string&, int hwLevel );
	static void appendWeaponCombo( string&,  MachPhys::WeaponCombo );
};


#endif

/* End ACTBMPNM.HPP *************************************************/
