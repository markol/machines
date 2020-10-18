/*
 * M C D M G D A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/mcdmgdat.hpp"
#include "machphys/machine.hpp"
#include "machphys/machburn.hpp"
#include "machphys/machdmg.hpp"

MachPhysMachineDamageData::MachPhysMachineDamageData( MachPhysMachine* pTarget )
:   pTargetMachine_( pTarget ),
	pMachineBurning_( NULL ),
    pMachineDamage_( NULL )
{
    TEST_INVARIANT;
}

MachPhysMachineDamageData::~MachPhysMachineDamageData()
{
	_DELETE( pMachineBurning_ );
	_DELETE( pMachineDamage_ );

    TEST_INVARIANT;

}

void MachPhysMachineDamageData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachineDamageData& t )
{

    o << "MachPhysMachineDamageData " << (void*)&t << " start" << std::endl;
    o << "MachPhysMachineDamageData " << (void*)&t << " end" << std::endl;

    return o;
}

PER_DEFINE_PERSISTENT( MachPhysMachineDamageData );
MachPhysMachineDamageData::MachPhysMachineDamageData( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysMachineDamageData& t )
{
	ostr << t.pTargetMachine_;
	ostr << t.pMachineBurning_;
	ostr << t.pMachineDamage_;
}

void perRead( PerIstream& istr, MachPhysMachineDamageData& t )
{
	istr >> t.pTargetMachine_;
	istr >> t.pMachineBurning_;
	istr >> t.pMachineDamage_;
}


bool MachPhysMachineDamageData::isDamaged() const
{
	return pMachineDamage_ != NULL;
}

void MachPhysMachineDamageData::damageLevel( const double& percent )
{
	if( percent>0 )
	{
		if( not isDamaged() )
			pMachineDamage_ = _NEW(MachPhysMachineDamage( pTargetMachine_ ) );

		pMachineDamage_->damageLevel(percent);
	}
}

const double& MachPhysMachineDamageData::damageLevel() const
{
    static const double zero = 0;
	return (pMachineDamage_==NULL? zero : pMachineDamage_->damageLevel() );
}

void MachPhysMachineDamageData::updateDamageLevel()
{
	if( isDamaged() )
	{
		pMachineDamage_->update();
		if( damageLevel()>=100 ) pTargetMachine_->beDestroyed();
	}
}

MachPhysMachineBurning& MachPhysMachineDamageData::machineBurning()
{
	if( not isBurning() )
		pMachineBurning_ = _NEW( MachPhysMachineBurning( pTargetMachine_ ) );

	return *pMachineBurning_;
}

void MachPhysMachineDamageData::finishBurning()
{
    //Check for finished burning
    if( isBurningFinished() )
    {
        _DELETE( pMachineBurning_ );
        pMachineBurning_ = NULL;
    }
}

bool MachPhysMachineDamageData::isBurning() const
{
	return pMachineBurning_ != NULL;
}

bool MachPhysMachineDamageData::isBurningFinished() const
{
	return 	isBurning()? pMachineBurning_->isBurningFinished():false;
}

/* End MCDMGDAT.CPP *************************************************/
