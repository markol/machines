/*
 * C N T R L . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
#include "base/base.hpp"
#include "ctl/algorith.hpp"

#include "sim/manager.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl.hpp"

/* //////////////////////////////////////////////////////////////// */

PER_DEFINE_PERSISTENT( MachLogController );

MachLogController::MachLogController( MachLogRace* logRace, W4dEntity* pPhysObject, const ControllerType& setType )
: SimActor( logRace, pPhysObject ),
//  : SimActor( &MachLogRaces::instance().race( r ), pPhysObject ),
  MachLogCanAdminister( logRace->race() ),
  race_( logRace->race() ),
  type_( setType )
{

}

MachLogController::~MachLogController()
{
	/* Intentionally Empty	*/
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogController::doOutputOperator( ostream& o) const
{
	o << (*this);
}

const MachLogController::ControllerType& MachLogController::type() const
{
	return type_;
}

ostream& operator<<( ostream& o, const MachLogController::ControllerType& t )
{
	switch( t )
	{
		case MachLogController::PLAYER_CONTROLLER:
			o << "PLAYER_CONTROLLER";
			break;
		case MachLogController::AI_CONTROLLER:
			o << "AI_CONTROLLER";
			break;
		case MachLogController::TEST_CONTROLLER:
			o << "TEST_CONTROLLER";
			break;
		default:
			o << "Unknown controller type\n";
			ASSERT_BAD_CASE;
	}
	return o;
}

 ostream& operator<<( ostream& o, const MachLogController& t )
{
	o << "MachLogController for race " << t.race_ << std::endl;
	o << "Type: " << t.type() << std::endl;
	return o;
}

void perWrite( PerOstream& ostr, const MachLogController& actor )
{
	const SimActor& base1 = actor;
	const MachLogCanAdminister& base2 = actor;
	const MachLogCanCommunicate& base3 = actor;
	

	ostr << base1;
	ostr << base2;
	ostr << base3;

	ostr << actor.race_;
	ostr << actor.type_;
	ostr << actor.hasWonScenario_;
}

void perRead( PerIstream& istr, MachLogController& actor )
{
	SimActor& base1 = actor;
	MachLogCanAdminister& base2 = actor;
	MachLogCanCommunicate& base3 = actor;
	

	istr >> base1;
	istr >> base2;
	istr >> base3;

	istr >> actor.race_;
	istr >> actor.type_;
	istr >> actor.hasWonScenario_;
	actor.assignToDifferentProcess( &MachLogRaces::instance().race( actor.race_ ) );

}

MachLogController::MachLogController( PerConstructor con )
:	SimActor( con ),
	MachLogCanAdminister( con )
{
}

/* //////////////////////////////////////////////////////////////// */

/* End CONTROLR.HPP *************************************************/
