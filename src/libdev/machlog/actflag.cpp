/*
 * A C T FLAG . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machlog/actflag.hpp"
#include "machlog/scenario.hpp"
#include "machlog/races.hpp"
#include "machlog/dbhandlr.hpp"

PER_DEFINE_PERSISTENT( MachLogSetFlagAction );

MachLogSetFlagAction::MachLogSetFlagAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled )
{
    TEST_INVARIANT;
}

//virtual
MachLogSetFlagAction::~MachLogSetFlagAction()
{
    TEST_INVARIANT;
}

void MachLogSetFlagAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogSetFlagAction& t )
{

    o << "MachLogSetFlagAction " << (void*)&t << " start" << std::endl;
    o << "MachLogSetFlagAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachLogSetFlagAction::doAction()
{
	MachLogRaces::instance().databaseHandler().setScenarioFlag( flagName_ );
}

//static 
MachLogSetFlagAction* MachLogSetFlagAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	MachLogSetFlagAction* pResult = NULL;
	pResult = _NEW( MachLogSetFlagAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "NAME" )
			pResult->flagName_ = pParser->tokens()[i+1];
	}
	return pResult;
}

//virtual
void MachLogSetFlagAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Flag " << flagName_ << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogSetFlagAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.flagName_;

}

void perRead( PerIstream& istr, MachLogSetFlagAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.flagName_;
}

MachLogSetFlagAction::MachLogSetFlagAction( PerConstructor con )
:	SimAction( con )
{
}

//static 
MachLogSetFlagAction* MachLogSetFlagAction::newDynamic( SimCondition* pCondition, bool enabled, const string& flagName )
{
	MachLogSetFlagAction* pResult = NULL;
	pResult = _NEW( MachLogSetFlagAction( pCondition, enabled ) );
	pResult->flagName_ = flagName;
	return pResult;
}

/* End ACTFLAG.CPP *************************************************/
