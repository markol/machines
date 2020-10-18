/*
 * C N T R L _ P C . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
#include "base/base.hpp"
#include "ctl/algorith.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/cvexpgon.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "machlog/cntrl_pc.hpp"
#include "machlog/machine.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogPCController );

MachLogPCController::MachLogPCController( MachLogRace* logRace, W4dEntity* pPhysObject )
  : MachLogController( logRace, pPhysObject, MachLogController::PLAYER_CONTROLLER )
//  : SimActor( &MachLogRaces::instance().race( r ), pPhysObject ),
{
}

MachLogPCController::~MachLogPCController()
{
	/* Intentionally Empty	*/
}

/* //////////////////////////////////////////////////////////////// */


PhysRelativeTime MachLogPCController::update( const PhysRelativeTime&, MATHEX_SCALAR )
{

	return 15.0;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogPCController::sendMessage( MachLogCommsId , const MachLogMessage& ) const
{
	ASSERT( false, "shouldn't be here" );
}

void MachLogPCController::receiveMessage( const MachLogMessage& msg )
{
	PRE( msg.sender()->race() == race() );

	switch( msg.text() )
	{
		case MachLogMessage::IDLE :
		{
			switch( msg.sender()->objectType() )
			{
				case MachLog::GEO_LOCATOR :
					handleIdleGeoLocator( msg.sender() );	
				break;
				case MachLog::SPY_LOCATOR :
					handleIdleSpyLocator( msg.sender() );	
				break;
				case MachLog::TECHNICIAN	:
					handleIdleTechnician( msg.sender() );
				break;
				case MachLog::CONSTRUCTOR :
					handleIdleConstructor( msg.sender() );
				break;
				case MachLog::AGGRESSOR :
					handleIdleAggressor( msg.sender() );
				break;
				case MachLog::ADMINISTRATOR :
					handleIdleAdministrator( msg.sender() );
				break;
				case MachLog::RESOURCE_CARRIER :
					handleIdleResourceCarrier( msg.sender() );	
				break;
				case MachLog::APC :
					handleIdleAPC( msg.sender() );	
				break;
				default	:
//					ASSERT_BAD_CASE;
				break;
			};
		} break;
//		case MachLogMessage::ENEMY_SIGHTED	:
//			handleEnemyContact( msg );
//		break;
		default				: 
//			ASSERT_BAD_CASE;
		break;
	};
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogPCController::handleIdleTechnician( MachLogCommsId )
{
}
			

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogPCController::handleIdleAdministrator( MachLogCommsId )
{
}

//virtual 
void MachLogPCController::handleIdleAggressor( MachLogCommsId )
{
}
//virtual 
void MachLogPCController::handleIdleAPC( MachLogCommsId )
{
}
//virtual 
void MachLogPCController::handleIdleResourceCarrier( MachLogCommsId )
{
}
//virtual 
void MachLogPCController::handleIdleConstructor( MachLogCommsId )
{
}
//virtual 
void MachLogPCController::handleIdleGeoLocator( MachLogCommsId )
{
}
//virtual 
void MachLogPCController::handleIdleSpyLocator( MachLogCommsId )
{
}

ostream& operator<<( ostream& o, const MachLogPCController& t )
{
	t.doOutputOperator( o );
	return o;
}

//virtual 
void MachLogPCController::doOutputOperator( ostream& o) const
{
	o << "Controller [PC] for race " << race() << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogPCController& actor )
{
	PER_WRITE_INDENT_STREAM("Writing a MachLogPCController\n" );

	const MachLogController& base1 = actor;

	ostr << base1;

	PER_WRITE_INDENT_STREAM("Writing a MachLogPCController DONE\n" );
}

void perRead( PerIstream& istr, MachLogPCController& actor )
{
	PER_READ_INDENT_STREAM("Reading a MachLogPCController\n" );

	MachLogController& base1 = actor;

	istr >> base1;

	PER_READ_INDENT_STREAM("Reading a MachLogPCController DONE\n" );
}

MachLogPCController::MachLogPCController( PerConstructor con )
:	MachLogController( con )
{
}

/* //////////////////////////////////////////////////////////////// */

/* End CONTROLR.HPP *************************************************/
