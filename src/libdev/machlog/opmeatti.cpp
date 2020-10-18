/*
 * O P E V A D E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opmeatti.hpp"

#include "machlog/actor.hpp"
#include "machlog/missilem.hpp"


#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogMissileEmplacementAttackOperationImpl );

MachLogMissileEmplacementAttackOperationImpl::MachLogMissileEmplacementAttackOperationImpl(
																	MachLogMissileEmplacement * pActor,
												  					MachActor * pDirectObject, 
												  					MachLogAttackOperation::RaceChangeTerminate rct )
:	pActor_( pActor ),
	currentlyAttached_( false ),
  	pDirectObject_( pDirectObject ),
  	targetBehindCover_( false ),
	raceChangeTerminate_( rct ),
	lastTargetPosition_( -5000.0, -5000.0 )
{
    TEST_INVARIANT;
}

MachLogMissileEmplacementAttackOperationImpl::~MachLogMissileEmplacementAttackOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogMissileEmplacementAttackOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMissileEmplacementAttackOperationImpl& t )
{

    o << "MachLogMissileEmplacementAttackOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogMissileEmplacementAttackOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMissileEmplacementAttackOperationImpl& missileEmplacementAttackOpImpl )
{
	ostr << missileEmplacementAttackOpImpl.pActor_;
	ostr << missileEmplacementAttackOpImpl.pDirectObject_;
	ostr << missileEmplacementAttackOpImpl.currentlyAttached_;
    ostr << missileEmplacementAttackOpImpl.lastTargetPosition_;
	ostr << missileEmplacementAttackOpImpl.targetBehindCover_;
	ostr << missileEmplacementAttackOpImpl.raceChangeTerminate_;
}

void perRead( PerIstream& istr, MachLogMissileEmplacementAttackOperationImpl& missileEmplacementAttackOpImpl )
{
	istr >> missileEmplacementAttackOpImpl.pActor_;
	istr >> missileEmplacementAttackOpImpl.pDirectObject_;
	istr >> missileEmplacementAttackOpImpl.currentlyAttached_;
    istr >> missileEmplacementAttackOpImpl.lastTargetPosition_;
	istr >> missileEmplacementAttackOpImpl.targetBehindCover_;
	istr >> missileEmplacementAttackOpImpl.raceChangeTerminate_;
}

MachLogMissileEmplacementAttackOperationImpl::MachLogMissileEmplacementAttackOperationImpl( PerConstructor )
{
}


/* End OPEVADEI.CPP ***************************************************/
