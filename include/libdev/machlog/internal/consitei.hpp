/*
 * C O N S I T E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogConstructionItemImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CONSITEI_HPP
#define _MACHLOG_CONSITEI_HPP

#include "base/base.hpp"
#include "machlog/machlog.hpp"

class MachLogConstructionItemImpl
// Canonical form revoked
{
public:
    MachLogConstructionItemImpl( void );
    ~MachLogConstructionItemImpl( void );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogConstructionItemImpl );
	PER_FRIEND_READ_WRITE( MachLogConstructionItemImpl );

private:
    friend ostream& operator <<( ostream& o, const MachLogConstructionItemImpl& t );

    MachLogConstructionItemImpl( const MachLogConstructionItemImpl& );
    MachLogConstructionItemImpl& operator =( const MachLogConstructionItemImpl& );

	friend class MachLogConstructionItem;

	MachLog::ObjectType					constructionType_;
	int									subType_;
	size_t								hwLevel_;
	MachPhys::WeaponCombo				weaponCombo_;
	bool								hasWeaponCombo_;
	bool								activated_[ MachPhys::N_RACES ];
	bool								activationLocked_[ MachPhys::N_RACES ];
	MachPhys::BuildingMaterialUnits 	buildingCost_;
};

PER_DECLARE_PERSISTENT( MachLogConstructionItemImpl );

#define CB_MachLogConstructionItem_DEPIMPL() \
	CB_DEPIMPL( MachLog::ObjectType, constructionType_ ); \
	CB_DEPIMPL( int, subType_ ); \
	CB_DEPIMPL( size_t, hwLevel_ ); \
	CB_DEPIMPL( MachPhys::WeaponCombo, weaponCombo_ ); \
	CB_DEPIMPL( bool, hasWeaponCombo_ ); \
	CB_DEPIMPL_ARRAY( bool, activated_ ); \
	CB_DEPIMPL_ARRAY( bool, activationLocked_ ); \
	CB_DEPIMPL( MachPhys::BuildingMaterialUnits, buildingCost_ );

#endif

/* End CONSITEI.HPP *************************************************/
