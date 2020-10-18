/*
 * T Y P E M A P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machgui/typemap.hpp"

/* ////////////////////////////////////////////////////// */

MachLogType::MachLogType( MachLog::ObjectType type )
: type_( type ),
  subtype_( 0 ),
  hardwareLevel_( 0 ),
  isSubtype_( false )
{
	POST( not isSubtype() );
}

MachLogType::MachLogType( MachLog::ObjectType type, int subtype, unsigned hardwareLevel )
: type_( type ),
  subtype_( subtype ),
  hardwareLevel_( hardwareLevel ),
  isSubtype_( true )
{
	POST( isSubtype() );
}

bool MachLogType::isSubtype() const
{
	return isSubtype_;
}

const MachLog::ObjectType&
MachLogType::type() const
{
	return type_;
}

int MachLogType::subtype() const
{
	PRE( isSubtype() );
	return subtype_;
}

unsigned MachLogType::hardwareLevel() const
{
	PRE( isSubtype() );
	return hardwareLevel_;
}

ostream& operator <<( ostream& o, const MachLogType& t )
{
	o << t.type();
	if( t.isSubtype() )
		o << ' ' << t.subtype() << ' ' << t.hardwareLevel();

	return o << std::endl;
}


/* ////////////////////////////////////////////////////// */

MachLogTypeMap::MachLogTypeMap( const MachLogType& type )
: type_( type )
{
	// Intentionally Empty
}

MachLogTypeMap::~MachLogTypeMap()
{
	// Intentionally Empty
}

const MachLogType& MachLogTypeMap::type() const
{
	return type_;
}

