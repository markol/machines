/*
 * A C T S Q U A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "mathex/point2d.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machlog/actsquad.hpp"
#include "machlog/scenario.hpp"
#include "machlog/races.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/optskatt.hpp"
#include "machlog/cntrl.hpp"

PER_DEFINE_PERSISTENT( MachLogActivateSquadronsAction );

MachLogActivateSquadronsAction::MachLogActivateSquadronsAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled )
{
    TEST_INVARIANT;
}

//virtual
MachLogActivateSquadronsAction::~MachLogActivateSquadronsAction()
{
    TEST_INVARIANT;
}

void MachLogActivateSquadronsAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogActivateSquadronsAction& t )
{

    o << "MachLogActivateSquadronsAction " << (void*)&t << " start" << std::endl;
    o << "MachLogActivateSquadronsAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachLogActivateSquadronsAction::doAction()
{
	MachLogRaces& races = MachLogRaces::instance();
	PRE( races.controller( race_ ).type() == MachLogController::AI_CONTROLLER );
	MachLogRaces::Squadrons& squadrons = races.squadrons( race_ );
	for( MachLogRaces::Squadrons::iterator i = squadrons.begin(); i != squadrons.end(); ++i )
	{
		for( Squadrons::iterator j = squadrons_.begin(); j != squadrons_.end(); ++j )
		{
			if( (*i)->squadronId() == *j )
			{
				if( (*i)->strategy().currentOperationType() == MachLogOperation::TASK_ATTACK_OPERATION )
                {
    				MachLogTaskAttackOperation& task = _STATIC_CAST( MachLogTaskAttackOperation&, (*i)->strategy().currentOperation() );
    				task.initialDelay( 0 );
                }
			}
		}
	}

}

//static
MachLogActivateSquadronsAction* MachLogActivateSquadronsAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	MachLogActivateSquadronsAction* pResult = NULL;
	pResult = _NEW( MachLogActivateSquadronsAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "RACE" )
			pResult->race_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( token == "SQUADRONS" )
		{
			for( int j = i+1; j < pParser->tokens().size(); ++j )
				pResult->squadrons_.push_back( atol( pParser->tokens()[j].c_str() ) );
		}
	}
	return pResult;
}

//virtual
void MachLogActivateSquadronsAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Race " << race_ << std::endl;
	o << "squadrons: " << squadrons_ << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogActivateSquadronsAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.race_;
	ostr << action.squadrons_;

}

void perRead( PerIstream& istr, MachLogActivateSquadronsAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.race_;
	istr >> action.squadrons_;
}

MachLogActivateSquadronsAction::MachLogActivateSquadronsAction( PerConstructor con )
:	SimAction( con )
{
}

/* End ACTREINF.CPP *************************************************/
