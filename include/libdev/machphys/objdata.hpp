/*
 * O B J D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysObjectData

    Base class for parmeterised object data
	classes such as MachPhysMachineConstructionData will derive from this.
*/

#ifndef _OBJDATA_HPP
#define _OBJDATA_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"


class MachPhysObjectData
{
public:
    virtual ~MachPhysObjectData( void );

	//get methods are public
	MachPhys::ArmourUnits armour( void ) const;
	MachPhys::HitPointUnits hitPoints( void ) const;
	MachPhys::BuildingMaterialUnits cost( void ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysObjectData& t );


private:
	friend class MachPhysDataParser;
	friend class MachPhysMachineData;
	friend class MachPhysConstructionData;
	friend class MachPhysOreHolographData;
	friend class MachPhysArtefactData;
    // Operations deliberately revoked
    MachPhysObjectData( const MachPhysObjectData& );
    MachPhysObjectData& operator =( const MachPhysObjectData& );
    bool operator ==( const MachPhysObjectData& );
    MachPhysObjectData( void );

	//set methods are private
	void armour( const MachPhys::ArmourUnits& );
	void hitPoints( const MachPhys::HitPointUnits& );
	void cost( const MachPhys::BuildingMaterialUnits& );

	MachPhys::ArmourUnits			armour_;
	MachPhys::HitPointUnits			hitPoints_;
	MachPhys::BuildingMaterialUnits	cost_;
};

#endif

/* End OBJDATA.HPP **************************************************/
