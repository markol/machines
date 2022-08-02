/*
 * T Y P E M A P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_TYPEMAP_HPP
#define _MACHLOG_TYPEMAP_HPP

#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"

//////////////////////////////////////////////////////////////////////

class MachLogType
// memberwise cannonical
{
public:

	MachLogType( MachLog::ObjectType type );
	// POST( not isSubtype() );

	MachLogType( MachLog::ObjectType type, int subtype, unsigned hardwareLevel );
	// POST( isSubtype() );

	bool isSubtype() const;

	const MachLog::ObjectType& type() const;

	int subtype() const;
	// PRE( isSubtype() );

	unsigned hardwareLevel() const;
	// PRE( isSubtype() );

private:

	MachLog::ObjectType type_;
	int					subtype_;
	unsigned			hardwareLevel_;
	bool				isSubtype_;

	friend ostream& operator <<( ostream&, const MachLogType& );
};

//////////////////////////////////////////////////////////////////////

class MachLogTypeMap
// cannonical form revoked
{
public:

	MachLogTypeMap( const MachLogType& );

	virtual ~MachLogTypeMap();

	void process();

protected:
 
	enum ProcessAsSubtype
	{
		PROCESS_AS_SUBTYPE = true, 
		DO_NOT_PROCESS_AS_SUBTYPE = false 
	};

	const MachLogType& type() const;
	
	///////////////////////////////
		
	virtual ProcessAsSubtype doConstruction() = 0;
	virtual ProcessAsSubtype doMachine() = 0;
	virtual ProcessAsSubtype doDefault() = 0;
	
	///////////////////////////////
							
	virtual ProcessAsSubtype doBeacon();
	virtual ProcessAsSubtype doFactory();
	virtual ProcessAsSubtype doCivilianFactory() = 0;
	virtual ProcessAsSubtype doMilitaryFactory() = 0;

	virtual ProcessAsSubtype doGarrison();
	virtual ProcessAsSubtype doHardwareLab();
	virtual ProcessAsSubtype doPod();
	virtual ProcessAsSubtype doMine();

	virtual ProcessAsSubtype doMissileEmplacement();
	virtual ProcessAsSubtype doTurret() = 0;
	virtual ProcessAsSubtype doSentry() = 0;
	virtual ProcessAsSubtype doLauncher() = 0;
	virtual ProcessAsSubtype doICBM() = 0;

	virtual ProcessAsSubtype doSoftwareLab();
	virtual ProcessAsSubtype doSmelter();

	///////////////////////////////
	
	virtual ProcessAsSubtype doAdministrator();
	virtual ProcessAsSubtype doBoss() = 0;
	virtual ProcessAsSubtype doOverseer() = 0;
	virtual ProcessAsSubtype doCommander() = 0;

	virtual ProcessAsSubtype doAggressor();
	virtual ProcessAsSubtype doAssassin();
	virtual ProcessAsSubtype doBallista() = 0;
	virtual ProcessAsSubtype doGrunt();
	virtual ProcessAsSubtype doKnight();
	virtual ProcessAsSubtype doNinja() = 0;

	virtual ProcessAsSubtype doConstructor();
	virtual ProcessAsSubtype doDozer() = 0;
	virtual ProcessAsSubtype doBuilder() = 0;
	virtual ProcessAsSubtype doBehemoth() = 0;
	
	virtual ProcessAsSubtype doLocator();
	virtual ProcessAsSubtype doGeoLocator() = 0;
	virtual ProcessAsSubtype doSpyLocator() = 0;
	
	virtual ProcessAsSubtype doTechnician();
	virtual ProcessAsSubtype doLabTechTechnician() = 0;
	virtual ProcessAsSubtype doTechBoyTechnician() = 0;
	virtual ProcessAsSubtype doBrainBoxTechnician() = 0;

	virtual ProcessAsSubtype doTransporter();
	virtual ProcessAsSubtype doPersonnelCarrier();
	virtual ProcessAsSubtype doResourceCarrier();
		
	///////////////////////////////

private:
	MachLogTypeMap( const MachLogTypeMap& );
	MachLogTypeMap& operator =( const MachLogTypeMap& );
	bool operator ==( const MachLogTypeMap& );
	
	///////////////////////////////

	void processMachine();
	void processAdministrator();
	void processAggressor();
	void processConstructor();
	void processLocator();
	void processTechnician();
	void processTransporter();
	
	///////////////////////////////

	void processConstruction();
	void processFactory();
	void processMissileEmplacement();

	///////////////////////////////

	MachLogType	type_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _MACHLOG_TYPEMAP_HPP

/* End TYPEMAP.HPP **************************************************/