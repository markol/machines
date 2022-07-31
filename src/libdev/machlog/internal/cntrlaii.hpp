/*
 * C N T R L A I I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAIControllerImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CNTRLAII_HPP
#define _MACHLOG_CNTRLAII_HPP

#include "base/base.hpp"
#include "machlog/cntrl_ai.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"
#include "stdlib/string.hpp"

class MachLogAIControllerImpl
// Canonical form revoked
{
public:
    MachLogAIControllerImpl( const string& rules );
    ~MachLogAIControllerImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogAIControllerImpl );
	PER_FRIEND_READ_WRITE( MachLogAIControllerImpl );

private:
	friend class MachLogAIController;
    friend ostream& operator <<( ostream& o, const MachLogAIControllerImpl& t );

    MachLogAIControllerImpl( const MachLogAIControllerImpl& );
    MachLogAIControllerImpl& operator =( const MachLogAIControllerImpl& );
	MachLogAIController::ResearchInterest				researchInterest_;

	//rules conatins the string variable which contains the strategic rules.
	string													rules_;
	MachLogAIController::StrategicProductionList			strategicProductionList_;
	MachLogAIController::ProductionList						constructionProductionList_;
	MachLogAIController::DesiredMachineList					desiredMachineList_;
	bool													checkForDynamicAllies_;
	PhysAbsoluteTime										nextAllyUpdateTime_;

};

class MachLogAIStrategicProductionUnit
{
	public:
		MachLogAIStrategicProductionUnit( const string& type, int number, int priority ):
			type_( type ),
			number_( number ),
			priority_( priority ) { ; }
		string	type() { return type_; }
		int		number() { return number_; }
		int		priority() { return priority_; }
		PER_MEMBER_PERSISTENT( MachLogAIStrategicProductionUnit );
		PER_FRIEND_READ_WRITE( MachLogAIStrategicProductionUnit );
	private:
		string	type_;
		int		number_;
		int		priority_;
};

PER_DECLARE_PERSISTENT( MachLogAIControllerImpl );
PER_DECLARE_PERSISTENT( MachLogAIStrategicProductionUnit );

#endif

/* End CNTRLAII.HPP *************************************************/
