/*
 * A C T TASKS . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/point2d.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machlog/acttasks.hpp"
#include "machlog/scenario.hpp"
#include "machlog/races.hpp"
#include "machlog/dbhandlr.hpp"

PER_DEFINE_PERSISTENT( MachLogTaskStateAction );

MachLogTaskStateAction::MachLogTaskStateAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled ),
    taskIndex_( 1 ),
    isAvailable_( true ),
    isComplete_( false )
{
    TEST_INVARIANT;
}

//virtual
MachLogTaskStateAction::~MachLogTaskStateAction()
{
    TEST_INVARIANT;
}

void MachLogTaskStateAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogTaskStateAction& t )
{

    o << "MachLogTaskStateAction " << (void*)&t << " start" << std::endl;
    o << "MachLogTaskStateAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachLogTaskStateAction::doAction()
{
	MachLogRaces& races = MachLogRaces::instance();
	races.databaseHandler().taskIsAvailable( taskIndex_ - 1, isAvailable_ );
	races.databaseHandler().taskIsComplete( taskIndex_ - 1, isComplete_ );
}

//static 
MachLogTaskStateAction* MachLogTaskStateAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
    ASSERT_INFO( pParser->line() );

	MachLogTaskStateAction* pResult = NULL;
	pResult = _NEW( MachLogTaskStateAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "NUMBER" )
        {
            ASSERT( pParser->tokens().size() >= i+3, "Missing tokens" );
            uint taskIndex = atol( pParser->tokens()[i+1].c_str() );

            ASSERT( taskIndex <= MachLogRaces::instance().databaseHandler().nTasksInCurrentScenario(), "Task number out of range" );
			pResult->taskIndex_ = taskIndex;

            bool isAvailable = true;
            bool isComplete = false;
            if( pParser->tokens()[i+2] == "COMPLETE" )
                isComplete = true;
            else
            {
                ASSERT( pParser->tokens()[i+2] == "OPEN", "COMPLETE or OPEN expected" );
            }

            pResult->isAvailable_ = isAvailable;
            pResult->isComplete_ = isComplete;
        }
	}
	return pResult;
}

//virtual
void MachLogTaskStateAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Index " << taskIndex_ << " available " << isAvailable_ << " complete " << isComplete_ << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogTaskStateAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.taskIndex_;
	ostr << action.isAvailable_;
	ostr << action.isComplete_;

}

void perRead( PerIstream& istr, MachLogTaskStateAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.taskIndex_;
	istr >> action.isAvailable_;
	istr >> action.isComplete_;
}

MachLogTaskStateAction::MachLogTaskStateAction( PerConstructor con )
:	SimAction( con )
{
}

//static 
MachLogTaskStateAction* MachLogTaskStateAction::newDynamic
(
    SimCondition* pCondition, bool enabled, uint taskIndex, bool isAvailable, bool isComplete
)
{
	MachLogTaskStateAction* pResult = NULL;
	pResult = _NEW( MachLogTaskStateAction( pCondition, enabled ) );
	pResult->taskIndex_ = taskIndex;
	pResult->isAvailable_ = isAvailable;
	pResult->isComplete_ = isComplete;
	return pResult;
}

/* End ACTTASKS.CPP *************************************************/
