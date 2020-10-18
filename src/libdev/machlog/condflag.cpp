/*
 * C O N D FLAG . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp" //for instantiation only
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condflag.hpp"
#include "machlog/races.hpp"
#include "machlog/dbhandlr.hpp"


PER_DEFINE_PERSISTENT( MachLogScenarioFlagCondition );

MachLogScenarioFlagCondition::MachLogScenarioFlagCondition( const string& keyName, bool isSet )
:	SimCondition( keyName ),
	flagSet_( isSet )
{
    TEST_INVARIANT;
}

MachLogScenarioFlagCondition::~MachLogScenarioFlagCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogScenarioFlagCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogScenarioFlagCondition::doHasConditionBeenMet set: " << flagSet_ << std::endl );
	return flagSet_;
}

//static
MachLogScenarioFlagCondition* MachLogScenarioFlagCondition::newFromParser( UtlLineTokeniser* pParser )
{
    //Get the flag setting via the database interface handler
    bool isSet = MachLogRaces::instance().databaseHandler().isFlagSet( pParser->tokens()[1], pParser->tokens()[3] );
	return _NEW( MachLogScenarioFlagCondition( pParser->tokens()[1], isSet ) );
}

void MachLogScenarioFlagCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogScenarioFlagCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogScenarioFlagCondition::recommendedCallBackTimeGap() const
{
	//Only really needs to be evaluated once since the result won't change during the scenario
	static const PhysRelativeTime value = 3600.0;
	return value;
}

//virtual
void MachLogScenarioFlagCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogScenarioFlagCondition " << (void*)this << " start" << std::endl;
	o << " flagSet_ " << flagSet_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogScenarioFlagCondition& t )
{
	const SimCondition& base1 = t;

	ostr << base1;
	ostr << t.flagSet_;

}

void perRead( PerIstream& istr, MachLogScenarioFlagCondition& t )
{
	SimCondition& base1 = t;

	istr >> base1;
	istr >> t.flagSet_;
}

MachLogScenarioFlagCondition::MachLogScenarioFlagCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDFLAG.CPP *************************************************/
