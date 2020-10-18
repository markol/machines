/*
 * C O N D N O T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condnot.hpp"


PER_DEFINE_PERSISTENT( MachLogNotCondition );

MachLogNotCondition::MachLogNotCondition( const string& keyName, SimCondition* pNotCondition )
:	SimCondition( keyName ),
	pNotCondition_( pNotCondition )
{
    TEST_INVARIANT;
	pNotCondition->incrementLinkedActionCount();
}

MachLogNotCondition::~MachLogNotCondition()
{
    TEST_INVARIANT;
	pNotCondition_->decrementLinkedActionCount();
    if( pNotCondition_->nLinkedActions() == 0 )
		_DELETE( pNotCondition_ );

}

//virtual
bool MachLogNotCondition::doHasConditionBeenMet() const
{
	return not pNotCondition_->hasConditionBeenMet();
}

//static
MachLogNotCondition* MachLogNotCondition::newFromParser( UtlLineTokeniser* pParser, ConditionMap* pMap )
{
	SimCondition* pNotCondition = NULL;
	ASSERT_INFO( pParser->tokens()[2] )
	pNotCondition = pMap->operator[]( pParser->tokens()[2] );
	return _NEW( MachLogNotCondition( pParser->tokens()[1], pNotCondition ) );
}

void MachLogNotCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNotCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogNotCondition::recommendedCallBackTimeGap() const
{
    static const PhysRelativeTime value = 1.54;
	return value;
}

//virtual
void MachLogNotCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogNotCondition " << (void*)this << " start" << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogNotCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.pNotCondition_;

}

void perRead( PerIstream& istr, MachLogNotCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.pNotCondition_;
}

MachLogNotCondition::MachLogNotCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
