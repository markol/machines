/*
 * C O N D D E A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condoded.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"


PER_DEFINE_PERSISTENT( MachLogAllOtherRacesUnitsDeadCondition );

MachLogAllOtherRacesUnitsDeadCondition::MachLogAllOtherRacesUnitsDeadCondition( const string& keyName, MachPhys::Race race )
:	SimCondition( keyName ),
	race_( race )
{
    otherRace_.reserve( MachPhys::N_RACES );

	for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
		if( i != race )
			otherRace_.push_back( i );

    TEST_INVARIANT;
}

MachLogAllOtherRacesUnitsDeadCondition::~MachLogAllOtherRacesUnitsDeadCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogAllOtherRacesUnitsDeadCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogAllOtherRacesUnitsDeadCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	bool result = true;
	for( int i = 0; i < otherRace_.size(); ++i )
		if( ( races.nConstructions( otherRace_[i] ) + races.nMachines( otherRace_[i] ) != 0 ) and not races.hasLost( otherRace_[i] ) )
			result = false;
	return result;
}

//static
MachLogAllOtherRacesUnitsDeadCondition* MachLogAllOtherRacesUnitsDeadCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a ALL_OTHER_UNITS_DEAD condition line is:
	//<keyName> RACE <race>

	return _NEW(
			MachLogAllOtherRacesUnitsDeadCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] )
				)
			);
}

void MachLogAllOtherRacesUnitsDeadCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAllOtherRacesUnitsDeadCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogAllOtherRacesUnitsDeadCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 2.0;
	return value;
}

//virtual
void MachLogAllOtherRacesUnitsDeadCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogAllOtherRacesUnitsDeadCondition " << (void*)this << " start" << std::endl;
	o << race_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogAllOtherRacesUnitsDeadCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.race_;
	ostr << condition.otherRace_;

}

void perRead( PerIstream& istr, MachLogAllOtherRacesUnitsDeadCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.race_;
	istr >> condition.otherRace_;
}

MachLogAllOtherRacesUnitsDeadCondition::MachLogAllOtherRacesUnitsDeadCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
