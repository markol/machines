/*
 * M I N E S I T I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMineralSiteImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_MINESITI_HPP
#define _MACHLOG_MINESITI_HPP

#include "base/base.hpp"

#include "machlog/minesite.hpp"

#include "mathex/point3d.hpp"
#include "machphys/machphys.hpp"

class MachLogOreHolograph;


class MachLogMineralSiteImpl
// Canonical form revoked
{
public:
    MachLogMineralSiteImpl( const MachPhys::MineralGrade& grade ,
							const MachPhys::BuildingMaterialUnits& amount,
							const MexPoint3d& p);
    ~MachLogMineralSiteImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogMineralSiteImpl );
	PER_FRIEND_READ_WRITE( MachLogMineralSiteImpl );

	friend class MachLogMineralSite;

private:
    friend ostream& operator <<( ostream& o, const MachLogMineralSiteImpl& t );

	// Operations deliberately revoked
    MachLogMineralSiteImpl( const MachLogMineralSiteImpl& );
    MachLogMineralSiteImpl& operator =( const MachLogMineralSiteImpl& );
	bool operator ==( const MachLogMineralSiteImpl& );

 	MachLogMineralSite::State	 		state_;
	MexPoint3d							position_;
	MachPhys::BuildingMaterialUnits		amountOfOre_;
	MachPhys::MineralGrade				grade_;
	MachPhys::Race						discoveredBy_;
	bool 								registeredAsExhausted_;
	MachLogOreHolograph* 				pOreHolo_;
	int									id_;

};

PER_DECLARE_PERSISTENT( MachLogMineralSiteImpl );

#endif

/* End MINESITI.HPP ***************************************************/
