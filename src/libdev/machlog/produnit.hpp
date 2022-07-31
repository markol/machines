/*
 * P R O D U N I T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogProductionUnit

    This class contains enough information for the manufacturing processes to
	continue with a type of object.
*/

#ifndef _MACHLOG_PRODUNIT_HPP
#define _MACHLOG_PRODUNIT_HPP

#include "base/base.hpp"
#include "base/tracker.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/machphys.hpp"

#include "machlog/machlog.hpp"


class MachLogProductionUnit
{
public:
	static const int UNCHECKED_PRIORITY;

    MachLogProductionUnit();
    MachLogProductionUnit( MachLog::ObjectType, int subType, int hwLevel, int swLevel, int priority );
	MachLogProductionUnit( const MachLogProductionUnit& );

//Note that both get and set methods are public.
	MachLog::ObjectType type() const;
	void type( MachLog::ObjectType );

	int subType() const;
	void subType( int );

	int hwLevel() const;
	void hwLevel( int );

	int swLevel() const;
	void swLevel( int );

	int priority() const;
	void priority( int );

	MachPhys::BuildingMaterialUnits amountBuilt() const;
	void amountBuilt( MachPhys::BuildingMaterialUnits );

//If a thing needs to be at a specific point (e.g. Buildings then store transfrom as well)
	const MexTransform3d& globalTransform() const;
	void globalTransform( const MexTransform3d& );

//Only certain machine/building types need a weapon combo so not in constructor.
	MachPhys::WeaponCombo weaponCombo() const;
	void weaponCombo( MachPhys::WeaponCombo );

//Only constructions have a construction ID so not in constructor.
	int constructionId() const;
	void constructionId( int );

//Only constructions have a rebuild flag so not in constructor.
	bool needRebuild() const;
	void needRebuild( bool );

    ~MachLogProductionUnit();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogProductionUnit& t );
	bool operator==( const MachLogProductionUnit&);
    friend bool operator< ( const MachLogProductionUnit& lhs, const MachLogProductionUnit& rhs );
    friend bool operator> ( const MachLogProductionUnit& lhs, const MachLogProductionUnit& rhs );

	PER_MEMBER_PERSISTENT_DEFAULT( MachLogProductionUnit );
	PER_FRIEND_READ_WRITE( MachLogProductionUnit );

private:
	OBJECT_TRACKER( MachLogProductionUnit );

	MachLog::ObjectType		type_;
	int						subType_;
	int						hwLevel_;
	int						swLevel_;
	int						priority_;
	MachPhys::BuildingMaterialUnits	amountBuilt_;
	MexTransform3d			globalTransform_;
	MachPhys::WeaponCombo	weaponCombo_;
	int						constructionId_;
	bool					needRebuild_;

};

PER_DECLARE_PERSISTENT( MachLogProductionUnit );

//	This goes after the end of the class declaration
#pragma off(unreferenced);
//EXISTS( MachLogProductionUnit );
#pragma on(unreferenced);


#endif

/* End PRODUNIT.HPP *************************************************/
