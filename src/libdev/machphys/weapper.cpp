/*
 * W E A P P E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/private/weapper.hpp"
#include "world4d/generic.hpp"
#include "machphys/weapon.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/flamethr.hpp"
#include "machphys/bolter.hpp"
#include "machphys/virusspr.hpp"
#include "machphys/elecharg.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/pulseblb.hpp"
#include "machphys/mulaunch.hpp"
#include "machphys/treachry.hpp"
#include "machphys/largemsl.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/nuclweap.hpp"
#include "machphys/vortweap.hpp"
#include "machphys/charger.hpp"
#include "machphys/punweap.hpp"
#include "machphys/objdemol.hpp"
#include "machphys/beeweap.hpp"
#include "machphys/stglwep.hpp"
#include "machphys/stgmwep.hpp"

PER_DEFINE_PERSISTENT( MachPhysWeaponPersistence );

//a pimple for the MachPhysWeaponPersistence singlton
struct MachPhysWeaponPersistenceImpl
{
    MachPhysPlasmaWeapon*   pPlasmaRifle_;
    MachPhysPlasmaWeapon*   pPlasmaCannon1_;
    MachPhysPlasmaWeapon*   pPlasmaCannon2_;

    MachPhysFlameThrower*   pFlameThrower1_;
    MachPhysFlameThrower*   pFlameThrower2_;

    MachPhysBolter*         pBolter_;
    MachPhysBolter*         pAutoCannon_;
    MachPhysBolter*         pHeavyBolter1_;
    MachPhysBolter*         pHeavyBolter2_;

    MachPhysVirusSpreader*  pVirusSpreaderLeft_;
    MachPhysVirusSpreader*  pVirusSpreaderRight_;
    MachPhysVirusSpreader*  pVirusSpreaderTop_;

    MachPhysElectroCharger* pElectroChargerLeft_;
    MachPhysElectroCharger* pElectroChargerRight_;
    MachPhysElectroCharger* pElectroChargerTop_;

    MachPhysMultiLauncher*  pMultiLauncher1_;
    MachPhysMultiLauncher*  pMultiLauncher2_;
    MachPhysMultiLauncher*  pMultiLauncher3_;
    MachPhysMultiLauncher*  pMultiLauncher4_;
    MachPhysMultiLauncher*  pMultiLauncher5_;
    MachPhysMultiLauncher*  pMultiLauncher6_;
    MachPhysMultiLauncher*  pMultiLauncher7_;

    MachPhysPulseRifle*     pPulseRifle_;
    MachPhysPulseCannon*    pPulseCannon_;
    MachPhysPulseBlob*      pPulseBlobRifle_;
    MachPhysPulseBlob*      pPulseBLobCannon_;
    MachPhysLargeMissile*   pLargeMissile_;

    MachPhysTreacheryWeapon*    pTreacheryWeapon_;

	MachPhysVortexWeapon*   pVortex_;
	MachPhysNuclearWeapon*  pNuke_;
	MachPhysIonWeapon*      pIon_;

	MachPhysSuperCharger*   pAdvancedSuperCharger_;
    MachPhysSuperCharger*   pSuperSuperCharger_;
	MachPhysPunchWeapon*    pPunch_;

	// TODO: Object demolition sequences should be in a separate class
	MachPhysObjDemolish*	pObjectDemolish_;

	MachPhysObjDemolish*	pCivilianHardwareLabL1Demolish_;
	MachPhysObjDemolish*	pCivilianHardwareLabL3Demolish_;
	void*	                pDummy1_;

	MachPhysObjDemolish*	pMilitaryHardwareLabL1Demolish_;
	MachPhysObjDemolish*	pMilitaryHardwareLabL3Demolish_;
	void*	                pDummy2_;

	MachPhysObjDemolish*	pSmelterL1Demolish_;
	MachPhysObjDemolish*	pSmelterL3Demolish_;
	MachPhysObjDemolish*	pMilitaryFactoryL4Demolish_;

	MachPhysObjDemolish*	pCivilianFactoryL1Demolish_;
	MachPhysObjDemolish*	pCivilianFactoryL3Demolish_;
	MachPhysObjDemolish*	pCivilianFactoryL5Demolish_;

	MachPhysObjDemolish*	pMilitaryFactoryL1Demolish_;
	MachPhysObjDemolish*	pMilitaryFactoryL3Demolish_;
	MachPhysObjDemolish*	pMilitaryFactoryL5Demolish_;

	MachPhysObjDemolish*	pTurretMissileEmplacementL1Demolish_;
	MachPhysObjDemolish*	pTurretMissileEmplacementL2Demolish_;
	MachPhysObjDemolish*	pTurretMissileEmplacementL3Demolish_;

	MachPhysObjDemolish*	pSentryMissileEmplacementL3Demolish_;
	MachPhysObjDemolish*	pSentryMissileEmplacementL4Demolish_;

	MachPhysObjDemolish*	pLauncherMissileEmplacementL4Demolish_;

	MachPhysObjDemolish*	pIcbmMissileEmplacementL5Demolish_;

	MachPhysObjDemolish*	pGarrisonL1Demolish_;
	void*	                pDummy4_;
	void*	                pDummy5_;

	MachPhysObjDemolish*	pMineL1Demolish_;
	MachPhysObjDemolish*	pMineL3Demolish_;
	MachPhysObjDemolish*	pMineL5Demolish_;

	MachPhysObjDemolish*	pBeaconL1Demolish_;
	MachPhysObjDemolish*	pBeaconL3Demolish_;
	MachPhysObjDemolish*	pBeaconL5Demolish_;

	MachPhysObjDemolish*	pPodL1Demolish_;

	MachPhysBeeBomber* pBee_;
	MachPhysLightStingWeapon* pLightSting_;
	MachPhysMetalStingWeapon* pMetalSting_;

	W4dGeneric* pBolterFlash_;
	W4dGeneric* pHeavyBolterFlash_;
	W4dGeneric* pAutoCannonFlash_;
};

// static
MachPhysWeaponPersistence& MachPhysWeaponPersistence::instance()
{
    static MachPhysWeaponPersistence instance_;
    return instance_;
}

MachPhysWeaponPersistence::MachPhysWeaponPersistence()
: root_( 50000 ),
  pImpl_( _NEW( MachPhysWeaponPersistenceImpl() ) )
{
  	pImpl_->pPlasmaRifle_ = NULL;
  	pImpl_->pPlasmaCannon1_ = NULL;
  	pImpl_->pPlasmaCannon2_ = NULL;
  	pImpl_->pFlameThrower1_ = NULL;
  	pImpl_->pFlameThrower2_ = NULL;
  	pImpl_->pBolter_ = NULL;
  	pImpl_->pAutoCannon_ = NULL;
  	pImpl_->pHeavyBolter1_ = NULL;
  	pImpl_->pHeavyBolter2_ = NULL;

  	pImpl_->pVirusSpreaderLeft_ = NULL;
  	pImpl_->pVirusSpreaderRight_ = NULL;
  	pImpl_->pVirusSpreaderTop_ = NULL;
  	pImpl_->pElectroChargerLeft_ = NULL;
  	pImpl_->pElectroChargerRight_ = NULL;
  	pImpl_->pElectroChargerTop_ = NULL;
  	pImpl_->pMultiLauncher1_ = NULL;
  	pImpl_->pMultiLauncher2_ = NULL;
  	pImpl_->pMultiLauncher3_ = NULL;
  	pImpl_->pMultiLauncher4_ = NULL;
  	pImpl_->pMultiLauncher5_ = NULL;
  	pImpl_->pMultiLauncher6_ = NULL;
  	pImpl_->pMultiLauncher7_ = NULL;
  	pImpl_->pPulseRifle_ = NULL;
  	pImpl_->pPulseCannon_ = NULL;
  	pImpl_->pPulseBlobRifle_ = NULL;
  	pImpl_->pPulseBLobCannon_ = NULL;
  	pImpl_->pLargeMissile_ = NULL;
  	pImpl_->pTreacheryWeapon_ = NULL;
  	pImpl_->pVortex_ = NULL;
  	pImpl_->pNuke_ = NULL;
  	pImpl_->pIon_ = NULL;
  	pImpl_->pAdvancedSuperCharger_ = NULL;
  	pImpl_->pSuperSuperCharger_ = NULL;
  	pImpl_->pPunch_ = NULL;
  	pImpl_->pObjectDemolish_ = NULL;
  	pImpl_->pCivilianHardwareLabL1Demolish_ = NULL;
  	pImpl_->pCivilianHardwareLabL3Demolish_ = NULL;
  	pImpl_->pMilitaryHardwareLabL1Demolish_ = NULL;
  	pImpl_->pMilitaryHardwareLabL3Demolish_ = NULL;
  	pImpl_->pSmelterL1Demolish_ = NULL;
  	pImpl_->pSmelterL3Demolish_ = NULL;
  	pImpl_->pCivilianFactoryL1Demolish_ = NULL;
  	pImpl_->pCivilianFactoryL3Demolish_ = NULL;
  	pImpl_->pCivilianFactoryL5Demolish_ = NULL;
  	pImpl_->pMilitaryFactoryL1Demolish_ = NULL;
  	pImpl_->pMilitaryFactoryL3Demolish_ = NULL;
  	pImpl_->pMilitaryFactoryL4Demolish_ = NULL;
  	pImpl_->pMilitaryFactoryL5Demolish_ = NULL;
  	pImpl_->pTurretMissileEmplacementL1Demolish_ = NULL;
  	pImpl_->pTurretMissileEmplacementL2Demolish_ = NULL;
  	pImpl_->pTurretMissileEmplacementL3Demolish_ = NULL;
  	pImpl_->pSentryMissileEmplacementL3Demolish_ = NULL;
  	pImpl_->pSentryMissileEmplacementL4Demolish_ = NULL;
  	pImpl_->pLauncherMissileEmplacementL4Demolish_ = NULL;
  	pImpl_->pIcbmMissileEmplacementL5Demolish_ = NULL;
  	pImpl_->pGarrisonL1Demolish_ = NULL;
  	pImpl_->pMineL1Demolish_ = NULL;
  	pImpl_->pMineL3Demolish_ = NULL;
  	pImpl_->pMineL5Demolish_ = NULL;
  	pImpl_->pBeaconL1Demolish_ = NULL;
  	pImpl_->pBeaconL3Demolish_ = NULL;
  	pImpl_->pBeaconL5Demolish_ = NULL;
  	pImpl_->pPodL1Demolish_ = NULL;
  	pImpl_->pBee_ = NULL;
  	pImpl_->pLightSting_ = NULL;
  	pImpl_->pMetalSting_ =  NULL;

	pImpl_->pBolterFlash_ =  NULL;
	pImpl_->pHeavyBolterFlash_ =  NULL;
	pImpl_->pAutoCannonFlash_ =  NULL;


    TEST_INVARIANT;
}

MachPhysWeaponPersistence::~MachPhysWeaponPersistence()
{
	_DELETE( pImpl_ );
    TEST_INVARIANT;
}

const MachPhysPlasmaWeapon& MachPhysWeaponPersistence::plasmaWeaponExemplar( MachPhys::WeaponType type )
{
    MachPhysPlasmaWeapon** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
            ppWeapon = &pImpl_->pPlasmaRifle_;
            break;
        case MachPhys::PLASMA_CANNON1:
            ppWeapon = &pImpl_->pPlasmaCannon1_;
            break;
        case MachPhys::PLASMA_CANNON2:
            ppWeapon = &pImpl_->pPlasmaCannon2_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
    {
        std::cout << "Make new plasma weapon" << std::endl;
        *ppWeapon = _NEW( MachPhysPlasmaWeapon( type ) );
    }

    return **ppWeapon;
}

const MachPhysFlameThrower& MachPhysWeaponPersistence::flameThrowerExemplar( MachPhys::WeaponType type )
{
    MachPhysFlameThrower** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::FLAME_THROWER1:
            ppWeapon = &pImpl_->pFlameThrower1_;
            break;
        case MachPhys::FLAME_THROWER2:
            ppWeapon = &pImpl_->pFlameThrower2_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
    {
        *ppWeapon = _NEW( MachPhysFlameThrower( type ) );
    }

    return **ppWeapon;
}

const MachPhysBolter& MachPhysWeaponPersistence::bolterExemplar( MachPhys::WeaponType type )
{
    MachPhysBolter** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::BOLTER:
            ppWeapon = &pImpl_->pBolter_;
            break;
        case MachPhys::AUTO_CANNON:
            ppWeapon = &pImpl_->pAutoCannon_;
            break;
        case MachPhys::HEAVY_BOLTER1:
            ppWeapon = &pImpl_->pHeavyBolter1_;
            break;
        case MachPhys::HEAVY_BOLTER2:
            ppWeapon = &pImpl_->pHeavyBolter2_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysBolter( type ) );

    return **ppWeapon;
}

const W4dGeneric& MachPhysWeaponPersistence::bolterFlashExemplar( MachPhys::WeaponType type )
{
    W4dGeneric** ppFlash = NULL;

    switch( type )
    {
        case MachPhys::BOLTER:
            ppFlash = &pImpl_->pBolterFlash_;
            break;
        case MachPhys::AUTO_CANNON:
            ppFlash = &pImpl_->pAutoCannonFlash_;
            break;
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
            ppFlash = &pImpl_->pHeavyBolterFlash_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppFlash == NULL )
        *ppFlash = MachPhysBolter::newFlash( type );

    return **ppFlash;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::objDemolishExemplar( MachPhys::DemolitionType type )
{

     PRE(type.objectType==MachPhys::NOT_CONSTRUCTION);
    MachPhysObjDemolish** ppObjectDemolish;

	  ppObjectDemolish=&pImpl_->pObjectDemolish_;
	  if (*ppObjectDemolish == NULL)
	  {
	      *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
	  }


	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::hardwareLabDemolishExemplar( MachPhys::DemolitionType type )
{

     PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
     PRE(type.constructionType==MachPhys::HARDWARE_LAB);

     MachPhysObjDemolish** ppObjectDemolish = NULL;

     switch ( type.subType )
	 {
	  case 0:
        switch ( type.level )
	    {
		 case 1:
		  ppObjectDemolish=&pImpl_->pCivilianHardwareLabL1Demolish_;
		  break;
		 case 3:
		  ppObjectDemolish=&pImpl_->pCivilianHardwareLabL3Demolish_;
		  break;
         DEFAULT_ASSERT_BAD_CASE( type.level );
	    }
	    break;
	  case 1:
        switch ( type.level )
	    {
		 case 1:
		  ppObjectDemolish=&pImpl_->pMilitaryHardwareLabL1Demolish_;
		  break;
		 case 3:
		  ppObjectDemolish=&pImpl_->pMilitaryHardwareLabL3Demolish_;
		  break;
         DEFAULT_ASSERT_BAD_CASE( type.level );
	    }
	   break;
       DEFAULT_ASSERT_BAD_CASE( type.subType );
	 }

     if (*ppObjectDemolish == NULL)
	 {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
   	 }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::smelterDemolishExemplar( MachPhys::DemolitionType type )
{

    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::SMELTER);
    MachPhysObjDemolish** ppObjectDemolish = NULL;

    switch ( type.level )
    {
		 case 1:
		  ppObjectDemolish=&pImpl_->pSmelterL1Demolish_;
		  break;
		 case 3:
		  ppObjectDemolish=&pImpl_->pSmelterL3Demolish_;
		  break;
         DEFAULT_ASSERT_BAD_CASE( type.level );
    }


   if (*ppObjectDemolish == NULL)
   {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
   }

   return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::factoryDemolishExemplar( MachPhys::DemolitionType type )
{

    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::FACTORY);
    MachPhysObjDemolish** ppObjectDemolish = NULL;

        switch ( type.subType )
		{
		  case 0:
           switch ( type.level )
           {
		    case 1:
		     ppObjectDemolish=&pImpl_->pCivilianFactoryL1Demolish_;
		     break;
		    case 3:
		     ppObjectDemolish=&pImpl_->pCivilianFactoryL3Demolish_;
		     break;
		    case 5:
		     ppObjectDemolish=&pImpl_->pCivilianFactoryL5Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
		  case 1:
           switch ( type.level )
           {
		    case 1:
		     ppObjectDemolish=&pImpl_->pMilitaryFactoryL1Demolish_;
		     break;
		    case 3:
		     ppObjectDemolish=&pImpl_->pMilitaryFactoryL3Demolish_;
		     break;
		    case 4:
		     ppObjectDemolish=&pImpl_->pMilitaryFactoryL4Demolish_;
		     break;
		    case 5:
		     ppObjectDemolish=&pImpl_->pMilitaryFactoryL5Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }

	  if (*ppObjectDemolish == NULL)
	  {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
   	  }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::missileEmplacementDemolishExemplar( MachPhys::DemolitionType type )
{
    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::MISSILE_EMPLACEMENT);

    MachPhysObjDemolish** ppObjectDemolish = NULL;

        switch ( type.subType )
		{
		  case 0:
           switch ( type.level )
           {
		    case 1:
		     ppObjectDemolish=&pImpl_->pTurretMissileEmplacementL1Demolish_;
		     break;
		    case 2:
		     ppObjectDemolish=&pImpl_->pTurretMissileEmplacementL2Demolish_;
		     break;
		    case 3:
		     ppObjectDemolish=&pImpl_->pTurretMissileEmplacementL3Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
		  case 1:
           switch ( type.level )
           {
		    case 3:
		     ppObjectDemolish=&pImpl_->pSentryMissileEmplacementL3Demolish_;
		     break;
		    case 4:
		     ppObjectDemolish=&pImpl_->pSentryMissileEmplacementL3Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
		  case 2:
           switch ( type.level )
           {
		    case 4:
		     ppObjectDemolish=&pImpl_->pLauncherMissileEmplacementL4Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
		  case 3:
           switch ( type.level )
           {
		    case 5:
		     ppObjectDemolish=&pImpl_->pIcbmMissileEmplacementL5Demolish_;
		     break;
            DEFAULT_ASSERT_BAD_CASE( type.level );
	       }
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }

	  if (*ppObjectDemolish == NULL)
	  {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
   	  }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::garrisonDemolishExemplar( MachPhys::DemolitionType type )
{
    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::GARRISON);

    MachPhysObjDemolish** ppObjectDemolish = NULL;
    switch ( type.level )
    {
	    case 1:
	     ppObjectDemolish=&pImpl_->pGarrisonL1Demolish_;
	     break;
        DEFAULT_ASSERT_BAD_CASE( type.level );
    }

    if (*ppObjectDemolish == NULL)
    {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
    }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::mineDemolishExemplar( MachPhys::DemolitionType type )
{
    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::MINE);

    MachPhysObjDemolish** ppObjectDemolish = NULL;
    switch ( type.level )
    {
	    case 1:
	     ppObjectDemolish=&pImpl_->pMineL1Demolish_;
	     break;
	    case 3:
	     ppObjectDemolish=&pImpl_->pMineL3Demolish_;
	     break;
	    case 5:
	     ppObjectDemolish=&pImpl_->pMineL5Demolish_;
	     break;
        DEFAULT_ASSERT_BAD_CASE( type.level );
    }

    if (*ppObjectDemolish == NULL)
    {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
    }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::beaconDemolishExemplar( MachPhys::DemolitionType type )
{
    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::BEACON);

    MachPhysObjDemolish** ppObjectDemolish = NULL;
    switch ( type.level )
    {
	    case 1:
	     ppObjectDemolish=&pImpl_->pBeaconL1Demolish_;
	     break;
	    case 3:
	     ppObjectDemolish=&pImpl_->pBeaconL3Demolish_;
	     break;
	    case 5:
	     ppObjectDemolish=&pImpl_->pBeaconL5Demolish_;
	     break;
        DEFAULT_ASSERT_BAD_CASE( type.level );
    }

    if (*ppObjectDemolish == NULL)
    {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
    }

	 return **ppObjectDemolish;
}

const MachPhysObjDemolish& MachPhysWeaponPersistence::podDemolishExemplar( MachPhys::DemolitionType type )
{
    PRE(not type.objectType==MachPhys::NOT_CONSTRUCTION);
    PRE(type.constructionType==MachPhys::POD);

    MachPhysObjDemolish** ppObjectDemolish;
    switch ( type.level )
    {
	    case 1:
	     ppObjectDemolish=&pImpl_->pPodL1Demolish_;
	     break;
        DEFAULT_ASSERT_BAD_CASE( type.level );
    }

    if (*ppObjectDemolish == NULL)
    {
		  if (MachPhysObjDemolish::useGenericDemolition( type ) )
		  {
			MachPhys::DemolitionType notConstructionType;
			notConstructionType.objectType=MachPhys::NOT_CONSTRUCTION;
		    *ppObjectDemolish= _NEW( MachPhysObjDemolish( instance().pRoot(), MexTransform3d(),  notConstructionType ) );
		  }
		  else
		  {
		    *ppObjectDemolish=_NEW( MachPhysObjDemolish( type ) );
		  }
    }

	 return **ppObjectDemolish;
}


const MachPhysVirusSpreader& MachPhysWeaponPersistence::virusSpreaderExemplar( MachPhys::Mounting mounting )
{
    MachPhysVirusSpreader** ppWeapon = NULL;

    switch( mounting )
    {
        case MachPhys::LEFT:
            ppWeapon = &pImpl_->pVirusSpreaderLeft_;
            break;
        case MachPhys::RIGHT:
            ppWeapon = &pImpl_->pVirusSpreaderRight_;
            break;
        case MachPhys::TOP:
            ppWeapon = &pImpl_->pVirusSpreaderTop_;
            break;
        DEFAULT_ASSERT_BAD_CASE( mounting );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysVirusSpreader( mounting ) );

    return **ppWeapon;
}

const MachPhysElectroCharger& MachPhysWeaponPersistence::electroChargerExemplar( MachPhys::Mounting mounting )
{
    MachPhysElectroCharger** ppWeapon = NULL;

    switch( mounting )
    {
        case MachPhys::LEFT:
            ppWeapon = &pImpl_->pElectroChargerLeft_;
            break;
        case MachPhys::RIGHT:
            ppWeapon = &pImpl_->pElectroChargerRight_;
            break;
        case MachPhys::TOP:
            ppWeapon = &pImpl_->pElectroChargerTop_;
            break;
        DEFAULT_ASSERT_BAD_CASE( mounting );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysElectroCharger( mounting ) );

    return **ppWeapon;
}

const MachPhysMultiLauncher& MachPhysWeaponPersistence::multiLauncherExemplar( MachPhys::WeaponType type )
{
    MachPhysMultiLauncher** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::MULTI_LAUNCHER1:
            ppWeapon = &pImpl_->pMultiLauncher1_;
            break;
        case MachPhys::MULTI_LAUNCHER2:
            ppWeapon = &pImpl_->pMultiLauncher2_;
            break;
        case MachPhys::MULTI_LAUNCHER3:
            ppWeapon = &pImpl_->pMultiLauncher3_;
            break;
        case MachPhys::MULTI_LAUNCHER4:
            ppWeapon = &pImpl_->pMultiLauncher4_;
            break;
        case MachPhys::MULTI_LAUNCHER5:
            ppWeapon = &pImpl_->pMultiLauncher5_;
            break;
        case MachPhys::MULTI_LAUNCHER6:
            ppWeapon = &pImpl_->pMultiLauncher6_;
            break;
        case MachPhys::MULTI_LAUNCHER7:
            ppWeapon = &pImpl_->pMultiLauncher7_;
            break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysMultiLauncher( type ) );

    return **ppWeapon;
}

const MachPhysPulseRifle& MachPhysWeaponPersistence::pulseRifleExemplar()
{
    if( pImpl_->pPulseRifle_ == NULL )
        pImpl_->pPulseRifle_ = _NEW( MachPhysPulseRifle() );

    return *pImpl_->pPulseRifle_;
}

const MachPhysPulseCannon& MachPhysWeaponPersistence::pulseCannonExemplar()
{
    if( pImpl_->pPulseCannon_ == NULL )
        pImpl_->pPulseCannon_ = _NEW( MachPhysPulseCannon() );

    return *pImpl_->pPulseCannon_;
}

const MachPhysPulseBlob& MachPhysWeaponPersistence::pulseBlobExemplar( MachPhys::WeaponType type )
{
    MachPhysPulseBlob** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::PULSE_RIFLE:
            ppWeapon = &pImpl_->pPulseBlobRifle_;
            break;
        case MachPhys::PULSE_CANNON:
            ppWeapon = &pImpl_->pPulseBLobCannon_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysPulseBlob( type ) );

    return **ppWeapon;
}

const MachPhysTreacheryWeapon& MachPhysWeaponPersistence::treacheryWeaponExemplar()
{
    if( pImpl_->pTreacheryWeapon_ == NULL )
        pImpl_->pTreacheryWeapon_ = _NEW( MachPhysTreacheryWeapon() );

    return *pImpl_->pTreacheryWeapon_;
}

const MachPhysLargeMissile& MachPhysWeaponPersistence::largeMissileExemplar()
{
    if( pImpl_->pLargeMissile_ == NULL )
        pImpl_->pLargeMissile_ = _NEW( MachPhysLargeMissile() );

    return *pImpl_->pLargeMissile_;
}

const MachPhysVortexWeapon& MachPhysWeaponPersistence::vortexExemplar()
{
    if( pImpl_->pVortex_ == NULL )
        pImpl_->pVortex_ = _NEW( MachPhysVortexWeapon() );

    return *pImpl_->pVortex_;
}

const MachPhysNuclearWeapon& MachPhysWeaponPersistence::nuclearExemplar()
{
    if( pImpl_->pNuke_ == NULL )
        pImpl_->pNuke_ = _NEW( MachPhysNuclearWeapon() );

    return *pImpl_->pNuke_;
}

const MachPhysIonWeapon& MachPhysWeaponPersistence::ionExemplar( MachPhys::WeaponType type )
{
    if( pImpl_->pIon_ == NULL )
        pImpl_->pIon_ = _NEW( MachPhysIonWeapon( type ) );

    return *pImpl_->pIon_;
}

const MachPhysSuperCharger& MachPhysWeaponPersistence::superChargerExemplar( MachPhys::WeaponType type )
{
    MachPhysSuperCharger** ppWeapon = NULL;

    switch( type )
    {
        case MachPhys::SUPERCHARGE_ADVANCED:
            ppWeapon = &pImpl_->pAdvancedSuperCharger_;
            break;
        case MachPhys::SUPERCHARGE_SUPER:
            ppWeapon = &pImpl_->pSuperSuperCharger_;
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    if( *ppWeapon == NULL )
        *ppWeapon = _NEW( MachPhysSuperCharger( type ) );

    return **ppWeapon;
}

const MachPhysPunchWeapon& MachPhysWeaponPersistence::punchExemplar()
{

    if( pImpl_->pPunch_ == NULL )
        pImpl_->pPunch_ = _NEW( MachPhysPunchWeapon() );

    return *pImpl_->pPunch_;
}

const MachPhysBeeBomber& MachPhysWeaponPersistence::beeExemplar()
{

    if( pImpl_->pBee_ == NULL )
        pImpl_->pBee_ = _NEW( MachPhysBeeBomber() );

    return *pImpl_->pBee_;
}

const MachPhysLightStingWeapon& MachPhysWeaponPersistence::lightStingExemplar()
{

    if( pImpl_->pLightSting_ == NULL )
        pImpl_->pLightSting_ = _NEW( MachPhysLightStingWeapon() );

    return *pImpl_->pLightSting_;
}

const MachPhysMetalStingWeapon& MachPhysWeaponPersistence::metalStingExemplar()
{

    if( pImpl_->pMetalSting_ == NULL )
        pImpl_->pMetalSting_ = _NEW( MachPhysMetalStingWeapon() );

    return *pImpl_->pMetalSting_;
}

W4dRoot* MachPhysWeaponPersistence::pRoot()
{
    return &root_;
}

void MachPhysWeaponPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWeaponPersistence& t )
{

    o << "MachPhysWeaponPersistence " << (void*)&t << " start" << std::endl;
    o << "MachPhysWeaponPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysWeaponPersistence& weaponPersist )
{
    ostr << weaponPersist.root_;

    ostr << weaponPersist.pImpl_->pPlasmaRifle_;
    ostr << weaponPersist.pImpl_->pPlasmaCannon1_;
    ostr << weaponPersist.pImpl_->pPlasmaCannon2_;

    ostr << weaponPersist.pImpl_->pFlameThrower1_;
    ostr << weaponPersist.pImpl_->pFlameThrower2_;

    ostr << weaponPersist.pImpl_->pBolter_;
    ostr << weaponPersist.pImpl_->pAutoCannon_;
    ostr << weaponPersist.pImpl_->pHeavyBolter1_;
    ostr << weaponPersist.pImpl_->pHeavyBolter2_;

    ostr << weaponPersist.pImpl_->pVirusSpreaderLeft_;
    ostr << weaponPersist.pImpl_->pVirusSpreaderRight_;
    ostr << weaponPersist.pImpl_->pVirusSpreaderTop_;

    ostr << weaponPersist.pImpl_->pElectroChargerLeft_;
    ostr << weaponPersist.pImpl_->pElectroChargerRight_;
    ostr << weaponPersist.pImpl_->pElectroChargerTop_;

    ostr << weaponPersist.pImpl_->pMultiLauncher1_;
    ostr << weaponPersist.pImpl_->pMultiLauncher2_;
    ostr << weaponPersist.pImpl_->pMultiLauncher3_;
    ostr << weaponPersist.pImpl_->pMultiLauncher4_;
    ostr << weaponPersist.pImpl_->pMultiLauncher5_;
    ostr << weaponPersist.pImpl_->pMultiLauncher6_;
    ostr << weaponPersist.pImpl_->pMultiLauncher7_;

    ostr << weaponPersist.pImpl_->pPulseRifle_;
    ostr << weaponPersist.pImpl_->pPulseCannon_;
   	ostr << weaponPersist.pImpl_->pPulseBlobRifle_;
    ostr << weaponPersist.pImpl_->pPulseBLobCannon_;

    ostr << weaponPersist.pImpl_->pLargeMissile_;
    ostr << weaponPersist.pImpl_->pTreacheryWeapon_;

    ostr << weaponPersist.pImpl_->pVortex_;
    ostr << weaponPersist.pImpl_->pNuke_;
    ostr << weaponPersist.pImpl_->pIon_;

	ostr << weaponPersist.pImpl_->pAdvancedSuperCharger_;
    ostr << weaponPersist.pImpl_->pSuperSuperCharger_;
    ostr << weaponPersist.pImpl_->pPunch_;

    ostr << weaponPersist.pImpl_->pObjectDemolish_;
    ostr << weaponPersist.pImpl_->pCivilianHardwareLabL1Demolish_;
    ostr << weaponPersist.pImpl_->pCivilianHardwareLabL3Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryHardwareLabL1Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryHardwareLabL3Demolish_;
    ostr << weaponPersist.pImpl_->pSmelterL1Demolish_;
    ostr << weaponPersist.pImpl_->pSmelterL3Demolish_;
    ostr << weaponPersist.pImpl_->pCivilianFactoryL1Demolish_;
    ostr << weaponPersist.pImpl_->pCivilianFactoryL3Demolish_;
    ostr << weaponPersist.pImpl_->pCivilianFactoryL5Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryFactoryL1Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryFactoryL3Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryFactoryL4Demolish_;
    ostr << weaponPersist.pImpl_->pMilitaryFactoryL5Demolish_;
    ostr << weaponPersist.pImpl_->pTurretMissileEmplacementL1Demolish_;
    ostr << weaponPersist.pImpl_->pTurretMissileEmplacementL2Demolish_;
    ostr << weaponPersist.pImpl_->pTurretMissileEmplacementL3Demolish_;
    ostr << weaponPersist.pImpl_->pSentryMissileEmplacementL3Demolish_;
    ostr << weaponPersist.pImpl_->pSentryMissileEmplacementL4Demolish_;
    ostr << weaponPersist.pImpl_->pLauncherMissileEmplacementL4Demolish_;
    ostr << weaponPersist.pImpl_->pIcbmMissileEmplacementL5Demolish_;
    ostr << weaponPersist.pImpl_->pGarrisonL1Demolish_;
    ostr << weaponPersist.pImpl_->pMineL1Demolish_;
    ostr << weaponPersist.pImpl_->pMineL3Demolish_;
    ostr << weaponPersist.pImpl_->pMineL5Demolish_;
    ostr << weaponPersist.pImpl_->pBeaconL1Demolish_;
    ostr << weaponPersist.pImpl_->pBeaconL3Demolish_;
    ostr << weaponPersist.pImpl_->pBeaconL5Demolish_;
    ostr << weaponPersist.pImpl_->pPodL1Demolish_;
    ostr << weaponPersist.pImpl_->pBee_;
    ostr << weaponPersist.pImpl_->pLightSting_;
    ostr << weaponPersist.pImpl_->pMetalSting_;

    ostr << weaponPersist.pImpl_->pBolterFlash_;
    ostr << weaponPersist.pImpl_->pHeavyBolterFlash_;
    ostr << weaponPersist.pImpl_->pAutoCannonFlash_;
}

void perRead( PerIstream& istr, MachPhysWeaponPersistence& weaponPersist )
{
    istr >> weaponPersist.root_;

    istr >> weaponPersist.pImpl_->pPlasmaRifle_;
    istr >> weaponPersist.pImpl_->pPlasmaCannon1_;
    istr >> weaponPersist.pImpl_->pPlasmaCannon2_;

    istr >> weaponPersist.pImpl_->pFlameThrower1_;
    istr >> weaponPersist.pImpl_->pFlameThrower2_;

    istr >> weaponPersist.pImpl_->pBolter_;
    istr >> weaponPersist.pImpl_->pAutoCannon_;
    istr >> weaponPersist.pImpl_->pHeavyBolter1_;
    istr >> weaponPersist.pImpl_->pHeavyBolter2_;

    istr >> weaponPersist.pImpl_->pVirusSpreaderLeft_;
    istr >> weaponPersist.pImpl_->pVirusSpreaderRight_;
    istr >> weaponPersist.pImpl_->pVirusSpreaderTop_;

    istr >> weaponPersist.pImpl_->pElectroChargerLeft_;
    istr >> weaponPersist.pImpl_->pElectroChargerRight_;
    istr >> weaponPersist.pImpl_->pElectroChargerTop_;

    istr >> weaponPersist.pImpl_->pMultiLauncher1_;
    istr >> weaponPersist.pImpl_->pMultiLauncher2_;
    istr >> weaponPersist.pImpl_->pMultiLauncher3_;
    istr >> weaponPersist.pImpl_->pMultiLauncher4_;
    istr >> weaponPersist.pImpl_->pMultiLauncher5_;
    istr >> weaponPersist.pImpl_->pMultiLauncher6_;
    istr >> weaponPersist.pImpl_->pMultiLauncher7_;

    istr >> weaponPersist.pImpl_->pPulseRifle_;
    istr >> weaponPersist.pImpl_->pPulseCannon_;
    istr >> weaponPersist.pImpl_->pPulseBlobRifle_;
    istr >> weaponPersist.pImpl_->pPulseBLobCannon_;

    istr >> weaponPersist.pImpl_->pLargeMissile_;
    istr >> weaponPersist.pImpl_->pTreacheryWeapon_;

    istr >> weaponPersist.pImpl_->pVortex_;
    istr >> weaponPersist.pImpl_->pNuke_;
    istr >> weaponPersist.pImpl_->pIon_;

	istr >> weaponPersist.pImpl_->pAdvancedSuperCharger_;
    istr >> weaponPersist.pImpl_->pSuperSuperCharger_;
    istr >> weaponPersist.pImpl_->pPunch_;

    istr >> weaponPersist.pImpl_->pObjectDemolish_;
    istr >> weaponPersist.pImpl_->pCivilianHardwareLabL1Demolish_;
    istr >> weaponPersist.pImpl_->pCivilianHardwareLabL3Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryHardwareLabL1Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryHardwareLabL3Demolish_;
    istr >> weaponPersist.pImpl_->pSmelterL1Demolish_;
    istr >> weaponPersist.pImpl_->pSmelterL3Demolish_;
    istr >> weaponPersist.pImpl_->pCivilianFactoryL1Demolish_;
    istr >> weaponPersist.pImpl_->pCivilianFactoryL3Demolish_;
    istr >> weaponPersist.pImpl_->pCivilianFactoryL5Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryFactoryL1Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryFactoryL3Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryFactoryL4Demolish_;
    istr >> weaponPersist.pImpl_->pMilitaryFactoryL5Demolish_;
    istr >> weaponPersist.pImpl_->pTurretMissileEmplacementL1Demolish_;
    istr >> weaponPersist.pImpl_->pTurretMissileEmplacementL2Demolish_;
    istr >> weaponPersist.pImpl_->pTurretMissileEmplacementL3Demolish_;
    istr >> weaponPersist.pImpl_->pSentryMissileEmplacementL3Demolish_;
    istr >> weaponPersist.pImpl_->pSentryMissileEmplacementL4Demolish_;
    istr >> weaponPersist.pImpl_->pLauncherMissileEmplacementL4Demolish_;
    istr >> weaponPersist.pImpl_->pIcbmMissileEmplacementL5Demolish_;
    istr >> weaponPersist.pImpl_->pGarrisonL1Demolish_;
    istr >> weaponPersist.pImpl_->pMineL1Demolish_;
    istr >> weaponPersist.pImpl_->pMineL3Demolish_;
    istr >> weaponPersist.pImpl_->pMineL5Demolish_;
    istr >> weaponPersist.pImpl_->pBeaconL1Demolish_;
    istr >> weaponPersist.pImpl_->pBeaconL3Demolish_;
    istr >> weaponPersist.pImpl_->pBeaconL5Demolish_;
    istr >> weaponPersist.pImpl_->pPodL1Demolish_;
    istr >> weaponPersist.pImpl_->pBee_;
    istr >> weaponPersist.pImpl_->pLightSting_;
    istr >> weaponPersist.pImpl_->pMetalSting_;

    istr >> weaponPersist.pImpl_->pBolterFlash_;
    istr >> weaponPersist.pImpl_->pHeavyBolterFlash_;
    istr >> weaponPersist.pImpl_->pAutoCannonFlash_;

}

/* End WEAPPER.CPP **************************************************/
