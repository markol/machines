/*
 * C O N D F I R E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condfire.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogFiredAtCondition );

MachLogFiredAtCondition::MachLogFiredAtCondition( const string& keyName, MachPhys::Race firingRace, MachPhys::Race firedAtRace )
:	SimCondition( keyName ),
	firingRace_( firingRace ),
	firedAtRace_( firedAtRace )
{
    TEST_INVARIANT;
}

MachLogFiredAtCondition::~MachLogFiredAtCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogFiredAtCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogFiredAtCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	return races.firedAt( firingRace_, firedAtRace_ );
}

//static
MachLogFiredAtCondition* MachLogFiredAtCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a VORTEX_FIRED condition line is:
	//<keyName> RACE <race> FIRED AT <race>

	return _NEW(
			MachLogFiredAtCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				MachLogScenario::machPhysRace( pParser->tokens()[6] )
				)
			);
}

void MachLogFiredAtCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogFiredAtCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogFiredAtCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 2;
	return value;
}

//virtual
void MachLogFiredAtCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogFiredAtCondition " << (void*)this << " start" << std::endl;
	o << "Firing " << firingRace_ << " At " << firedAtRace_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogFiredAtCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.firingRace_;
	ostr << condition.firedAtRace_;

}

void perRead( PerIstream& istr, MachLogFiredAtCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.firingRace_;
	istr >> condition.firedAtRace_;
}

MachLogFiredAtCondition::MachLogFiredAtCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
