/*
 * F A C T O R Y I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogFactoryImpl

    Implementation class for logical factories.
*/

#ifndef _MACHLOG_FACTORYI_HPP
#define _MACHLOG_FACTORYI_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "mathex/point2d.hpp"
#include "machlog/factory.hpp"

class MachLogFactoryImpl
// Canonical form revoked
{
public:
    MachLogFactoryImpl( MachPhys::FactorySubType );
    ~MachLogFactoryImpl();

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT( MachLogFactoryImpl );
	PER_FRIEND_READ_WRITE( MachLogFactoryImpl );

private:
    friend ostream& operator <<( ostream& o, const MachLogFactoryImpl& t );
	friend class MachLogFactory;

    MachLogFactoryImpl( const MachLogFactoryImpl& );
    MachLogFactoryImpl& operator =( const MachLogFactoryImpl& );

	MachLogFactory::ProductionLine		productionLine_;
	MachPhys::BuildingMaterialUnits		amountBuilt_;
	PhysAbsoluteTime					lastUpdateTime_;
	MachPhys::FactorySubType 			subType_;
	MexPoint2d 							specifiedDeployPoint_;
	bool								deployPointIsSet_;

};

PER_DECLARE_PERSISTENT( MachLogFactoryImpl );

#endif

/* End FACTORYI.HPP *************************************************/
