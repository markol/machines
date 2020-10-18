/*
 * R E D U C T I O . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machlog/actor.hpp"
#include "machlog/reductio.hpp"

/* //////////////////////////////////////////////////////////////// */

MachReductionStrategy::MachReductionStrategy( MachActor * pActor )
: MachStrategy( pActor )
{
	/* Intentionally Empty	*/
}

MachReductionStrategy::~MachReductionStrategy()
{
	/* Intentionally Empty	*/
}

bool MachReductionStrategy::isFinished() const
{
	return stack_.empty();
}

void MachReductionStrategy::newOperation( MachOperation * pNewOperation )
{
	while( not stack_.empty() )
	{
		MachOperation * pO = stack_.top();
		if( not pO->isFinished() )
			pO->doBeInterrupted();

		_DELETE( pO );
		stack_.pop();
	}

	stack_.push( pNewOperation );
}

void MachReductionStrategy::subOperation( MachOperation * pNewOperation )
{
	stack_.push( pNewOperation );
}

PhysRelativeTime
MachReductionStrategy::update( const PhysRelativeTime& maxCPUTime )
{
	PRE( not isFinished() );

	PhysRelativeTime result = 0.0;
	MachOperation * pO = stack_.top();

	if( not pO->isFinished() )
	{
		result = pO->update( maxCPUTime );
	}
	else
	{
		_DELETE( pO );
		stack_.pop();
	}

	return result;
}

/* //////////////////////////////////////////////////////////////// */

ostream& operator <<( ostream& o, const MachReductionStrategy& s )
{
	if( s.isFinished() )
		o << "idle";
	else
		o << *s.stack_.top();

	return o;
}

/* //////////////////////////////////////////////////////////////// */

/* End REDUCTIO.CTP *************************************************/
