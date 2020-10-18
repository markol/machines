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
#include "machlog/condhuni.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"
#include "machlog/artefact.hpp"


PER_DEFINE_PERSISTENT( MachLogHighUnitCountCondition );

MachLogHighUnitCountCondition::MachLogHighUnitCountCondition( const string& keyName, MachPhys::Race race, MachLog::ObjectType ot, size_t number, int subType )
:	SimCondition( keyName ),
	objectType_( ot ),
	race_( race ),
	number_( number ),
	subType_( subType )
{
    TEST_INVARIANT;
}

MachLogHighUnitCountCondition::~MachLogHighUnitCountCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogHighUnitCountCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogHighUnitCountCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	switch( objectType_ )
	{
		case MachLog::AGGRESSOR: return races.aggressors( race_ ).size() >= number_;
		case MachLog::ADMINISTRATOR: return races.administrators( race_ ).size() >= number_;
		case MachLog::APC: return races.apcs( race_ ).size() >= number_;
		case MachLog::CONSTRUCTOR: return races.constructors( race_ ).size() >= number_;
		case MachLog::GEO_LOCATOR: return races.geoLocators( race_ ).size() >= number_;
		case MachLog::SPY_LOCATOR: return races.spyLocators( race_ ).size() >= number_;
		case MachLog::RESOURCE_CARRIER: return races.resourceCarriers( race_ ).size() >= number_;
		case MachLog::TECHNICIAN: return races.technicians( race_ ).size() >= number_;
		case MachLog::BEACON:
		case MachLog::FACTORY:
		case MachLog::GARRISON:
		case MachLog::HARDWARE_LAB:
		case MachLog::MINE:
		case MachLog::MISSILE_EMPLACEMENT:
		case MachLog::POD:
		case MachLog::SMELTER:
			return races.nCompleteConstructions( race_, objectType_ ) >= number_;
		case MachLog::ARTEFACT:
		{
			int nArtefacts = 0;
			for( MachLogRaces::Artefacts::iterator i = races.artefactsCollection().begin(); i != races.artefactsCollection().end(); ++i )
				if( (*i)->subType() == subType_ )
					++nArtefacts;
			return nArtefacts >= number_;
		}
		default:
			ASSERT_INFO( objectType_ );
			ASSERT_BAD_CASE;
	}
	return false;
}

//static
MachLogHighUnitCountCondition* MachLogHighUnitCountCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a LOW_UNIT_COUNT condition line is:
	//<keyName> RACE <race> <objectType> <number less than = done>
	MachLog::ObjectType ot = MachLogScenario::objectType( pParser->tokens()[4] );
	int subType = 0;
	if( ot == MachLog::ARTEFACT )
		subType = atol( pParser->tokens()[6].c_str() );
	return _NEW(
			MachLogHighUnitCountCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				ot,
				atol( pParser->tokens()[5].c_str() ),
				subType
				)
			);
}

void MachLogHighUnitCountCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogHighUnitCountCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogHighUnitCountCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 2;
	return value;
}

//virtual
void MachLogHighUnitCountCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogHighUnitCountCondition " << (void*)this << " start" << std::endl;
	o << "objectType_ " << objectType_ << race_ << " number " << number_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogHighUnitCountCondition& HighUnitCount )
{
	const SimCondition& base1 = HighUnitCount;

	ostr << base1;
	ostr << HighUnitCount.objectType_;
	ostr << HighUnitCount.race_;
	ostr << HighUnitCount.number_;
	ostr << HighUnitCount.subType_;

}

void perRead( PerIstream& istr, MachLogHighUnitCountCondition& HighUnitCount )
{
	SimCondition& base1 = HighUnitCount;

	istr >> base1;
	istr >> HighUnitCount.objectType_;
	istr >> HighUnitCount.race_;
	istr >> HighUnitCount.number_;
	istr >> HighUnitCount.subType_;
}

MachLogHighUnitCountCondition::MachLogHighUnitCountCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
