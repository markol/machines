/*
 * T E C H N I C I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_TECHNICI_HPP
#define _MACHLOG_TECHNICI_HPP

#include "machlog/machine.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysTechnician;
class MachPhysTechnicianData;

class MachLogTechnician
: public MachLogMachine // ,
//  public MachLogCanResearch
// cannonical form revoked
{
public:

	MachLogTechnician( const MachPhys::TechnicianSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogTechnician( const MachPhys::TechnicianSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogTechnician();
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysTechnicianData& data() const;

	// inherited from MachLogCanResearch
	virtual PhysRelativeTime research();

	///////////////////////////////

	const MachPhysTechnician& physTechnician() const;

	const MachPhys::TechnicianSubType& subType() const;

	int technologyLevel() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTechnician );
	PER_FRIEND_READ_WRITE( MachLogTechnician );

private:

	MachPhysTechnician& physTechnician();

	MachLogTechnician( const MachLogTechnician& );
	MachLogTechnician& operator =( const MachLogTechnician& );
	bool operator ==( const MachLogTechnician& ) const;

    static MachPhysTechnician* pNewPhysTechnician( const MachPhys::TechnicianSubType& subType, Level hwlevel, Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );

	virtual void doOutputOperator( ostream& o ) const;
	MachPhys::TechnicianSubType subType_;
};

PER_DECLARE_PERSISTENT( MachLogTechnician );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_TECHNICIAN_HPP	*/

/* End TECHNICI.HPP **************************************************/