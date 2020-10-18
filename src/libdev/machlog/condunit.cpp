/*
 * C O N D U N I T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condunit.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"
#include "machlog/artefact.hpp"


PER_DEFINE_PERSISTENT( MachLogLowUnitCountCondition );

MachLogLowUnitCountCondition::MachLogLowUnitCountCondition( const string& keyName, MachPhys::Race race, MachLog::ObjectType ot, size_t number, int subType )
:	SimCondition( keyName ),
	objectType_( ot ),
	race_( race ),
	number_( number ),
	subType_( subType )
{
    TEST_INVARIANT;
}

MachLogLowUnitCountCondition::~MachLogLowUnitCountCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogLowUnitCountCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogLowUnitCountCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	switch( objectType_ )
	{
		case MachLog::AGGRESSOR: return races.aggressors( race_ ).size() < number_;
		case MachLog::ADMINISTRATOR: return races.administrators( race_ ).size() < number_;
		case MachLog::APC: return races.apcs( race_ ).size() < number_;
		case MachLog::CONSTRUCTOR: return races.constructors( race_ ).size() < number_;
		case MachLog::GEO_LOCATOR: return races.geoLocators( race_ ).size() < number_;
		case MachLog::SPY_LOCATOR: return races.spyLocators( race_ ).size() < number_;
		case MachLog::RESOURCE_CARRIER: return races.resourceCarriers( race_ ).size() < number_;
		case MachLog::TECHNICIAN: return races.technicians( race_ ).size() < number_;
		case MachLog::ARTEFACT:
		{
			int nArtefacts = 0;
			for( MachLogRaces::Artefacts::iterator i = races.artefactsCollection().begin(); i != races.artefactsCollection().end(); ++i )
				if( (*i)->subType() == subType_ )
					++nArtefacts;
			return nArtefacts < number_;
		}
		//All other cases should be for constructions
		case MachLog::BEACON:
		case MachLog::FACTORY:
		case MachLog::GARRISON:
		case MachLog::HARDWARE_LAB:
		case MachLog::MINE:
		case MachLog::MISSILE_EMPLACEMENT:
		case MachLog::POD:
		case MachLog::SMELTER:
			return races.nCompleteConstructions( race_, objectType_ ) < number_;
		default:
			ASSERT_INFO( objectType_ );
			ASSERT_BAD_CASE;
	}
	return false;
}

//static
MachLogLowUnitCountCondition* MachLogLowUnitCountCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a LOW_UNIT_COUNT condition line is:
	//<keyName> RACE <race> <objectType> <number less than = done>
	MachLog::ObjectType ot = MachLogScenario::objectType( pParser->tokens()[4] );
	int subType = 0;
	if( ot == MachLog::ARTEFACT )
		subType = atol( pParser->tokens()[6].c_str() );
	return _NEW(
			MachLogLowUnitCountCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				ot,
				atol( pParser->tokens()[5].c_str() ),
				subType
				)
			);
}

void MachLogLowUnitCountCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogLowUnitCountCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogLowUnitCountCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 1.8;
	return value;
}

//virtual
void MachLogLowUnitCountCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogLowUnitCountCondition " << (void*)this << " start" << std::endl;
	o << "objectType_ " << objectType_ << race_ << " number " << number_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogLowUnitCountCondition& lowUnitCount )
{
	const SimCondition& base1 = lowUnitCount;

	ostr << base1;
	ostr << lowUnitCount.objectType_;
	ostr << lowUnitCount.race_;
	ostr << lowUnitCount.number_;
	ostr << lowUnitCount.subType_;

}

void perRead( PerIstream& istr, MachLogLowUnitCountCondition& lowUnitCount )
{
	SimCondition& base1 = lowUnitCount;

	istr >> base1;
	istr >> lowUnitCount.objectType_;
	istr >> lowUnitCount.race_;
	istr >> lowUnitCount.number_;
	istr >> lowUnitCount.subType_;
}

MachLogLowUnitCountCondition::MachLogLowUnitCountCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
