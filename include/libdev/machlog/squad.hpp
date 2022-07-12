/*
 * S Q U A D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogSquadron

    This class represents the container of machines that are in a squadron.
	If there is at least one administrator then that will be cached as the
	squadron commander.
	If there are more than one administrators in a squad then the highest
	level sw will be in charge.
	There will (should?!) only ever be ten of these classes per side.
*/

#ifndef _MACHLOG_SQUAD_HPP
#define _MACHLOG_SQUAD_HPP

#include "base/base.hpp"
#include "machlog/actor.hpp"

class MachLogMachine;
class MachLogAdministrator;
class MachLogProductionUnit;
class MachLogRace;
class W4dEntity;
class DesiredMachineData;
class MachLogSquadronImpl;
template < class X > class ctl_pvector;

class MachLogSquadron : public MachActor
{
public:

	typedef ctl_pvector< MachLogMachine >	Machines;

//    MachLogSquadron( const int& id );
	MachLogSquadron( MachLogRace* logRace, W4dEntity* pPhysObject, int squadronId );
	MachLogSquadron( MachLogRace* logRace, W4dEntity* pPhysObject, int squadronId, UtlId );
    virtual ~MachLogSquadron();

	int squadronId() const;

	bool hasCommander() const;
	MachLogAdministrator& commander();
	const MachLogAdministrator& commander() const;

    void CLASS_INVARIANT;

	bool addToControl( MachLogMachine* );
	void removeFromControl( const MachLogMachine* );
	void releaseAllMachines();

	Machines& machines();
	const Machines& machines() const;

	//part of machactor
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR );
	//override this to return holograph data.
	virtual MachPhysObjectData& objectData() const;

	void addDesiredMachine( MachLogProductionUnit*, int number );
	void removeAllDesiredMachines();
//	const MachLogSquadron::DesiredMachines& desiredMachines() const;

	// sets the virtual defcon to all machines to DEFCON_HIGH for a short period.
	void manualCommandIssuedToSquadron();

    MachLogMachine* getStrongestMachine();

    virtual PhysRelativeTime beDestroyed();

    friend ostream& operator <<( ostream& o, const MachLogSquadron& t );

	virtual void doOutputOperator( ostream& ) const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSquadron );
	PER_FRIEND_READ_WRITE( MachLogSquadron );

	void autoSetDefCon( MachLog::DefCon );

protected:
	//have to have these...implementation does nothing
	virtual void doStartExplodingAnimation();
	virtual void doEndExplodingAnimation();


private:
	friend class MachLogSquadronImpl;
   	//Subclass must override to modify the display.
   	//Called on change of state. Squadrons implementation does nothing.
   	virtual void doVisualiseSelectionState();
    // Operations deliberately revoked
    MachLogSquadron( const MachLogSquadron& );
    MachLogSquadron& operator =( const MachLogSquadron& );
    bool operator ==( const MachLogSquadron& );

	typedef ctl_pvector< DesiredMachineData >	DesiredMachineList;

	bool checkIfMachineMissing( MachLogProductionUnit* ) const;

	MachLogSquadronImpl*				pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogSquadron );

#endif

/* End SQUAD.HPP ****************************************************/
