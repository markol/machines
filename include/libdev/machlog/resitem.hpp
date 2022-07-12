/*
 * R E S I T E M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogResearchItem

    Describes a research item (for all races)
*/

#ifndef _MACHLOG_RESITEM_HPP
#define _MACHLOG_RESITEM_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"
#include "machlog/machine.hpp"

class MachLogResearchTree;
class MachLogResearchItemImpl;

class MachLogResearchItem
{
public:
    MachLogResearchItem( MachLog::ObjectType,
    					int subType,
    					MachLogMachine::Level hwLevel,
    					MachLogMachine::Level swLevel,
    					int technologyLevel,
    					MachPhys::ResearchUnits researchCost,
    					MachPhys::BuildingMaterialUnits buildingCost,
    					MachLogResearchItem* pParentDependancy,
    					MachPhys::HardwareLabSubType hardwareLabSubType,
    					MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );

    ~MachLogResearchItem();

	const MachLog::ObjectType&		objectType() const;
	int									subType() const;
	const MachLogMachine::Level&		hwLevel() const;
	const MachPhys::WeaponCombo			weaponCombo() const;
	bool								hasWeaponCombo() const;
	const MachLogMachine::Level&		swLevel( MachPhys::Race ) const;
	void swLevel( MachPhys::Race race, int level );
	int									technologyLevel() const;
	const MachPhys::ResearchUnits& 		researchCost() const;
	//The cost is overridable - this is to allow certain RSI files to manipulate research times.
	//Theres probably easier ways to do this but this is what they wanted - probably won't even get used
	void								researchCost( const MachPhys::ResearchUnits& );
	const MachPhys::BuildingMaterialUnits& buildingCost() const;
	//Both costs are overridable.
	void								buildingCost( const MachPhys::BuildingMaterialUnits& );
	bool								hasParentDependancy() const;
	MachLogResearchItem& 				parentDependancy() const;
	bool								available( MachPhys::Race ) const;
	bool								researched( MachPhys::Race) const;

	const MachPhys::ResearchUnits& 		swResearchCost( int level ) const;
	const MachPhys::BuildingMaterialUnits& swBuildingCost( int level ) const;
	bool								swAvailable( MachPhys::Race, int level ) const;
	bool								swResearched( MachPhys::Race, int level ) const;
	int									swTechnologyLevel( int level ) const;

	MachPhys::HardwareLabSubType		hardwareLabSubType() const;

	void available( MachPhys::Race , bool );
	void researched( MachPhys::Race, bool );
	void advanceResearch( MachPhys::Race, MachPhys::ResearchUnits );
	MachPhys::ResearchUnits amountResearched( MachPhys::Race );
	MachPhys::BuildingMaterialUnits amountBuilt( MachPhys::Race );
	MachPhys::BuildingMaterialUnits costToAddResearch( MachPhys::ResearchUnits ) const; 

	void swAvailable( MachPhys::Race , int level, bool );
	void swResearched( MachPhys::Race, int level, bool );
	void swAdvanceResearch( MachPhys::Race, int level, MachPhys::ResearchUnits, MachPhys::BuildingMaterialUnits );
	MachPhys::ResearchUnits swAmountResearched( MachPhys::Race, int level );
	MachPhys::BuildingMaterialUnits swAmountBuilt( MachPhys::Race, int level );

	//return how much one of these costs to build in a factory
	MachPhys::BuildingMaterialUnits factoryInstanceCost() const;
	//The ability to override defualt cost (which is gained from parmdata)
	void factoryInstanceCost( MachPhys::BuildingMaterialUnits );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogResearchItem& t );

	enum { N_RESEARCH_RACES = MachPhys::N_RACES };
	enum { MAX_SW_LEVELS = 5 };

	PER_MEMBER_PERSISTENT( MachLogResearchItem );
	PER_FRIEND_READ_WRITE( MachLogResearchItem );

private:
	friend class MachLogResearchTree;
	friend class MachLogResearchItemImpl;

	void swTechnologyLevel( int level, int techLevel );
	void swResearchCost( int level, MachPhys::ResearchUnits );
	void swBuildingCost( int level, MachPhys::BuildingMaterialUnits );

	MachPhys::MachineType mapToMachPhysMachine( MachLog::ObjectType ) const;

	MachLogResearchItem* pParentDependancy() const;
	MachPhys::ResearchUnits amountResearched( MachPhys::Race ) const;
	MachPhys::BuildingMaterialUnits amountBuilt( MachPhys::Race ) const;
	MachPhys::ResearchUnits swAmountResearched( MachPhys::Race, int level ) const;
	MachPhys::BuildingMaterialUnits swAmountBuilt( MachPhys::Race, int level ) const;

    // Operation deliberately revoked
    MachLogResearchItem( const MachLogResearchItem& );
    MachLogResearchItem& operator =( const MachLogResearchItem& );
    bool operator ==( const MachLogResearchItem& );

	MachLogResearchItemImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogResearchItem );

#endif

/* End RESITEM.HPP **************************************************/
