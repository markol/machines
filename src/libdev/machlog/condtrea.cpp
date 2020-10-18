/*
 * C O N D T R E A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condtrea.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogTechniciansTreacheriedCondition );

MachLogTechniciansTreacheriedCondition::MachLogTechniciansTreacheriedCondition( const string& keyName, MachPhys::Race race, int number )
:	SimCondition( keyName ),
	race_( race ),
	number_( number )
{
    TEST_INVARIANT;
}

MachLogTechniciansTreacheriedCondition::~MachLogTechniciansTreacheriedCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogTechniciansTreacheriedCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogTechniciansTreacheriedCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	return races.techniciansTreacheried( race_ ) >= number_;
}

//static
MachLogTechniciansTreacheriedCondition* MachLogTechniciansTreacheriedCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a TECHNICIANS_TREACHERIED condition line is:
	//<keyName> RACE <race> <number>

	return _NEW(
			MachLogTechniciansTreacheriedCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				atol( pParser->tokens()[4].c_str() )
				)
			);
}

void MachLogTechniciansTreacheriedCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogTechniciansTreacheriedCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogTechniciansTreacheriedCondition::recommendedCallBackTimeGap() const
{
    static const PhysRelativeTime value = 2;
	return value;
}

//virtual
void MachLogTechniciansTreacheriedCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogTechniciansTreacheriedCondition " << (void*)this << " start" << std::endl;
	o << race_ << " number " << number_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogTechniciansTreacheriedCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.race_;
	ostr << condition.number_;

}

void perRead( PerIstream& istr, MachLogTechniciansTreacheriedCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.race_;
	istr >> condition.number_;
}

MachLogTechniciansTreacheriedCondition::MachLogTechniciansTreacheriedCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
