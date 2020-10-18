/*
 * M P R E L O A D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 *
 *
 * Preloads machine and construction objects
 *
 */

#include "render/device.hpp"
#include "world4d/scenemgr.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "machphys/vappuff.hpp"
#include "planeted/mpreload.hpp"
#include "machphys/administ.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/construc.hpp"
#include "machphys/technici.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/apc.hpp"
#include "machphys/pod.hpp"
#include "machphys/beacon.hpp"
#include "machphys/factory.hpp"
#include "machphys/garrison.hpp"
#include "machphys/hwlab.hpp"
#include "machphys/mine.hpp"
#include "machphys/missemp.hpp"
#include "machphys/smelter.hpp"
#include "machphys/fireball.hpp"
#include "render/mesh.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/domain.hpp"
#include "machlog/planet.hpp"
#include "mathex/transf3d.hpp"

//////////////////////////////////////////////////////////////////////


static
void doLoadingProgressUpdate( W4dSceneManager * pManager )
{
	static unsigned count = 0;
	if( ( ++count % 2 ) == 0 )
		pManager->pDevice()->displayImage("data/wait.bmp");
	else
		pManager->pDevice()->displayImage("data/wait2.bmp");
}

//////////////////////////////////////////////////////////////////////

MachPreload::MachPreload( W4dSceneManager * pManager )
{
	HAL_STREAM("MachPreload\n" );
	doLoadingProgressUpdate( pManager );

    W4dGenericComposite* pFaceAngry =    _NEW(W4dGenericComposite(&MachLogPlanet::instance().hiddenRoot(),
												MexTransform3d(),
										       	SysPathName("models/shared/faceplat/angry/faceangr.cdf")));

	doLoadingProgressUpdate( pManager );

    W4dGenericComposite* pFaceNormal =   _NEW(W4dGenericComposite(&MachLogPlanet::instance().hiddenRoot(),
   												MexTransform3d(),
											    SysPathName("models/shared/faceplat/normal/facenorm.cdf")));

	doLoadingProgressUpdate( pManager );

    W4dGenericComposite* pFaceSad =      _NEW(W4dGenericComposite(&MachLogPlanet::instance().hiddenRoot(),
   												MexTransform3d(),
											    SysPathName("models/shared/faceplat/sad/facesad.cdf")));

	doLoadingProgressUpdate( pManager );

	W4dGenericComposite* pSharedWheels = _NEW(W4dGenericComposite(&MachLogPlanet::instance().hiddenRoot(),
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/wheel/xwhl.cdf")));

	doLoadingProgressUpdate( pManager );

	//Preload all machine models
    //Aggressors
	//TBD::update aggressor loading stuff
	HAL_STREAM("MachPreload::KNIGHTS\n" );
	MachPhysAggressor* pAgK3 = _NEW( MachPhysAggressor(&MachLogPlanet::instance().hiddenRoot(),
														MexTransform3d(),
														MachPhys::KNIGHT,
														3,
														1,
														MachPhys::RED,
														MachPhys::LR_PULSE_RIFLE_X2));

	MachPhysAggressor* pAgK4 = _NEW( MachPhysAggressor(&MachLogPlanet::instance().hiddenRoot(),
														MexTransform3d(),
														MachPhys::KNIGHT,
														4,
														1,
														MachPhys::RED,
														MachPhys::LR_PULSE_RIFLE_X2));
//TBD::	MachPhysAggressor* pAgG1 = _NEW(MachPhysAggressor(&MachLogPlanet::instance().hiddenRoot(),
//TBD::												  	MexTransform3d(),
//TBD::													MachPhys::GRUNT,
//TBD::													1,
//TBD::													1,
//TBD::													MachPhys::RED));
//TBD::
//TBD::	doLoadingProgressUpdate( pManager );
//TBD::
//TBD::	MachPhysAggressor* pAgG2 = _NEW(MachPhysAggressor(&MachLogPlanet::instance().hiddenRoot(),
//TBD::												  	MexTransform3d(),
//TBD::													MachPhys::GRUNT,
//TBD::													2,
//TBD::													1,
//TBD::													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Locators
	HAL_STREAM("MachPreload::GEO_LOCATORS\n" );
	MachPhysGeoLocator* pLoG1 = _NEW(MachPhysGeoLocator(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysGeoLocator* pLoG2 = _NEW(MachPhysGeoLocator(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													2,
													1,
													MachPhys::RED));


	doLoadingProgressUpdate( pManager );

	HAL_STREAM("MachPreload::SPY_LOCATORS\n" );
	MachPhysSpyLocator* pLoS3 = _NEW(MachPhysSpyLocator(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													3,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysSpyLocator* pLoS5 = _NEW(MachPhysSpyLocator(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													5,
													1,
													MachPhys::RED));


	doLoadingProgressUpdate( pManager );

	//Transporters
	HAL_STREAM("MachPreload::RESOURCE_CARRIERS\n" );
	MachPhysResourceCarrier* pTrR1 = _NEW(MachPhysResourceCarrier(&MachLogPlanet::instance().hiddenRoot(),
												  MexTransform3d(),
												  1,
												  1,
												  MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysResourceCarrier* pTrR2 = _NEW(MachPhysResourceCarrier(&MachLogPlanet::instance().hiddenRoot(),
												  MexTransform3d(),
												  2,
												  1,
												  MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	HAL_STREAM("MachPreload::APC\n" );
	MachPhysAPC* pTrP2 = _NEW(MachPhysAPC(&MachLogPlanet::instance().hiddenRoot(),
												  MexTransform3d(),
												  2,
												  1,
												  MachPhys::RED));


	doLoadingProgressUpdate( pManager );

	//Technicians
	HAL_STREAM("MachPreload::TECHNICIANS\n" );
	MachPhysTechnician* pTeR1 = _NEW(MachPhysTechnician(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::LAB_TECH,
													1,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysTechnician* pTeR2 = _NEW(MachPhysTechnician(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::LAB_TECH,
													2,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Constructor
	HAL_STREAM("MachPreload::CONSTRUCTORS\n" );
	MachPhysConstructor* pCoD1 = _NEW(MachPhysConstructor(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::DOZER,
													1,
													1,
													MachPhys::RED));
	doLoadingProgressUpdate( pManager );

	MachPhysConstructor* pCoD2 = _NEW(MachPhysConstructor(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::DOZER,
													2,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Administrators
	//TBD:: update with weaponcombo stuff
//TBD::	MachPhysAdministrator* pAdB1 = _NEW(MachPhysAdministrator(&MachLogPlanet::instance().hiddenRoot(),
//TBD::												  	MexTransform3d(),
//TBD::													MachPhys::BOSS,
//TBD::													1,
//TBD::													1,
//TBD::													MachPhys::RED));
//TBD::
//TBD::	doLoadingProgressUpdate( pManager );
//TBD::
//TBD::	MachPhysAdministrator* pAdB2 = _NEW(MachPhysAdministrator(&MachLogPlanet::instance().hiddenRoot(),
//TBD::												  	MexTransform3d(),
//TBD::													MachPhys::BOSS,
//TBD::													2,
//TBD::													1,
//TBD::													MachPhys::RED));
	HAL_STREAM("MachPreload::ADMINISTRATORS\n" );
	MachPhysAdministrator* pAdC4 = _NEW( MachPhysAdministrator(&MachLogPlanet::instance().hiddenRoot(),
														MexTransform3d(),
														MachPhys::COMMANDER,
														4,
														1,
														MachPhys::RED,
														MachPhys::LR_PULSE_RIFLE_X2));

	MachPhysAdministrator* pAdC5 = _NEW( MachPhysAdministrator(&MachLogPlanet::instance().hiddenRoot(),
														MexTransform3d(),
														MachPhys::COMMANDER,
														5,
														1,
														MachPhys::RED,
														MachPhys::LR_PULSE_RIFLE_X2));

	doLoadingProgressUpdate( pManager );

	//////////////////////////////////////////////
	//Now Preload all constructions

 	//Preload Pods
	HAL_STREAM("MachPreload::POD\n" );
	MachPhysPod* pP1 = _NEW(MachPhysPod(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Hardware Labs
	HAL_STREAM("MachPreload::HARDWARE_LAB\n" );
	MachPhysHardwareLab* pHLC1 = _NEW(MachPhysHardwareLab(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::LAB_CIVILIAN,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysHardwareLab* pHLM1 = _NEW(MachPhysHardwareLab(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::LAB_MILITARY,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Smelter
	HAL_STREAM("MachPreload::SMELTER\n" );
	MachPhysSmelter* pSm1 = _NEW(MachPhysSmelter(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Factories
	HAL_STREAM("MachPreload::FACTORY\n" );
	MachPhysFactory* pFaC1 = _NEW(MachPhysFactory(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::CIVILIAN,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

/*	MachPhysFactory* pFaC2 = _NEW(MachPhysFactory(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::CIVILIAN,
													2,
													MachPhys::RED));
  */

	MachPhysFactory* pFaM1 = _NEW(MachPhysFactory(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::MILITARY,
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

/*
	MachPhysFactory* pFaM2 = _NEW(MachPhysFactory(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													MachPhys::MILITARY,
													2,
													MachPhys::RED));
  */
	//Missile Emplacements
	HAL_STREAM("MachPreload::MISSILE_EMPLACEMENTS\n" );
//TBD: not working	MachPhysMissileEmplacement* pMeT1 = _NEW(MachPhysMissileEmplacement(&MachLogPlanet::instance().hiddenRoot(),
//TBD: not working												  	MexTransform3d(),
//TBD: not working													MachPhys::TURRET,
//TBD: not working													1,
//TBD: not working													MachPhys::RED,
//TBD: not working													MachPhys::LR_PULSE_CANNON_X2 ));
//TBD: not working
//TBD: not working	doLoadingProgressUpdate( pManager );
//TBD: not working
//TBD: not working	MachPhysMissileEmplacement* pMeT2 = _NEW(MachPhysMissileEmplacement(&MachLogPlanet::instance().hiddenRoot(),
//TBD: not working												  	MexTransform3d(),
//TBD: not working													MachPhys::TURRET,
//TBD: not working													2,
//TBD: not working													MachPhys::RED,
//TBD: not working													MachPhys::LR_PULSE_CANNON_X2 ));
//TBD: not working
//TBD: not working	doLoadingProgressUpdate( pManager );
//TBD: not working
//TBD: not working	MachPhysMissileEmplacement* pMeS3 = _NEW(MachPhysMissileEmplacement(&MachLogPlanet::instance().hiddenRoot(),
//TBD: not working												  	MexTransform3d(),
//TBD: not working													MachPhys::SENTRY,
//TBD: not working													3,
//TBD: not working													MachPhys::RED,
//TBD: not working													MachPhys::LR_PULSE_CANNON_X2 ));

	doLoadingProgressUpdate( pManager );

	//Garrisons
	HAL_STREAM("MachPreload::GARRISON\n" );
	MachPhysGarrison* pGa1 = _NEW(MachPhysGarrison(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Mines
	HAL_STREAM("MachPreload::MINE\n" );
	MachPhysMine* pMn1 = _NEW(MachPhysMine(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													MachPhys::RED));


	doLoadingProgressUpdate( pManager );

	MachPhysMine* pMn3 = _NEW(MachPhysMine(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													3,
													MachPhys::RED));


	doLoadingProgressUpdate( pManager );

	//Beacons
	HAL_STREAM("MachPreload::BEACON\n" );
	MachPhysBeacon* pBe1 = _NEW(MachPhysBeacon(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													1,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysBeacon* pBe2 = _NEW(MachPhysBeacon(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													2,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	MachPhysBeacon* pBe3 = _NEW(MachPhysBeacon(&MachLogPlanet::instance().hiddenRoot(),
												  	MexTransform3d(),
													3,
													MachPhys::RED));

	doLoadingProgressUpdate( pManager );

	//Preload fireball meshes
	MachPhysFireball::preload();

	doLoadingProgressUpdate( pManager );

	MachPhysVapourPuff::preload();

	//Clear the unused meshes
	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();

	doLoadingProgressUpdate( pManager );

	//Delete our temporary models
    _DELETE(pFaceAngry);
	_DELETE(pFaceNormal);
	_DELETE(pFaceSad);
	_DELETE(pSharedWheels);
	_DELETE(pAgK3);
	_DELETE(pAgK4);
	_DELETE(pLoG1);
	_DELETE(pLoG2);
	_DELETE(pLoS3);
	_DELETE(pLoS5);
	_DELETE(pTrR1);
	_DELETE(pTrR2);
	_DELETE(pTrP2);
	_DELETE(pTeR1);
	_DELETE(pTeR2);
	_DELETE(pCoD1);
	_DELETE(pCoD2);
	_DELETE(pAdC4);
	_DELETE(pAdC5);

	doLoadingProgressUpdate( pManager );

	//Remove all constructions
	_DELETE(pP1);
	_DELETE(pHLC1);
	_DELETE(pHLM1);
	_DELETE(pSm1);
	_DELETE(pFaC1);
	//_DELETE(pFaC2);
	_DELETE(pFaM1);
	//_DELETE(pFaM2);
//	_DELETE(pMeT1);
//	_DELETE(pMeT2);
//	_DELETE(pMeS3);
	_DELETE(pGa1);
	_DELETE(pMn1);
	_DELETE(pMn3);
	_DELETE(pBe1);
	_DELETE(pBe2);
	_DELETE(pBe3);

	doLoadingProgressUpdate( pManager );

    TEST_INVARIANT;
}

MachPreload::~MachPreload()
{
    TEST_INVARIANT;

}



void MachPreload::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPreload& t )
{

    o << "MachPreload " << (void*)&t << " start" << std::endl;
    o << "MachPreload " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MPRELOAD.CPP *************************************************/
