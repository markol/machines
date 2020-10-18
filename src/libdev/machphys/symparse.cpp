/*
 * S Y M P A R S E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machphys/symparse.hpp"

//static
size_t  MachPhysSymbolParser::parseConstructionLevel( const string& token )
{
    size_t  result = 0;

    if( token == "LEVEL_1" )
        result = 1;
    else if( token == "LEVEL_2" )
        result = 2;
    else if( token == "LEVEL_3" )
        result = 3;
    else if( token == "LEVEL_4" )
        result = 4;
    else if( token == "LEVEL_5" )
        result = 5;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Illegal level token" );
    }

    POST( 0 < result );

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::ConstructionType	MachPhysSymbolParser::constructionType( const string& token )
{
    MachPhys::ConstructionType  result = MachPhys::BEACON;

    if( token == "HARDWARE_LAB" )
        result = MachPhys::HARDWARE_LAB;
    else if( token == "SMELTER" )
        result = MachPhys::SMELTER;
    else if( token == "FACTORY" )
        result = MachPhys::FACTORY;
    else if( token == "MISSILE_EMPLACEMENT" )
        result = MachPhys::MISSILE_EMPLACEMENT;
	else if( token == "GARRISON" )
        result = MachPhys::GARRISON;
    else if( token == "MINE" )
        result = MachPhys::MINE;
    else if( token == "BEACON" )
        result = MachPhys::BEACON;
    else if( token == "POD" )
        result = MachPhys::POD;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Illegal construction token" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::AggressorSubType      MachPhysSymbolParser::aggressorSubType( const string& token )
{
    MachPhys::AggressorSubType  result = MachPhys::GRUNT;

    if( token == "GRUNT" )
        result = MachPhys::GRUNT;
    else if( token == "ASSASSIN" )
        result = MachPhys::ASSASSIN;
    else if( token == "KNIGHT" )
        result = MachPhys::KNIGHT;
    else if( token == "BALLISTA" )
        result = MachPhys::BALLISTA;
    else if( token == "NINJA" )
        result = MachPhys::NINJA;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::AdministratorSubType  MachPhysSymbolParser::administratorSubType( const string& token )
{
    MachPhys::AdministratorSubType  result = MachPhys::COMMANDER;

    if( token == "BOSS" )
        result = MachPhys::BOSS;
    else if( token == "OVERSEER" )
        result = MachPhys::OVERSEER;
    else if( token == "COMMANDER" )
        result = MachPhys::COMMANDER;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::ConstructorSubType    MachPhysSymbolParser::constructorSubType( const string& token )
{
    MachPhys::ConstructorSubType    result = MachPhys::DOZER;

    if( token == "DOZER" )
        result = MachPhys::DOZER;
    else if( token == "BUILDER" )
        result = MachPhys::BUILDER;
    else if( token == "BEHEMOTH" )
        result = MachPhys::BEHEMOTH;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::TechnicianSubType     MachPhysSymbolParser::technicianSubType( const string& token)
{
    MachPhys::TechnicianSubType result = MachPhys::LAB_TECH;

    if( token == "LAB_TECH" )
        result = MachPhys::LAB_TECH;
    else if( token == "TECH_BOY" )
        result = MachPhys::TECH_BOY;
    else if( token == "BRAIN_BOX" )
        result = MachPhys::BRAIN_BOX;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::FactorySubType            MachPhysSymbolParser::factorySubType( const string& token )
{
    MachPhys::FactorySubType    result = MachPhys::CIVILIAN;

    if( token == "CIVILIAN" )
        result = MachPhys::CIVILIAN;
    else if( token == "MILITARY" )
        result = MachPhys::MILITARY;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::HardwareLabSubType MachPhysSymbolParser::hardwareLabSubType( const string& token )
{
    MachPhys::HardwareLabSubType    result = MachPhys::LAB_CIVILIAN;

    if( token == "CIVILIAN" )
        result = MachPhys::LAB_CIVILIAN;
    else if( token == "MILITARY" )
        result = MachPhys::LAB_MILITARY;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhys::MissileEmplacementSubType MachPhysSymbolParser::missileEmplacementSubType( const string& token )
{
    MachPhys::MissileEmplacementSubType result = MachPhys::TURRET;

    if( token == "TURRET" )
        result = MachPhys::TURRET;
    else if( token == "SENTRY" )
        result = MachPhys::SENTRY;
    else if( token == "LAUNCHER" )
        result = MachPhys::LAUNCHER;
    else if( token == "ICBM" )
        result = MachPhys::ICBM;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Unknown sub type" );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
int MachPhysSymbolParser::voidSubType( const string& token ){
	if( token == "VOID" )
		return 0;
	ASSERT_INFO( token );
	ASSERT( false," Unknown subtype. May have specified incorrect type as first argument in constructionSubType, otherwise should be VOID if legal type has no subtypes.\n" );
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

//static
int MachPhysSymbolParser::constructionSubType( MachPhys::ConstructionType type,  const string& subType )
{
	switch( type )
	{
		case MachPhys::HARDWARE_LAB:
			return hardwareLabSubType( subType );
		case MachPhys::FACTORY:
			return factorySubType( subType );
		case MachPhys::MISSILE_EMPLACEMENT:
			return missileEmplacementSubType( subType );
		default:
			return voidSubType( subType );
	}
}

/* End SCENARIO.CPP *************************************************/
