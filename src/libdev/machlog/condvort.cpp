/*
 * C O N D V O R T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condvort.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogVortexHasBeenFiredCondition );

MachLogVortexHasBeenFiredCondition::MachLogVortexHasBeenFiredCondition( const string& keyName, MachPhys::Race race )
:	SimCondition( keyName ),
	race_( race )
{
    TEST_INVARIANT;
}

MachLogVortexHasBeenFiredCondition::~MachLogVortexHasBeenFiredCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogVortexHasBeenFiredCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogVortexHasBeenFiredCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	return races.vortexHasBeenFired( race_ );
}

//static
MachLogVortexHasBeenFiredCondition* MachLogVortexHasBeenFiredCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a VORTEX_FIRED condition line is:
	//<keyName> RACE <race>

	return _NEW(
			MachLogVortexHasBeenFiredCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] )
				)
			);
}

void MachLogVortexHasBeenFiredCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogVortexHasBeenFiredCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogVortexHasBeenFiredCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 1.9;
	return value;
}

//virtual
void MachLogVortexHasBeenFiredCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogVortexHasBeenFiredCondition " << (void*)this << " start" << std::endl;
	o << race_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogVortexHasBeenFiredCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.race_;

}

void perRead( PerIstream& istr, MachLogVortexHasBeenFiredCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.race_;
}

MachLogVortexHasBeenFiredCondition::MachLogVortexHasBeenFiredCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
