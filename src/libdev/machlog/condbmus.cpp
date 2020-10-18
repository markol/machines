/*
 * C O N D B M U S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condbmus.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogBMUCountCondition );

MachLogBMUCountCondition::MachLogBMUCountCondition( const string& keyName, MachPhys::Race race, MachPhys::BuildingMaterialUnits number, MachLogBMUCountCondition::CheckType checkType )
:	SimCondition( keyName ),
	race_( race ),
	number_( number ),
	checkType_( checkType )
{
    TEST_INVARIANT;
}

MachLogBMUCountCondition::~MachLogBMUCountCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogBMUCountCondition::doHasConditionBeenMet() const
{
	bool result = false;

	HAL_STREAM("MachLogBMUCountCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::BuildingMaterialUnits nBMUs = races.nBuildingMaterialUnits( race_ );

	if( checkType_ == GREATER_THAN_EQUAL )
	{
		result = nBMUs >= number_;
	}
	else
	{
		result = nBMUs <= number_;
	}

	return result;
}

//static
MachLogBMUCountCondition* MachLogBMUCountCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a LOW_UNIT_COUNT condition line is:
	//<keyName> RACE <race> <comparitor> <number>
	CheckType comparitor = pParser->tokens()[4] == "GREATER_THAN_EQUAL" ? GREATER_THAN_EQUAL : LESS_THAN_EQUAL;

	return _NEW(
			MachLogBMUCountCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				atol( pParser->tokens()[5].c_str() ),
				comparitor
				)
			);
}

void MachLogBMUCountCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogBMUCountCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogBMUCountCondition::recommendedCallBackTimeGap() const
{
    static const PhysRelativeTime value = 2;
	return value;
}

//virtual
void MachLogBMUCountCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogBMUCountCondition " << (void*)this << " start" << std::endl;
	o << race_ << " number " << number_;
	if( checkType_ == GREATER_THAN_EQUAL )
		o << " GREATER_THAN_EQUAL\n";
	else
		o << " LESS_THAN_EQUAL\n";
}


void perWrite( PerOstream& ostr, const MachLogBMUCountCondition& bmuCount )
{
	const SimCondition& base1 = bmuCount;

	ostr << base1;
	ostr << bmuCount.race_;
	ostr << bmuCount.number_;
	ostr << bmuCount.checkType_;

}

void perRead( PerIstream& istr, MachLogBMUCountCondition& bmuCount )
{
	SimCondition& base1 = bmuCount;

	istr >> base1;
	istr >> bmuCount.race_;
	istr >> bmuCount.number_;
	istr >> bmuCount.checkType_;
}

MachLogBMUCountCondition::MachLogBMUCountCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
