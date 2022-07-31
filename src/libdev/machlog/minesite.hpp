/*
 * M I N E S I T E. H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
 * MachLogMineralSites is a temporary singleton class
 * used to indicate the status of mining sites in the
 * Milestone 6 Deliverable.
 */

#ifndef _MACHLOG_MINESITE_HPP
#define _MACHLOG_MINESITE_HPP

//#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "machphys/machphys.hpp"

// forward declarations

class MachLogMineralSiteImpl;
class MexPoint3d;

/* //////////////////////////////////////////////////////////////// */

class MachLogMineralSite
{
private:

	enum State { UNDISCOVERED, DISCOVERED };
	PER_FRIEND_ENUM_PERSISTENT( State );

public:

	MachLogMineralSite( const MachPhys::MineralGrade&,
		const MachPhys::BuildingMaterialUnits& amount,
		const MexPoint3d& p);
	// PRE( amountOfOre_ > 0 );
	// PRE( grade_ > 0 and grade < 5 );

	~MachLogMineralSite();

	bool hasBeenDiscoveredBy( MachPhys::Race race ) const;

	bool hasBeenDiscovered() const;

	void beDiscoveredBy( MachPhys::Race race );
	// PRE( not hasBeenDiscoveredBy( race ) );

	const MexPoint3d& position() const;

	MachPhys::Race discoveredByRace() const;

	friend ostream& operator<<( ostream& o, const MachLogMineralSite& t );

	MachPhys::BuildingMaterialUnits amountOfOre() const;

	//bool operator ==( const MachLogMineralSiteImpl& ) const;

	// oreExtracted is the requested value - may not be able to mine this much. This method
	// returns how much ore was actually extracted after the extraction has been done.
	MachPhys::BuildingMaterialUnits extractOre( MachPhys::BuildingMaterialUnits oreExtracted );

	MachPhys::MineralGrade grade() const;

	int id() const;

	PER_MEMBER_PERSISTENT( MachLogMineralSite );
	PER_FRIEND_READ_WRITE( MachLogMineralSite );

	friend class MachLogMineralSiteImpl;

	friend class MachLogMessageBroker;
	friend class MachLogPlanet;

private:

	// Operations deliberately revoked
    MachLogMineralSite( const MachLogMineralSite& );
    MachLogMineralSite& operator =( const MachLogMineralSite& );

	void setOre( MachPhys::BuildingMaterialUnits amountOfOre );

	void id( int idVal );

	// data members
	MachLogMineralSiteImpl*				pImpl_;


};

PER_DECLARE_PERSISTENT( MachLogMineralSite );

/* //////////////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_MINESITE_HPP	*/

/* End MINESITE.HPP *************************************************/
