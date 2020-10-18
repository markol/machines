/*
 * S C O R E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogScoreImpl

    Score. A score object contains all kinds of values hence the impl class.
*/

#ifndef _MACHLOG_SCOREI_HPP
#define _MACHLOG_SCOREI_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"


class MachLogScoreImpl
{
public:
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogScoreImpl );
	PER_FRIEND_READ_WRITE( MachLogScoreImpl );
	MachLogScoreImpl();
	~MachLogScoreImpl();

private:
	friend class MachLogScore;
	friend class MachLogMessageBroker;
	MachPhys::Race		race_;
	int					grossScore_;
	int 				machinesBuilt_;
	int 				militaryMachinesBuilt_;
	int 				constructionsBuilt_;
	int 				myMachinesDestroyed_;
	int 				raceMyMachinesDestroyed_[ MachPhys::N_RACES ];
	int 				otherMachinesDestroyed_;
	int 				raceMachinesDestroyed_[ MachPhys::N_RACES ];
	int 				myConstructionsDestroyed_;
	int 				raceMyConstructionsDestroyed_[ MachPhys::N_RACES ];
	int 				otherConstructionsDestroyed_;
	int 				raceConstructionsDestroyed_[ MachPhys::N_RACES ];
	int 				itemsResearched_;
	int 				totalResearchCost_;
	MachPhys::BuildingMaterialUnits	BMUsMined_;
	bool				scoreShouldBeDisplayed_;
};

PER_DECLARE_PERSISTENT( MachLogScoreImpl );

#endif