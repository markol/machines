/*
 * M A C H D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMachineData
	This class is the base class for each machine type specific
	data class. This class contains all aspects common to ALL machines


*/

#ifndef _MACHPHYS_MACHDATA_HPP
#define _MACHPHYS_MACHDATA_HPP

#include "machphys/objdata.hpp"
#include "mathex/angle.hpp"
#include "stdlib/string.hpp"
#include "machphys/epp.hpp"

class MachPhysMachineData : public MachPhysObjectData
{
public:
    virtual ~MachPhysMachineData( void );
	//Get methods are public
	MachPhys::ResearchUnits hwResearchCost( void ) const;
	MachPhys::ResearchUnits swResearchCost( void ) const;
	MachPhys::BuildingMaterialUnits hwCost( void ) const;
	MachPhys::BuildingMaterialUnits swCost( void ) const;
	MachPhys::SizeUnits volume( void ) const;
	MATHEX_SCALAR speed( void ) const;
	MATHEX_SCALAR taskSpeed( void ) const;
	MATHEX_SCALAR acceleration( void ) const;
	MexRadians rotationSpeed( void ) const;
	MexRadians rotationAcceleration( void ) const;
	MachPhys::RepairRateUnits repairRate( void ) const;
	MATHEX_SCALAR scannerRange( void ) const;
    MachPhys::MachineType machineType() const;
    int subType() const;
    uint hwLevel() const;
    uint swLevel() const;
    MachPhys::LocomotionType locomotionType() const;
    MATHEX_SCALAR locomotionParameter() const;
	void setEPP( const string& EPPName );
	const MachPhysEvasionPriorityPlan& EPP() const;
	int minAccessSizeIndex() const;
    const string& upperBodyTurnLinkName() const;
    const MexRadians& upperBodyTurnRate() const;
    MATHEX_SCALAR highClearance() const;
    MATHEX_SCALAR lowClearance() const;
	bool hasNVG() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachineData& t );

private:
	friend class MachPhysAggressorData;
	friend class MachPhysAdministratorData;
	friend class MachPhysConstructorData;
	friend class MachPhysGeoLocatorData;
	friend class MachPhysSpyLocatorData;
	friend class MachPhysTechnicianData;
	friend class MachPhysAPCData;
	friend class MachPhysResourceCarrierData;
	friend class MachPhysDataParser;
    // Operations deliberately revoked
    MachPhysMachineData( void );
    MachPhysMachineData( const MachPhysMachineData& );
    MachPhysMachineData& operator =( const MachPhysMachineData& );
    bool operator ==( const MachPhysMachineData& );

	//Set Methods are all private
	void hwResearchCost( const MachPhys::ResearchUnits& );
	void swCost( const MachPhys::BuildingMaterialUnits& );
	void hwCost( const MachPhys::BuildingMaterialUnits& );
	void swResearchCost( const MachPhys::ResearchUnits& );
	void volume( const MachPhys::SizeUnits& );
	void speed( const MATHEX_SCALAR& );
	void taskSpeed( const MATHEX_SCALAR& );
	void acceleration( const MATHEX_SCALAR& );
	void rotationSpeed( const MexRadians& );
	void rotationAcceleration( const MexRadians& );
	void repairRate( const MachPhys::RepairRateUnits& );
	void scannerRange( const MATHEX_SCALAR& );
    void machineType( MachPhys::MachineType type );
    void subType( int type );
    void hwLevel( uint level );
    void swLevel( uint level );
    void locomotionType( MachPhys::LocomotionType type );
    void locomotionParameter( MATHEX_SCALAR v );
	void minAccessSizeIndex( int size );
    void upperBodyTurnLinkName( const string& name );
    void upperBodyTurnRate(  const MexRadians& rate );
    void highClearance( MATHEX_SCALAR clearance );
    void lowClearance( MATHEX_SCALAR clearance );
	void hasNVG( bool status );

	MachPhys::ResearchUnits			hwResearchCost_;
	MachPhys::BuildingMaterialUnits	swCost_;
	MachPhys::BuildingMaterialUnits	hwCost_;
	MachPhys::ResearchUnits			swResearchCost_;
	MachPhys::SizeUnits				volume_;
	MATHEX_SCALAR					speed_;
	MATHEX_SCALAR					taskSpeed_;
	MATHEX_SCALAR					acceleration_;
	MexRadians						rotationSpeed_;
	MexRadians						rotationAcceleration_;
	MachPhys::RepairRateUnits		repairRate_;
	MATHEX_SCALAR					scannerRange_;
    MachPhys::MachineType           machineType_;
    int                             subType_;
    uint                            hwLevel_;
    uint                            swLevel_;
    MachPhys::LocomotionType        locomotionType_; //Wheels, tracks etc
    MATHEX_SCALAR                   locomotionParameter_; //Associated value (eg wheel radius)
	MachPhysEvasionPriorityPlan*	pEPP_;		// cached pointer to assoc'd Evasion Priority Plan within EPPs singleton.
	int 							minAccessSizeIndex_;
    string                          upperBodyTurnLinkName_;
    MexRadians                      upperBodyTurnRate_;
    MATHEX_SCALAR                   highClearance_; //Pathfinding clearance higher value
    MATHEX_SCALAR                   lowClearance_; //Pathfinding clearance lower value
	bool							hasNVG_;	//Capability of using night vision goggles. And of wearing non-virtuous gloves.
};


#endif

/* End MACHDATA.HPP *************************************************/
