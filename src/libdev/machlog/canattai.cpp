/*
 * C A N A T T A I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/canattai.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/weapon.hpp"

PER_DEFINE_PERSISTENT( MachLogCanAttackImpl )

MachLogCanAttackImpl::MachLogCanAttackImpl( MachActor* pMe, MachPhysCanAttack* pPhysCanAttack, MachPhys::WeaponCombo wc )
:	pCurrentTarget_( NULL ),
	currentlyAttached_( false ),
	weaponCombo_( wc ),
	lastFireFrame_( 0 ),
    pMe_( pMe ),
    pPhysCanAttack_( pPhysCanAttack ),
	lastFireDiceRoll_( 0 ),
	maxWeaponRange_( 0 ),
	alertness_( 100 )			// born totally alert
{

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();	
	lastFireTime_ = timeNow;
	lastAlertnessDiminishTime_ = timeNow;
	nextTimeAllowedToCheckAndAttack_ = timeNow;
	nextHitByTestTime_ = timeNow;
	 
    TEST_INVARIANT;
}

MachLogCanAttackImpl::~MachLogCanAttackImpl()
{
    TEST_INVARIANT;
}

void MachLogCanAttackImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogCanAttackImpl& t )
{

    o << "MachLogCanAttackImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogCanAttackImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogCanAttackImpl& actorImpl )
{
	ostr << actorImpl.pCurrentTarget_;
	ostr << actorImpl.currentlyAttached_;
	ostr << actorImpl.weaponCombo_;
	ostr << actorImpl.weapons_;
	ostr << actorImpl.lastFireFrame_;
	ostr << actorImpl.pMe_;
	ostr << actorImpl.minimumInaccuracy_;
	ostr << actorImpl.currentInaccuracy_;
	ostr << actorImpl.lastFireDiceRoll_;
	ostr << actorImpl.alertness_;	
	ostr << actorImpl.nextTimeAllowedToCheckAndAttack_;		
	ostr << actorImpl.maxWeaponRange_;		
}

void perRead( PerIstream& istr, MachLogCanAttackImpl& actorImpl )
{
	istr >> actorImpl.pCurrentTarget_;
	istr >> actorImpl.currentlyAttached_;
	istr >> actorImpl.weaponCombo_;
	istr >> actorImpl.weapons_;
	istr >> actorImpl.lastFireFrame_;
	istr >> actorImpl.pMe_;
	istr >> actorImpl.minimumInaccuracy_;
	istr >> actorImpl.currentInaccuracy_;
	istr >> actorImpl.lastFireDiceRoll_;
	istr >> actorImpl.alertness_;
	istr >> actorImpl.nextTimeAllowedToCheckAndAttack_;	
	istr >> actorImpl.maxWeaponRange_;		

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();	
	actorImpl.lastFireTime_ = timeNow;
	actorImpl.lastAlertnessDiminishTime_ = timeNow;
	actorImpl.nextHitByTestTime_ = timeNow;
}

MachLogCanAttackImpl::MachLogCanAttackImpl( PerConstructor )
{
} 


/* End CANATTAI.CPP *************************************************/
