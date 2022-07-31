/*
 * C O N S I T E M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogConstructionItem

    Describes a construction item (for all races)
*/

#ifndef _MACHLOG_CONSITEM_HPP
#define _MACHLOG_CONSITEM_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"

class MachLogConstructionItemImpl;

class MachLogConstructionItem
{
public:
    MachLogConstructionItem(MachLog::ObjectType consType,
							int subType ,
							size_t hwLevel,
							MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );

    ~MachLogConstructionItem();

	const MachLog::ObjectType&			constructionType() const;
	int									subType() const;
	size_t								hwLevel() const;
	const MachPhys::WeaponCombo			weaponCombo() const;
	bool								hasWeaponCombo() const;
    bool								activated( MachPhys::Race ) const;
	void 								activate( MachPhys::Race );
   	MachPhys::BuildingMaterialUnits 	buildingCost() const;

	void 								activationLocked( MachPhys::Race, bool );
	bool								activationLocked( MachPhys::Race ) const;

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT( MachLogConstructionItem );
	PER_FRIEND_READ_WRITE( MachLogConstructionItem );


    friend ostream& operator <<( ostream& o, const MachLogConstructionItem& t );

private:
	friend class MachLogConstructionTree;
	
    // Operation deliberately revoked
    MachLogConstructionItem( const MachLogConstructionItem& );
    MachLogConstructionItem& operator =( const MachLogConstructionItem& );
    bool operator ==( const MachLogConstructionItem& );

	MachLogConstructionItemImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogConstructionItem );

#endif

/* End CONSITEM.HPP **************************************************/
