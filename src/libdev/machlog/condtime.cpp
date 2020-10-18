/*
 * C O N D T I M E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condtime.hpp"


PER_DEFINE_PERSISTENT( MachLogTimerCondition );

MachLogTimerCondition::MachLogTimerCondition( const string& keyName, const PhysAbsoluteTime& fireAtTime )
:	SimCondition( keyName ),
	fireAtTime_( fireAtTime ),
	originalFireAtTime_( fireAtTime )
{
    TEST_INVARIANT;
}

MachLogTimerCondition::~MachLogTimerCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogTimerCondition::doHasConditionBeenMet() const
{
	HAL_STREAM("MachLogTimerCondition:;doHasConditionBeenMet fire at time: " << fireAtTime_ << std::endl );
	bool result = false;
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	if( now >= fireAtTime_ )
		result = true;
	return result;
}

//static
MachLogTimerCondition* MachLogTimerCondition::newFromParser( UtlLineTokeniser* pParser )
{
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	return _NEW( MachLogTimerCondition( pParser->tokens()[1], atof( pParser->tokens()[2].c_str() ) + now ) );
}

void MachLogTimerCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogTimerCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogTimerCondition::recommendedCallBackTimeGap() const
{
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	// TODO another fix?
	static PhysAbsoluteTime value;
	value = fireAtTime_ - now;
	return value;
}

//virtual
void MachLogTimerCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogTimerCondition " << (void*)this << " start" << std::endl;
	o << " fireAtTime_ " << fireAtTime_ << " originalFireAtTime_ " << originalFireAtTime_ << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogTimerCondition& timer )
{
	const SimCondition& base1 = timer;

	ostr << base1;
	ostr << timer.fireAtTime_;
	ostr << timer.originalFireAtTime_;

}

void perRead( PerIstream& istr, MachLogTimerCondition& timer )
{
	SimCondition& base1 = timer;

	istr >> base1;
	istr >> timer.fireAtTime_;
	istr >> timer.originalFireAtTime_;
}

MachLogTimerCondition::MachLogTimerCondition( PerConstructor con )
:	SimCondition( con )
{
}

//virtual
void MachLogTimerCondition::doBecomeEnabled()
{
	fireAtTime_ = originalFireAtTime_ + SimManager::instance().currentTime();
}

/* End CONDTIME.CPP *************************************************/
