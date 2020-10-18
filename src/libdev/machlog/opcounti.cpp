/*
 * O P C O U N T I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opcounti.hpp"

#include "sim/manager.hpp"

#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/opcounte.hpp"
#include "machlog/operatio.hpp"

#include "mathex/point2d.hpp"


PER_DEFINE_PERSISTENT( MachLogCounterattackOperationImpl );

MachLogCounterattackOperationImpl::MachLogCounterattackOperationImpl( MachLogMachine* pActor, MachActor* pTarget )
:	pActor_( pActor ),
	pTarget_( pTarget ),
	finished_( false ),
	initiatedAttackOp_( false ),
	pCachedOperation_( NULL ),
	distanceBeyondWeaponRangeLastTimeIChecked_( 0.0 )		// dummy assignment
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	lastTimeTargetWasntEvading_ = timeNow;
	nextTimeINeedToCheckImClosing_ = timeNow + MachLogCounterattackOperation::distanceBeyondWeaponRangeCheckInterval() + _STATIC_CAST( MATHEX_SCALAR, MachPhysRandom::randomInt( 0, 11 ) );
	
    TEST_INVARIANT;
}

MachLogCounterattackOperationImpl::~MachLogCounterattackOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogCounterattackOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogCounterattackOperationImpl& t )
{

    o << "MachLogCounterattackOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogCounterattackOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogCounterattackOperationImpl& counterattackOpImpl )
{
	ostr << counterattackOpImpl.pActor_;
	ostr << counterattackOpImpl.pTarget_;
	ostr << counterattackOpImpl.finished_;
	ostr << counterattackOpImpl.initiatedAttackOp_;	
	ostr << counterattackOpImpl.pCachedOperation_;		
	ostr << counterattackOpImpl.lastTimeTargetWasntEvading_;
	ostr << counterattackOpImpl.distanceBeyondWeaponRangeLastTimeIChecked_;		
	ostr << counterattackOpImpl.nextTimeINeedToCheckImClosing_;														
}

void perRead( PerIstream& istr, MachLogCounterattackOperationImpl& counterattackOpImpl )
{
	istr >> counterattackOpImpl.pActor_;
	istr >> counterattackOpImpl.pTarget_;
	istr >> counterattackOpImpl.finished_;
	istr >> counterattackOpImpl.initiatedAttackOp_;	
	istr >> counterattackOpImpl.pCachedOperation_;													
	istr >> counterattackOpImpl.lastTimeTargetWasntEvading_;													
	istr >> counterattackOpImpl.distanceBeyondWeaponRangeLastTimeIChecked_;		
	istr >> counterattackOpImpl.nextTimeINeedToCheckImClosing_;
}

MachLogCounterattackOperationImpl::MachLogCounterattackOperationImpl( PerConstructor )
{
}


/* End OPCOUNTI.CPP ***************************************************/
