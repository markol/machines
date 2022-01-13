/*
 * S Q U A D I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogSquadronImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_SQUADI_HPP
#define _MACHLOG_SQUADI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/squad.hpp"

class MachLogSquadronImpl
// Canonical form revoked
{
public:
    MachLogSquadronImpl( int squadronId );
    ~MachLogSquadronImpl( void );

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT( MachLogSquadronImpl );
	PER_FRIEND_READ_WRITE( MachLogSquadronImpl );

private:
	friend class MachLogSquadron;
    friend ostream& operator <<( ostream& o, const MachLogSquadronImpl& t );

    MachLogSquadronImpl( const MachLogSquadronImpl& );
    MachLogSquadronImpl& operator =( const MachLogSquadronImpl& );

	MachLogAdministrator*				pCommander_;
	bool								hasCommander_;
    MachLogMachine*                     pStrongestMachine_;
    bool                                squadronHasChanged_;
	MachLogSquadron::Machines			machines_;
	int									squadronId_;
	MachLogSquadron::DesiredMachineList	desiredMachineList_;
	int									totalDesiredMachines_;
	bool								setDefCon_;
	MachLog::DefCon						defCon_;

};

PER_DECLARE_PERSISTENT( MachLogSquadronImpl );

#endif

/* End SQUADI.HPP ***************************************************/
