/*
 * M A C H I N E I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include  "stdlib/string.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/internal/machinei.hpp"

#include "machphys/machdata.hpp"
#include "machphys/locomoti.hpp"

PER_DEFINE_PERSISTENT( MachPhysMachineImpl );

// Should only be called by the one time constructor of MachPhysMachine
MachPhysMachineImpl::MachPhysMachineImpl()
: maxTranslationSpeed_( 0.0 ),
  maxTranslationAcceleration_( 0.0 ),
  maxRotationSpeed_( 0.0 ),
  maxRotationAcceleration_( 0.0 ),
  pLocomotionMethod_( NULL ),
  pExplosion_( NULL ),
  explosionDataPtr_( _NEW( MachPhysMachineExplosionData() ) ),
  pFaceplate_( NULL ),
  pDamageData_( NULL ),
  pUpperBodyTurnLink_( NULL ),
  pTurnerTracker_( NULL ),
  machineControlType_( MachPhysMachine::OTHER )
{

    TEST_INVARIANT;
}

MachPhysMachineImpl::MachPhysMachineImpl
(
    const MachPhysMachineImpl& copyMe, const MachPhysMachineData& data, MachPhys::Race race,
    size_t bodyLevel, size_t brainLevel
)
: maxTranslationSpeed_( data.speed() ),
  maxTranslationAcceleration_( data.acceleration() ),
  maxRotationSpeed_( data.rotationSpeed() ),
  maxRotationAcceleration_( data.rotationAcceleration() ),
  race_( race ),
  pFaceplate_( NULL ),
  pExplosion_( NULL ),
  explosionDataPtr_( copyMe.explosionDataPtr_ ),
  pLocomotionMethod_( NULL ),
  brainLevel_( brainLevel ),
  bodyLevel_( bodyLevel ),
  shakeLinks_( copyMe.shakeLinks_ ),
  pDamageData_( NULL ),
  pUpperBodyTurnLink_( NULL ),
  pTurnerTracker_( NULL ),
  machineControlType_( MachPhysMachine::OTHER )
{

    TEST_INVARIANT;
}

MachPhysMachineImpl::~MachPhysMachineImpl()
{
    TEST_INVARIANT;

}

void MachPhysMachineImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachineImpl& t )
{

    o << "MachPhysMachineImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysMachineImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysMachineImpl& machine )
{
    ostr << machine.maxTranslationSpeed_;
    ostr << machine.maxRotationSpeed_;
    ostr << machine.maxTranslationAcceleration_;
    ostr << machine.maxRotationAcceleration_;
    ostr << machine.pLocomotionMethod_;
    ostr << machine.pFaceplate_;
    ostr << machine.shakeLinks_;
    ostr << machine.race_;
    ostr << machine.explosionDataPtr_;
    ostr << machine.bodyLevel_;
    ostr << machine.brainLevel_;
    ostr << machine.pUpperBodyTurnLink_;
}

void perRead( PerIstream& istr, MachPhysMachineImpl& machine )
{
    istr >> machine.maxTranslationSpeed_;
    istr >> machine.maxRotationSpeed_;
    istr >> machine.maxTranslationAcceleration_;
    istr >> machine.maxRotationAcceleration_;
    istr >> machine.pLocomotionMethod_;
    istr >> machine.pFaceplate_;
    istr >> machine.shakeLinks_;
    istr >> machine.race_;
    istr >> machine.explosionDataPtr_;
    istr >> machine.bodyLevel_;
    istr >> machine.brainLevel_;
    istr >> machine.pUpperBodyTurnLink_;
}

MachPhysMachineImpl::MachPhysMachineImpl( PerConstructor )
: maxTranslationSpeed_( 0.0 ),
  maxTranslationAcceleration_( 0.0 ),
  maxRotationSpeed_( 0.0 ),
  maxRotationAcceleration_( 0.0 ),
  pLocomotionMethod_( NULL ),
  pExplosion_( NULL ),
  pFaceplate_( NULL ),
  pDamageData_( NULL ),
  pTurnerTracker_( NULL ),
  machineControlType_( MachPhysMachine::OTHER )
{
}

/* End MACHINEI.CPP *************************************************/
