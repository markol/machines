/*
 * C N T R L B A S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachLogController is the common base class for all controllers
   which represents the top of the
 * logical command heirarchy. I.e. it is the entity responsible
 * (for an NPC) of making the strategic desicions. This includes such things 
 * as creating administrator patrols.
 */

#ifndef _MACHLOG_CONTROLR_BASE_HPP
#define _MACHLOG_CONTROLR_BASE_HPP

#include "sim/actor.hpp"
#include "machphys/machphys.hpp"
#include "machlog/message.hpp"
#include "machlog/canadmin.hpp"

//class ostream;
class W4dEntity;
class MachLogRace;
/* //////////////////////////////////////////////////////////////// */

class MachLogController
: 	public SimActor,
	public MachLogCanCommunicate,
	public MachLogCanAdminister
{
public:

	enum ControllerType
	{
		PLAYER_CONTROLLER,
		AI_CONTROLLER,
		TEST_CONTROLLER
	};

	MachLogController( MachLogRace* logRace, W4dEntity* pPhysObject, const ControllerType& );

	virtual ~MachLogController();

	virtual void doOutputOperator( ostream& ) const;

	MachPhys::Race race() const { return race_; };

	const ControllerType& type() const;

	friend ostream& operator<<( ostream&, const MachLogController& );
	friend ostream& operator<<( ostream& o, const ControllerType& t );

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogController );
	PER_FRIEND_READ_WRITE( MachLogController );

private:

	///////////////////////////////

	void deleteHolographAtPosition( const MexPoint3d& pos );

	///////////////////////////////
	
	MachPhys::Race		race_;
	ControllerType		type_;
	bool				hasWonScenario_;

};

PER_DECLARE_PERSISTENT( MachLogController );
PER_ENUM_PERSISTENT( MachLogController::ControllerType );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_CONTROLR_BASE_HPP	*/

/* End CONTROLR.HPP *************************************************/
