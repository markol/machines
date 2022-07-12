/*
 * A C T R E I N F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogReinforcementsAction

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_ACTREINF_HPP
#define _MACHLOG_ACTREINF_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "sim/action.hpp"
#include "mathex/point2d.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;
class MachLogProductionUnit;

class MachLogReinforcementsAction : public SimAction
// Canonical form revoked
{
public:
	typedef ctl_pvector< MachLogProductionUnit >	ProductionList;

    virtual ~MachLogReinforcementsAction();
	static MachLogReinforcementsAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );

	const MexPoint2d& destination() const;
	MachPhys::Race	race() const;

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogReinforcementsAction );
	PER_FRIEND_READ_WRITE( MachLogReinforcementsAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:
	//method for defining the desired machine types.
	void addDesiredMachine( MachLogProductionUnit* );
	void destination( const MexPoint2d& );

    void parseFromScenario( MachPhys::Race race, UtlLineTokeniser* pParser );

    MachLogReinforcementsAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogReinforcementsAction& t );

    MachLogReinforcementsAction( const MachLogReinforcementsAction& );
    MachLogReinforcementsAction& operator =( const MachLogReinforcementsAction& );

	ProductionList			productionList_;
	MexPoint2d				dest_;
	MachPhys::Race			race_;
	MachPhys::Race			displayAsRace_;
	int						assignToSquadronIndex_;
	bool					assignToSquadron_;
	bool					giveVoiceMail_;

};

PER_DECLARE_PERSISTENT( MachLogReinforcementsAction );

#endif

/* End ACTREINF.HPP *************************************************/
