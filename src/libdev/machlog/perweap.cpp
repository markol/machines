/*
 * P E R W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "base/persist.hpp"

#include "machphys/machphys.hpp"

#include "machlog/machlog.hpp"
#include "machlog/perweap.hpp"
#include "machlog/elecharg.hpp"
#include "machlog/bolter.hpp"
#include "machlog/flamethr.hpp"
#include "machlog/mulaunch.hpp"
#include "machlog/plasmawe.hpp"
#include "machlog/pulsewep.hpp"
#include "machlog/vortweap.hpp"
#include "machlog/ionweap.hpp"
#include "machlog/charger.hpp"
#include "machlog/treachry.hpp"
#include "machlog/nuclweap.hpp"
#include "machlog/punweap.hpp"
#include "machlog/lmisslch.hpp"
#include "machlog/beeweap.hpp"
#include "machlog/stglwep.hpp"
#include "machlog/stgmwep.hpp"

MachLogWeaponPersistence::MachLogWeaponPersistence()
{

    TEST_INVARIANT;
}

MachLogWeaponPersistence::~MachLogWeaponPersistence()
{
    TEST_INVARIANT;

}

void MachLogWeaponPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogWeaponPersistence& t )
{

    o << "MachLogWeaponPersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogWeaponPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

//static 
void MachLogWeaponPersistence::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogBolterWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogElectroCharger );
	PER_REGISTER_DERIVED_CLASS( MachLogIonWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogVortexWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogMultiLauncher );
	PER_REGISTER_DERIVED_CLASS( MachLogPulseWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogPlasmaWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogFlameThrower );
	PER_REGISTER_DERIVED_CLASS( MachLogSuperCharger );
	PER_REGISTER_DERIVED_CLASS( MachLogTreacheryWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogNuclearWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogPunchWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogLargeMissileLauncher );
	PER_REGISTER_DERIVED_CLASS( MachLogBeeBomber );
	PER_REGISTER_DERIVED_CLASS( MachLogLightStingWeapon );
	PER_REGISTER_DERIVED_CLASS( MachLogMetalStingWeapon );
}
/* End PERWEAP.CPP **************************************************/
