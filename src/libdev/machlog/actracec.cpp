/*
 * A C T R A C E C . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machlog/actracec.hpp"
#include "machlog/actor.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"

PER_DEFINE_PERSISTENT( MachLogChangeRaceAction );

MachLogChangeRaceAction::MachLogChangeRaceAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled )
{
    TEST_INVARIANT;
}

//virtual
MachLogChangeRaceAction::~MachLogChangeRaceAction()
{
    TEST_INVARIANT;
}

void MachLogChangeRaceAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogChangeRaceAction& t )
{

    o << "MachLogChangeRaceAction " << (void*)&t << " start" << std::endl;
    o << "MachLogChangeRaceAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachLogChangeRaceAction::doAction()
{
	HAL_STREAM("MachLogChangeRaceAction::doAction()\n" );
	const MATHEX_SCALAR sqrRange = range_ * range_;
	HAL_STREAM(" sqr Range check " << sqrRange << std::endl );
	MachLogRaces::Objects actorsToChange;

	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin(); i != MachLogRaces::instance().objects().end(); ++i	 )
		if( (*i)->race() == originalRace_ and dest_.sqrEuclidianDistance( (*i)->position() ) < sqrRange )
		{
			actorsToChange.push_back( *i );
		}
	//have to determine which to change before changing them as the objects collection will get reset halfway through
	for( MachLogRaces::Objects::iterator i = actorsToChange.begin(); i != actorsToChange.end(); ++i	 )
	{
		(*i)->assignToDifferentRace( MachLogRaces::instance().race( targetRace_ ) );
		(*i)->changeRaceStartTime( 0.0 );
	}
}

//static 
MachLogChangeRaceAction* MachLogChangeRaceAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	MachLogChangeRaceAction* pResult = NULL;
	pResult = _NEW( MachLogChangeRaceAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "ORIGINAL_RACE" )
			pResult->originalRace_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( token == "TARGET_RACE" )
			pResult->targetRace_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( token == "AT" )
			pResult->dest_ = MexPoint2d( atof( pParser->tokens()[i+1].c_str() ), atof( pParser->tokens()[i+2 ].c_str() ) );
		else if( token == "RANGE" )
			pResult->range_ = atof( pParser->tokens()[i+1].c_str() );
	}
	return pResult;
}

//virtual
void MachLogChangeRaceAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << " Original Race " << originalRace_;
	o << " Target Race " << targetRace_;
	o << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogChangeRaceAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.originalRace_;
	ostr << action.targetRace_;
	ostr << action.dest_;
	ostr << action.range_;

}

void perRead( PerIstream& istr, MachLogChangeRaceAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.originalRace_;
	istr >> action.targetRace_;
	istr >> action.dest_;
	istr >> action.range_;
}

MachLogChangeRaceAction::MachLogChangeRaceAction( PerConstructor con )
:	SimAction( con )
{
}

/* End ACTREINF.CPP *************************************************/
