/*
 * C O N D P O D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condpod.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogPodCapturedCondition );

MachLogPodCapturedCondition::MachLogPodCapturedCondition( const string& keyName, MachPhys::Race race )
:	SimCondition( keyName ),
	race_( race )
{
    TEST_INVARIANT;
}

MachLogPodCapturedCondition::~MachLogPodCapturedCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogPodCapturedCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogPodCapturedCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	return races.podCaptured( race_ );
}

//static
MachLogPodCapturedCondition* MachLogPodCapturedCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a VORTEX_FIRED condition line is:
	//<keyName> RACE <race>

	return _NEW(
			MachLogPodCapturedCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] )
				)
			);
}

void MachLogPodCapturedCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPodCapturedCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogPodCapturedCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 2;
	return value;
}

//virtual
void MachLogPodCapturedCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogPodCapturedCondition " << (void*)this << " start" << std::endl;
	o << race_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogPodCapturedCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.race_;

}

void perRead( PerIstream& istr, MachLogPodCapturedCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.race_;
}

MachLogPodCapturedCondition::MachLogPodCapturedCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