void MachLogTypeMap::process()
{
	switch( type_.type() )
	{
		case MachLog::ADMINISTRATOR :
		case MachLog::AGGRESSOR :
		case MachLog::CONSTRUCTOR :
		case MachLog::GEO_LOCATOR :
		case MachLog::SPY_LOCATOR :
		case MachLog::TECHNICIAN :
		case MachLog::APC :
		case MachLog::RESOURCE_CARRIER :
			processMachine();
		break;
		case MachLog::BEACON :
		case MachLog::FACTORY :
		case MachLog::GARRISON :
		case MachLog::HARDWARE_LAB :
		case MachLog::POD :
		case MachLog::MINE :
		case MachLog::MISSILE_EMPLACEMENT :
		case MachLog::SOFTWARE_LAB :
		case MachLog::SMELTER :
			processConstruction();
		break;
		DEFAULT_ASSERT_BAD_CASE( type_.type() );
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeMap::processConstruction()
{
	if( doConstruction() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		switch( type_.type() )
		{
			case MachLog::BEACON :
				doBeacon();
			break;
			case MachLog::FACTORY :
				processFactory();
			break;
			case MachLog::GARRISON :
				doGarrison();
			break;
			case MachLog::HARDWARE_LAB :
				doHardwareLab();
			break;
			case MachLog::POD :
				doPod();
			break;
			case MachLog::MINE :
				doMine();
			break;
			case MachLog::MISSILE_EMPLACEMENT :
				processMissileEmplacement();
			break;
			case MachLog::SOFTWARE_LAB :
				doSoftwareLab();
			break;
			case MachLog::SMELTER :
				doSmelter();
			break;
			DEFAULT_ASSERT_BAD_CASE( type_.type() );
		}
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeMap::processAdministrator()
{
	if( doAdministrator() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::AdministratorSubType subtype = (MachPhys::AdministratorSubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::BOSS :
				doBoss();
			break;
			case MachPhys::COMMANDER :
				doCommander();
			break;
			case MachPhys::OVERSEER :
				doOverseer();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

void MachLogTypeMap::processAggressor()
{
	if( doAggressor() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::AggressorSubType subtype = (MachPhys::AggressorSubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::ASSASSIN :
				doAssassin();
			break;
			case MachPhys::BALLISTA :
				doBallista();
			break;
			case MachPhys::GRUNT :
				doGrunt();
			break;
			case MachPhys::KNIGHT :
				doKnight();
			break;
			case MachPhys::NINJA :
				doNinja();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

void MachLogTypeMap::processConstructor()
{
	if( doConstructor() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::ConstructorSubType subtype = (MachPhys::ConstructorSubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::BEHEMOTH :
				doBehemoth();
			break;
			case MachPhys::BUILDER :
				doBuilder();
			break;
			case MachPhys::DOZER :
				doDozer();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

void MachLogTypeMap::processLocator()
{
	PRE( type_.type() == MachLog::GEO_LOCATOR or type_.type() == MachLog::SPY_LOCATOR );
	if( doLocator() == PROCESS_AS_SUBTYPE )
	{
		switch( type_.type() )
		{
			case MachLog::GEO_LOCATOR :
				doGeoLocator();
			break;
			case MachLog::SPY_LOCATOR :
				doSpyLocator();
			break;
			DEFAULT_ASSERT_BAD_CASE( type_.type() );
		}
	}
}

void MachLogTypeMap::processTechnician()
{
	if( doTechnician() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::TechnicianSubType subtype = (MachPhys::TechnicianSubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::BRAIN_BOX :
				doBrainBoxTechnician();
			break;
			case MachPhys::LAB_TECH :
				doLabTechTechnician();
			break;
			case MachPhys::TECH_BOY :
				doTechBoyTechnician();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

void MachLogTypeMap::processTransporter()
{
	PRE( type_.type() == MachLog::APC or type_.type() == MachLog::RESOURCE_CARRIER );
	if( doTransporter() == PROCESS_AS_SUBTYPE )
	{
		switch( type_.type() )
		{
			case MachLog::APC :
				doPersonnelCarrier();
			break;
			case MachLog::RESOURCE_CARRIER :
				doResourceCarrier();
			break;
			DEFAULT_ASSERT_BAD_CASE( type_.type() );
		}
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeMap::processMachine()
{
	if( doMachine() == PROCESS_AS_SUBTYPE )
	{
		switch( type_.type() )
		{
			case MachLog::ADMINISTRATOR :
				processAdministrator();
			break;
			case MachLog::AGGRESSOR :	
				processAggressor();
			break;
			case MachLog::CONSTRUCTOR :
				processConstructor();
			break;
			case MachLog::GEO_LOCATOR :
			case MachLog::SPY_LOCATOR :
				processLocator();
			break;
			case MachLog::TECHNICIAN :
				processTechnician();
			break;
			case MachLog::APC :
			case MachLog::RESOURCE_CARRIER :
				processTransporter();
			break;
			DEFAULT_ASSERT_BAD_CASE( type_.type() );
		}
	}
}

/* ///////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBeacon()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doFactory()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doGarrison()
{
	return doDefault();
}


MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doHardwareLab()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doPod()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doMine()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doMissileEmplacement()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doSoftwareLab()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doSmelter()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doAdministrator()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBoss()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doOverseer()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doCommander()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doAggressor()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doAssassin()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBallista()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doGrunt()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doKnight()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doNinja()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doConstructor()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doDozer()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBuilder()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBehemoth()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doLocator()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doGeoLocator()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doSpyLocator() 
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doTechnician()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doLabTechTechnician()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doTechBoyTechnician()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doBrainBoxTechnician()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doTransporter()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doResourceCarrier()
{
	return doDefault();
}

MachLogTypeMap::ProcessAsSubtype 
MachLogTypeMap::doPersonnelCarrier()
{
	return doDefault();
}

/* //////////////////////////////////////////////////////////////// */

void MachLogTypeMap::processFactory()
{
	if( doFactory() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::FactorySubType subtype = (MachPhys::FactorySubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::CIVILIAN :
				doCivilianFactory();
			break;
			case MachPhys::MILITARY :
				doMilitaryFactory();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

void MachLogTypeMap::processMissileEmplacement()
{
	if( doMissileEmplacement() == PROCESS_AS_SUBTYPE and type_.isSubtype() )
	{
		MachPhys::MissileEmplacementSubType subtype = (MachPhys::MissileEmplacementSubType)type_.subtype();
		switch( subtype )
		{
			case MachPhys::TURRET :
				doTurret();
			break;
			case MachPhys::SENTRY :
				doSentry();
			break;
			case MachPhys::LAUNCHER :
				doLauncher();
			break;
			case MachPhys::ICBM :
				doICBM();
			break;
			DEFAULT_ASSERT_BAD_CASE( subtype );
		}
	}
}

/* //////////////////////////////////////////////////////////////// */

/* End TYPEMAP.CPP **************************************************/