/*
 * C O N D R E A C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/actor.hpp"
#include "machlog/condreac.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"
#include "machlog/machine.hpp"
#include "machlog/constron.hpp"
#include "machlog/canattac.hpp"
#include "machlog/artefact.hpp"


PER_DEFINE_PERSISTENT( MachLogReachedLocationCondition );

MachLogReachedLocationCondition::MachLogReachedLocationCondition( const string& keyName, MachPhys::Race race, size_t number, const MexPoint2d& point, const MATHEX_SCALAR& range )
:	SimCondition( keyName ),
	race_( race ),
	number_( number ),
	point_( point ),
	sqrRange_( (range * range) ),
	subTypeSet_( false ),
	hwLevelSet_( false ),
	weaponComboSet_( false ),
	anyTypeWillDo_( false ),
	machineWillDo_( false ),
	constructionWillDo_( false ),
	doRangeCheck_( true ),
	booleanType_( GREATER_THAN_EQUAL )
{
    TEST_INVARIANT;
}

MachLogReachedLocationCondition::~MachLogReachedLocationCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogReachedLocationCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogReachedLocationCondition::doHasConditionBeenMet " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	if( objectType_ == MachLog::ARTEFACT )
	{
		int nArtefacts = 0;
		for( MachLogRaces::Artefacts::iterator i = races.artefactsCollection().begin(); i != races.artefactsCollection().end(); ++i )
			if( (*i)->subType() == subType_ )
				++nArtefacts;
		if( booleanType_ == GREATER_THAN_EQUAL )
		{
			if( nArtefacts >= number_ )
				return true;
		}
		else if( booleanType_ == LESS_THAN )
		{
			if( nArtefacts < number_ )
				return true;
		}
		return false;
	}
	MachLogRaces::Objects& objects = races.objects();
	//iterate through all the objects collection and push_back onto local vector any matching objects.
	MachLogRaces::Objects matchingObjects;

	for( MachLogRaces::Objects::iterator i = objects.begin(); i != objects.end(); ++i )
	{
		//do the quick checks first - these are all the numerical based checks.
		bool objectOk = true;
		if( (*i)->race() != race_ )
			objectOk = false;
		else if( anyTypeWillDo_ )	//take out object type selection if any is set
			objectOk = true;
		else if( machineWillDo_ )
		{
			if( not (*i)->objectIsMachine() )
				objectOk = false;
		}
		else if( constructionWillDo_ )
		{
			if( not (*i)->objectIsConstruction() )
				objectOk = false;
			else if( not (*i)->asConstruction().isComplete() )
				objectOk = false;
		}
		else if( (*i)->objectType() == objectType_ )
		{
			if( subTypeSet_ and (*i)->subType() != subType_ )
				objectOk = false;
			if( objectOk and hwLevelSet_ )
			{
				if( (*i)->objectIsMachine() )
				{
					MachLogMachine& mlm = (*i)->asMachine();
					if( mlm.hwLevel() != hwLevel_ )
						objectOk = false;
				}
				else if( (*i)->objectIsConstruction() )
				{
					MachLogConstruction& mlc = (*i)->asConstruction();
					if( mlc.level() != hwLevel_ or not mlc.isComplete() )
						objectOk = false;
				}
				else	//if the object is not a machine or construction then matching on level is impossible
				{
					objectOk = false;
				}
			}
			if( objectOk and weaponComboSet_ )
			{
				if( (*i)->objectIsCanAttack() )
				{
					if( (*i)->asCanAttack().weaponCombo() != weaponCombo_ )
						objectOk = false;
				}
				else
					objectOk = false;
			}
		}
		else
			objectOk = false;

		if( doRangeCheck_ )
		{
			if( objectOk and point_.sqrEuclidianDistance( (*i)->position() ) < sqrRange_ )
				matchingObjects.push_back( *i );
		}
		else
			if( objectOk )
				matchingObjects.push_back( *i );
	}

	if( booleanType_ == GREATER_THAN_EQUAL )
	{
		if( matchingObjects.size() >= number_ )
			return true;
	}
	else if( booleanType_ == LESS_THAN )
	{
		if( matchingObjects.size() < number_ )
			return true;
	}

	return false;
}

//static
MachLogReachedLocationCondition* MachLogReachedLocationCondition::newFromParser( UtlLineTokeniser* pParser )
{
	//format of a REACHED_LOCATION condition line is:
	//<keyName> RACE <race> <number greater than = done> AT <point> RANGE <range> <objectType> [<hwLevel>] [<subType>] [<weaponCombo>]

	MachLogReachedLocationCondition* pResult =
			_NEW( MachLogReachedLocationCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				atol( pParser->tokens()[4].c_str() ),
				MexPoint2d( atof( pParser->tokens()[6].c_str() ), atof( pParser->tokens()[7].c_str() ) ),
				atof( pParser->tokens()[9].c_str() ) ) );
	if( pParser->tokens()[10] == "ANY" )
		pResult->anyTypeWillDo_ = true;
	else if( pParser->tokens()[10] == "MACHINE" )
		pResult->machineWillDo_ = true;
	else if( pParser->tokens()[10] == "CONSTRUCTION" )
		pResult->constructionWillDo_ = true;
	else
	{
		//objectType must be set
		pResult->objectType_ = MachLogScenario::objectType( pParser->tokens()[10] );
	}
	//All other switch parameters are optional.
	if( pParser->tokens().size() >= 12 )
	{
		pResult->hwLevelSet_ = true;
		pResult->hwLevel_ = atol( pParser->tokens()[11].c_str() );
		if( pParser->tokens().size() >= 13 )
		{
			pResult->subTypeSet_ = true;
			pResult->subType_ = MachLogScenario::objectSubType( pResult->objectType_, pParser->tokens()[12] );
			if( pParser->tokens().size() >= 14 )
			{
				pResult->weaponComboSet_ = true;
				pResult->weaponCombo_ = MachLogScenario::weaponCombo( pParser->tokens()[13] );
			}
		}
	}
	return pResult;
}

//static
MachLogReachedLocationCondition* MachLogReachedLocationCondition::newFromParser( UtlLineTokeniser* pParser, MachLogReachedLocationCondition::BooleanType booleanType )
{
	//format of a LOW_UNIT_COUNT/HIGH_UNIT_COUNT condition line is:
	//<keyName> RACE <race> <number> <objectType> [<hwLevel>] [<subType>] [<weaponCombo>]

	MachLogReachedLocationCondition* pResult =
			_NEW( MachLogReachedLocationCondition(
				pParser->tokens()[1],
				MachLogScenario::machPhysRace( pParser->tokens()[3] ),
				atol( pParser->tokens()[4].c_str() ),
				MexPoint2d( 0, 0, 0 ),
				0 ) );

	pResult->booleanType_ = booleanType;
	pResult->doRangeCheck_ = false;

	if( pParser->tokens()[5] == "ANY" )
		pResult->anyTypeWillDo_ = true;
	else if( pParser->tokens()[5] == "MACHINE" )
		pResult->machineWillDo_ = true;
	else if( pParser->tokens()[5] == "CONSTRUCTION" )
		pResult->constructionWillDo_ = true;
	else
	{
		//objectType must be set
		pResult->objectType_ = MachLogScenario::objectType( pParser->tokens()[5] );
	}
	if( pResult->objectType_ == MachLog::ARTEFACT )
	{
		pResult->subType_ = atol( pParser->tokens()[6].c_str() );
	}
	else
	{
		//All other switch parameters are optional.
		if( pParser->tokens().size() >= 7 )
		{
			pResult->hwLevelSet_ = true;
			pResult->hwLevel_ = atol( pParser->tokens()[6].c_str() );
			if( pParser->tokens().size() >= 8 )
			{
				pResult->subTypeSet_ = true;
				pResult->subType_ = MachLogScenario::objectSubType( pResult->objectType_, pParser->tokens()[7] );
				if( pParser->tokens().size() >= 9 )
				{
					pResult->weaponComboSet_ = true;
					pResult->weaponCombo_ = MachLogScenario::weaponCombo( pParser->tokens()[8] );
				}
			}
		}
	}
	return pResult;
}

void MachLogReachedLocationCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogReachedLocationCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogReachedLocationCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 5;
	return value;
}

//virtual
void MachLogReachedLocationCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogReachedLocationCondition " << (void*)this << " start" << std::endl;
	o << "objectType_ " << objectType_ << race_ << " number " << number_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogReachedLocationCondition& reachedLocation )
{
	const SimCondition& base1 = reachedLocation;

	ostr << base1;
	ostr << reachedLocation.race_;
	ostr << reachedLocation.point_;
	ostr << reachedLocation.sqrRange_;
	ostr << reachedLocation.number_;
	ostr << reachedLocation.anyTypeWillDo_;
	ostr << reachedLocation.machineWillDo_;
	ostr << reachedLocation.constructionWillDo_;
	ostr << reachedLocation.objectType_;
	ostr << reachedLocation.subTypeSet_;
	ostr << reachedLocation.subType_;
	ostr << reachedLocation.hwLevelSet_;
	ostr << reachedLocation.hwLevel_;
	ostr << reachedLocation.weaponComboSet_;
	ostr << reachedLocation.weaponCombo_;
	ostr << reachedLocation.doRangeCheck_;
	ostr << reachedLocation.booleanType_;

}

void perRead( PerIstream& istr, MachLogReachedLocationCondition& reachedLocation )
{
	SimCondition& base1 = reachedLocation;

	istr >> base1;
	istr >> reachedLocation.race_;
	istr >> reachedLocation.point_;
	istr >> reachedLocation.sqrRange_;
	istr >> reachedLocation.number_;
	istr >> reachedLocation.anyTypeWillDo_;
	istr >> reachedLocation.machineWillDo_;
	istr >> reachedLocation.constructionWillDo_;
	istr >> reachedLocation.objectType_;
	istr >> reachedLocation.subTypeSet_;
	istr >> reachedLocation.subType_;
	istr >> reachedLocation.hwLevelSet_;
	istr >> reachedLocation.hwLevel_;
	istr >> reachedLocation.weaponComboSet_;
	istr >> reachedLocation.weaponCombo_;
	istr >> reachedLocation.doRangeCheck_;
	istr >> reachedLocation.booleanType_;
}

MachLogReachedLocationCondition::MachLogReachedLocationCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
