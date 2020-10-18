/*
 * C O N D T I M E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condanya.hpp"


PER_DEFINE_PERSISTENT( MachLogAnyAllCondition );

MachLogAnyAllCondition::MachLogAnyAllCondition( const string& keyName, const ctl_pvector< SimCondition >& subConditions, BooleanOperator booleanOperator )
:	SimCondition( keyName, subConditions, booleanOperator )
{
    TEST_INVARIANT;
}

MachLogAnyAllCondition::~MachLogAnyAllCondition()
{
    TEST_INVARIANT;

}

//virtual
//this will never be called but has to be provided
bool MachLogAnyAllCondition::doHasConditionBeenMet() const
{
	return false;
}

//static
MachLogAnyAllCondition* MachLogAnyAllCondition::newFromParser( UtlLineTokeniser* pParser, ConditionMap* pMap, SimCondition::BooleanOperator booleanOperator )
{
	ctl_pvector< SimCondition > subOps;
	for( int i = 2; i < pParser->tokens().size(); ++i )
	{
		subOps.push_back( pMap->operator[]( pParser->tokens()[i] ) );
	}
	return _NEW( MachLogAnyAllCondition( pParser->tokens()[1], subOps, booleanOperator ) );
}

void MachLogAnyAllCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAnyAllCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogAnyAllCondition::recommendedCallBackTimeGap() const
{
    static const PhysRelativeTime value = 2.1;
	return value;
}

//virtual
void MachLogAnyAllCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogAnyAllCondition " << (void*)this << " start" << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogAnyAllCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogAnyAllCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
}

MachLogAnyAllCondition::MachLogAnyAllCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
