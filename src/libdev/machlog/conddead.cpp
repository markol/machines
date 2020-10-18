/*
 * C O N D D E A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/conddead.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogAllUnitsDeadCondition );

MachLogAllUnitsDeadCondition::MachLogAllUnitsDeadCondition( const string& keyName, MachPhys::Race race )
:	SimCondition( keyName ),
	race_( race )
{
    TEST_INVARIANT;
}

MachLogAllUnitsDeadCondition::~MachLogAllUnitsDeadCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogAllUnitsDeadCondition::doHasConditionBeenMet() const
{
	MachLogRaces& races = MachLogRaces::instance();
	HAL_STREAM("MachLogAllUnitsDeadCondition::doHasConditionBeenMet " << race_ << " constructions " << races.nConstructions( race_ ) << " machines " << races.nMachines( race_ ) << std::endl );
	return ( races.nConstructions( race_ ) + races.nMachines( race_ ) ) == 0;
}

//static
MachLogAllUnitsDeadCondition* MachLogAllUnitsDeadCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a ALL_UNITS_DEAD condition line is:
	//<keyName> RACE <race>

	return _NEW(
			MachLogAllUnitsDeadCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] )
				)
			);
}

void MachLogAllUnitsDeadCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAllUnitsDeadCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogAllUnitsDeadCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime result = 2.0;
	return result;
}

//virtual
void MachLogAllUnitsDeadCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogAllUnitsDeadCondition " << (void*)this << " start" << std::endl;
	o << race_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogAllUnitsDeadCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.race_;

}

void perRead( PerIstream& istr, MachLogAllUnitsDeadCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.race_;
}

MachLogAllUnitsDeadCondition::MachLogAllUnitsDeadCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
