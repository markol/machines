/*
 * R E S I T E M I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogResearchItemImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_RESITEMI_HPP
#define _MACHLOG_RESITEMI_HPP

#include "base/base.hpp"
#include "machlog/resitem.hpp"

class MachLogResearchItemImpl
// Canonical form revoked
{
public:
    MachLogResearchItemImpl();
    ~MachLogResearchItemImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogResearchItemImpl );
	PER_FRIEND_READ_WRITE( MachLogResearchItemImpl );

private:
    friend ostream& operator <<( ostream& o, const MachLogResearchItemImpl& t );

    MachLogResearchItemImpl( const MachLogResearchItemImpl& );
    MachLogResearchItemImpl& operator =( const MachLogResearchItemImpl& );

	friend class MachLogResearchItem;

	MachLog::ObjectType					objectType_;
	int									subType_;
	MachLogMachine::Level				hwLevel_;
	MachLogMachine::Level				swLevel_[ MachLogResearchItem::MachLogResearchItem::N_RESEARCH_RACES ];
	int									technologyLevel_;
	int									swTechnologyLevel_[ MachLogResearchItem::MAX_SW_LEVELS ];
	MachPhys::ResearchUnits 			researchCost_;
	MachPhys::BuildingMaterialUnits 	buildingCost_;
	MachPhys::BuildingMaterialUnits 	factoryInstanceCost_;
	MachLogResearchItem* 				pParentDependancy_;
	bool								available_[ MachLogResearchItem::N_RESEARCH_RACES ];
	bool								researched_[ MachLogResearchItem::N_RESEARCH_RACES ];
	MachPhys::ResearchUnits				amountResearched_[ MachLogResearchItem::N_RESEARCH_RACES ];
	MachPhys::BuildingMaterialUnits		amountBuilt_[ MachLogResearchItem::N_RESEARCH_RACES ];
	MachPhys::ResearchUnits 			swResearchCost_[ MachLogResearchItem::MAX_SW_LEVELS ];
	MachPhys::BuildingMaterialUnits 	swBuildingCost_[ MachLogResearchItem::MAX_SW_LEVELS ];
	bool								swAvailable_[ MachLogResearchItem::N_RESEARCH_RACES][ MachLogResearchItem::MAX_SW_LEVELS ];
	bool								swResearched_[ MachLogResearchItem::N_RESEARCH_RACES][ MachLogResearchItem::MAX_SW_LEVELS ];
	MachPhys::ResearchUnits				swAmountResearched_[ MachLogResearchItem::N_RESEARCH_RACES][ MachLogResearchItem::MAX_SW_LEVELS ];
	MachPhys::BuildingMaterialUnits		swAmountBuilt_[ MachLogResearchItem::N_RESEARCH_RACES][ MachLogResearchItem::MAX_SW_LEVELS ];
	MachPhys::HardwareLabSubType		hardwareLabSubType_;
	MachPhys::WeaponCombo				weaponCombo_;
	bool								hasWeaponCombo_;
};

PER_DECLARE_PERSISTENT( MachLogResearchItemImpl );

#define CB_DEPIMPL_2D_ARRAY( vartype, varname, secondDimension ) \
	PRE( pImpl_ ); \
	vartype (*varname)[secondDimension] = pImpl_->varname;


#define CB_MachLogResearchItem_DEPIMPL() \
	CB_DEPIMPL( MachLog::ObjectType					,objectType_ ); \
	CB_DEPIMPL( int									,subType_ ); \
	CB_DEPIMPL( MachLogMachine::Level				,hwLevel_ ); \
	CB_DEPIMPL( int									,technologyLevel_ ); \
	CB_DEPIMPL( MachPhys::ResearchUnits 			,researchCost_ ); \
	CB_DEPIMPL( MachPhys::BuildingMaterialUnits 	,buildingCost_ ); \
	CB_DEPIMPL( MachPhys::BuildingMaterialUnits 	,factoryInstanceCost_ ); \
	CB_DEPIMPL( MachLogResearchItem* 				,pParentDependancy_ ); \ 
	CB_DEPIMPL( MachPhys::HardwareLabSubType		,hardwareLabSubType_ ); \
	CB_DEPIMPL( MachPhys::WeaponCombo				,weaponCombo_ ); \
	CB_DEPIMPL( bool								,hasWeaponCombo_ ); \
	CB_DEPIMPL_ARRAY( MachLogMachine::Level			,swLevel_ ); \
	CB_DEPIMPL_ARRAY( bool							,available_ ); \
	CB_DEPIMPL_ARRAY( bool							,researched_ ); \
	CB_DEPIMPL_ARRAY( int							,swTechnologyLevel_ ); \
	CB_DEPIMPL_ARRAY( MachPhys::ResearchUnits		,amountResearched_ ); \
	CB_DEPIMPL_ARRAY( MachPhys::BuildingMaterialUnits,amountBuilt_ ); \
	CB_DEPIMPL_ARRAY( MachPhys::ResearchUnits 		,swResearchCost_ ); \
	CB_DEPIMPL_ARRAY( MachPhys::BuildingMaterialUnits,swBuildingCost_ ); \
	CB_DEPIMPL_2D_ARRAY( bool							,swAvailable_, MachLogResearchItem::MAX_SW_LEVELS ); \
	CB_DEPIMPL_2D_ARRAY( bool							,swResearched_, MachLogResearchItem::MAX_SW_LEVELS );\ 
	CB_DEPIMPL_2D_ARRAY( MachPhys::ResearchUnits		,swAmountResearched_, MachLogResearchItem::MAX_SW_LEVELS ); \
	CB_DEPIMPL_2D_ARRAY( MachPhys::BuildingMaterialUnits,swAmountBuilt_,MachLogResearchItem::MAX_SW_LEVELS ); 

#endif

/* End RESITEMI.HPP *************************************************/
