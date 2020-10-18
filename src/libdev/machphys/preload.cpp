/*
 * P R E L O A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndwavid.hpp"
#include "phys/rampacce.hpp"
#include "render/texture.hpp"
#include "render/device.hpp"
#include "render/mesh.hpp"
#include "render/colour.hpp"

#include "system/memcaps.hpp"

#include "world4d/genecomp.hpp"
#include "world4d/soundman.hpp"

#include "machphys/preload.hpp"
#include "machphys/persist.hpp"
#include "machphys/effects.hpp"
#include "machphys/bolter.hpp"
#include "machphys/elecharg.hpp"
#include "machphys/halo.hpp"
#include "machphys/electro.hpp"
#include "machphys/flamebal.hpp"
#include "machphys/flamethr.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/plasmbol.hpp"
#include "machphys/mulaunch.hpp"
#include "machphys/vappuff.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/pulseblb.hpp"
#include "machphys/charger.hpp"
#include "machphys/healhelx.hpp"
#include "machphys/healaura.hpp"
#include "machphys/helix.hpp"
#include "machphys/missile.hpp"
#include "machphys/treachry.hpp"
#include "machphys/radidisc.hpp"
#include "machphys/virusspr.hpp"
#include "machphys/administ.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/construc.hpp"
#include "machphys/technici.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/apc.hpp"
#include "machphys/lmine.hpp"
#include "machphys/beam.hpp"
//#include "machphys/scorch.hpp"
#include "machphys/bsphere.hpp"
#include "machphys/wsphere.hpp"
#include "machphys/vortweap.hpp"
#include "machphys/flames.hpp"
#include "machphys/fireball.hpp"
#include "machphys/debrisun.hpp"

#include "machphys/pod.hpp"
#include "machphys/beacon.hpp"
#include "machphys/factory.hpp"
#include "machphys/garrison.hpp"
#include "machphys/hwlab.hpp"
#include "machphys/mine.hpp"
#include "machphys/missemp.hpp"
#include "machphys/smelter.hpp"
#include "machphys/oreholo.hpp"
#include "machphys/snddata.hpp"

#include "machphys/nuclweap.hpp"
#include "machphys/mushtop.hpp"
#include "machphys/mushedge.hpp"
#include "machphys/mushshaf.hpp"
#include "machphys/splat.hpp"
#include "machphys/shockwav.hpp"
#include "machphys/nukewave.hpp"

#include "machphys/jetringm.hpp"
#include "machphys/hsphere.hpp"
#include "machphys/sparks.hpp"

#include "machphys/punweap.hpp"
#include "machphys/blast.hpp"
#include "machphys/crakfire.hpp"

#include "machphys/ionweap.hpp"
#include "machphys/largemsl.hpp"
#include "machphys/dblflame.hpp"
#include "machphys/damagwav.hpp"
#include "machphys/smokpuff.hpp"

#include "machphys/plasaura.hpp"
#include "machphys/plasplat.hpp"
#include "machphys/pulsaura.hpp"
#include "machphys/pulsplat.hpp"
#include "machphys/objdemol.hpp"
#include "machphys/light.hpp"
#include "machphys/vsphere.hpp"

#include "machphys/waterfal.hpp"
#include "machphys/terrtile.hpp"

#include "machphys/beewave.hpp"
#include "machphys/beebomb.hpp"
#include "machphys/beeweap.hpp"

#include "machphys/stingl.hpp"
#include "machphys/stingm.hpp"

#include "machphys/stglwep.hpp"
#include "machphys/stgmwep.hpp"

#include "machphys/stlsplat.hpp"
#include "machphys/stlaura.hpp"

#include "machphys/loading.hpp"
#include "machphys/loadligh.hpp"
#include "machphys/loadunde.hpp"
#include "machphys/loadover.hpp"
#include "machphys/scavenge.hpp"
#include "machphys/locator.hpp"

#include "device/timer.hpp"
#include "base/istrrep.hpp"
#include "profiler/profiler.hpp"

void MachPhysPreload::swPreload( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
	preloadMachines( doUpdate );
	preloadConstructions( doUpdate );
	preloadWeaponsAndEffects( doUpdate );
    extrasPreload( doUpdate );
}

//static
void MachPhysPreload::preloadMachines( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
    W4dRoot tempRoot( 100000 );

    MachPhysMachine* pM;

    //Load the shared models

    doLoadingProgressUpdate( doUpdate );
	W4dGenericComposite* pSharedWheels = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/wheel/xwhl.cdf")));

	W4dGenericComposite* pSharedTracksA = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeA/xtra.cdf")));

	W4dGenericComposite* pSharedTracksB = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeB/xtrb.cdf")));

	W4dGenericComposite* pSharedTracksF = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeF/xtrf.cdf")));

	W4dGenericComposite* pSharedTracksG = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeG/xtrg.cdf")));

	W4dGenericComposite* pSharedTracksH = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeH/xtrh.cdf")));

	W4dGenericComposite* pSharedTracksI = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeI/xtri.cdf")));

	W4dGenericComposite* pSharedTracksJ = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeJ/xtrj.cdf")));

	W4dGenericComposite* pSharedTracksK = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/tracks/typeK/xtrk.cdf")));


	W4dGenericComposite* pSharedFace = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/faceplat/fplat.cdf")));

	W4dGenericComposite* pSharedHoverG = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/hover/typeG/xhvg.cdf")));

	W4dGenericComposite* pSharedHoverJ = _NEW(W4dGenericComposite(&tempRoot,
   					  							MexTransform3d(),
					  					       	SysPathName("models/shared/hover/typeJ/xhvj.cdf")));

	HAL_STREAM("MachPreload::ADMINISTRATORS\n" );
    doLoadingProgressUpdate( doUpdate );

	if( not getenv( "cb_builddemo" ) )
	{
		pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
        	MachPhys::BOSS, 1, 1, MachPhys::RED, MachPhys::T_WASP_LIGHT_STING));
    	_DELETE( pM );

 		pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
     	   MachPhys::BOSS, 2, 1, MachPhys::RED, MachPhys::T_BEE_BOMB));
   		 _DELETE( pM );

    	doLoadingProgressUpdate( doUpdate );

		HAL_STREAM("MachPreload::ADMINISTRATORS::OVERSEER\n" );
 		pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
        	MachPhys::OVERSEER, 2, 1, MachPhys::RED, MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED));
    	_DELETE( pM );
    }
 	pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
         MachPhys::OVERSEER, 3, 1, MachPhys::RED, MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED));
     _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

	if( not getenv( "cb_builddemo" ) )
	{
		HAL_STREAM("MachPreload::ADMINISTRATORS::COMMANDER\n" );
		pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
	        MachPhys::COMMANDER, 4, 1, MachPhys::RED, MachPhys::LR_PULSE_CANNON_X2));
	    _DELETE( pM );

	    pM = _NEW( MachPhysAdministrator(&tempRoot, MexTransform3d(),
	        MachPhys::COMMANDER, 5, 1, MachPhys::RED, MachPhys::LR_PULSE_CANNON_X2));
	    _DELETE( pM );
    doLoadingProgressUpdate( doUpdate );
	}
	HAL_STREAM("MachPreload::AGGRESSORS::GRUNT\n" );

    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::GRUNT, 1, 1, MachPhys::RED, MachPhys::R_BOLTER));
    _DELETE( pM );

    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::GRUNT, 2, 1, MachPhys::RED, MachPhys::L_ELECTRIC_CHARGE));
    _DELETE( pM );

    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::GRUNT, 3, 1, MachPhys::RED, MachPhys::LR_HEAVY_BOLTER1_X2));
    _DELETE( pM );
    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::AGGRESSORS::ASSASSIN\n" );
    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::ASSASSIN, 2, 1, MachPhys::RED, MachPhys::L_AUTO_CANNON));
    _DELETE( pM );

    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::ASSASSIN, 3, 1, MachPhys::RED, MachPhys::LR_PLASMA_RIFLE_X2));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::AGGRESSORS::ASSASSIN::BALLISTA\n" );

	if( not getenv( "cb_builddemo" ) )
	{
	    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
	        MachPhys::BALLISTA, 3, 1, MachPhys::RED, MachPhys::T_MULTI_LAUNCHER1));
	    _DELETE( pM );

	    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
	        MachPhys::BALLISTA, 3, 1, MachPhys::RED, MachPhys::T_VORTEX));
	    _DELETE( pM );

	    doLoadingProgressUpdate( doUpdate );

	    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
	        MachPhys::BALLISTA, 4, 1, MachPhys::RED, MachPhys::LR_MULTI_LAUNCHER2_X2));
	    _DELETE( pM );
	}
	HAL_STREAM("MachPreload::AGGRESSORS::ASSASSIN::KNIGHT\n" );
    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::KNIGHT, 3, 1, MachPhys::RED, MachPhys::LR_PLASMA_CANNON1_X2));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

	if( not getenv( "cb_builddemo" ) )
	{
	    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
	        MachPhys::KNIGHT, 4, 1, MachPhys::RED, MachPhys::LR_PLASMA_CANNON2_X2));
	    _DELETE( pM );

	    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
	        MachPhys::KNIGHT, 5, 1, MachPhys::RED, MachPhys::LRT_PLASMA_CANNON2_X3));
	    _DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );

    pM = _NEW( MachPhysAggressor(&tempRoot, MexTransform3d(),
        MachPhys::NINJA, 5, 1, MachPhys::RED, MachPhys::LR_MULTI_LAUNCHER7_X2));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );


	HAL_STREAM("MachPreload::GEO_LOCATORS\n" );

	pM = _NEW(MachPhysGeoLocator(&tempRoot, MexTransform3d(),
        1, 1, MachPhys::RED));
    _DELETE( pM );

	pM = _NEW(MachPhysGeoLocator(&tempRoot, MexTransform3d(),
        2, 1, MachPhys::RED));
    _DELETE( pM );

	if( not getenv( "cb_builddemo" ) )
	{
		pM = _NEW(MachPhysGeoLocator(&tempRoot, MexTransform3d(),
        	3, 1, MachPhys::RED));
    	_DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );


	HAL_STREAM("MachPreload::SPY_LOCATORS\n" );

	pM = _NEW(MachPhysSpyLocator(&tempRoot, MexTransform3d(),
        3, 1, MachPhys::RED));
    _DELETE( pM );

    pM = _NEW(MachPhysSpyLocator(&tempRoot, MexTransform3d(),
        5, 1, MachPhys::RED));
    _DELETE( pM );
    doLoadingProgressUpdate( doUpdate );


	HAL_STREAM("MachPreload::RESOURCE_CARRIERS\n" );

	pM = _NEW(MachPhysResourceCarrier(&tempRoot, MexTransform3d(),
        1, 1, MachPhys::RED));
    _DELETE( pM );

	pM = _NEW(MachPhysResourceCarrier(&tempRoot, MexTransform3d(),
        2, 1, MachPhys::RED));
    _DELETE( pM );

	if( not getenv( "cb_builddemo" ) )
	{
		pM = _NEW(MachPhysResourceCarrier(&tempRoot, MexTransform3d(),
	        3, 1, MachPhys::RED));
	    _DELETE( pM );

		pM = _NEW(MachPhysResourceCarrier(&tempRoot, MexTransform3d(),
	        4, 1, MachPhys::RED));
	    _DELETE( pM );

		pM = _NEW(MachPhysResourceCarrier(&tempRoot, MexTransform3d(),
	        5, 1, MachPhys::RED));
	    _DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::APC\n" );

	pM = _NEW(MachPhysAPC(&tempRoot, MexTransform3d(),
        2, 1, MachPhys::RED));
    _DELETE( pM );

	if( not getenv( "cb_builddemo" ) )
	{
		pM = _NEW(MachPhysAPC(&tempRoot, MexTransform3d(),
	        4, 1, MachPhys::RED));
	    _DELETE( pM );

		pM = _NEW(MachPhysAPC(&tempRoot, MexTransform3d(),
	        5, 1, MachPhys::RED));
	    _DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::TECHNICIANS\n" );

	pM = _NEW(MachPhysTechnician(&tempRoot, MexTransform3d(),
        MachPhys::LAB_TECH, 2, 1, MachPhys::RED));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

	pM = _NEW(MachPhysTechnician(&tempRoot, MexTransform3d(),
        MachPhys::TECH_BOY, 3, 3, MachPhys::RED));
    _DELETE( pM );

   doLoadingProgressUpdate( doUpdate );

	if( not getenv( "cb_builddemo" ) )
	{
		pM = _NEW(MachPhysTechnician(&tempRoot, MexTransform3d(),
	        MachPhys::BRAIN_BOX, 5, 5, MachPhys::RED));
	    _DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::CONSTRUCTORS\n" );

	pM = _NEW(MachPhysConstructor(&tempRoot, MexTransform3d(),
        MachPhys::DOZER, 1, 1, MachPhys::RED));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

    pM = _NEW(MachPhysConstructor(&tempRoot, MexTransform3d(),
        MachPhys::BUILDER, 3, 1, MachPhys::RED));
    _DELETE( pM );

    doLoadingProgressUpdate( doUpdate );

	if( not getenv( "cb_builddemo" ) )
	{
	    pM = _NEW(MachPhysConstructor(&tempRoot, MexTransform3d(),
	        MachPhys::BEHEMOTH, 4, 1, MachPhys::RED));
	    _DELETE( pM );

	    pM = _NEW(MachPhysConstructor(&tempRoot, MexTransform3d(),
	        MachPhys::BEHEMOTH, 5, 1, MachPhys::RED));
	    _DELETE( pM );
	}

    doLoadingProgressUpdate( doUpdate );

    _DELETE( pSharedWheels );
    _DELETE( pSharedTracksA );
    _DELETE( pSharedTracksB );
    _DELETE( pSharedTracksF );
    _DELETE( pSharedTracksG );
    _DELETE( pSharedTracksH );
    _DELETE( pSharedTracksI );
    _DELETE( pSharedTracksJ );
    _DELETE( pSharedTracksK );
    _DELETE( pSharedFace );
    _DELETE( pSharedHoverG );
    _DELETE( pSharedHoverJ );

    doLoadingProgressUpdate( doUpdate );

	//Clear the unused meshes
	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();
}

//static
void MachPhysPreload::preloadConstructions( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
    W4dRoot tempRoot( 100000 );
    MachPhysConstruction* pC;

   	HAL_STREAM("MachPreload::POD\n" );

	pC = _NEW(MachPhysPod(&tempRoot, MexTransform3d(),
        1, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::HARDWARE_LAB\n" );

	pC = _NEW(MachPhysHardwareLab(&tempRoot, MexTransform3d(),
        MachPhys::LAB_CIVILIAN, 1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysHardwareLab(&tempRoot, MexTransform3d(),
        MachPhys::LAB_CIVILIAN, 3, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	pC = _NEW(MachPhysHardwareLab(&tempRoot, MexTransform3d(),
        MachPhys::LAB_MILITARY, 1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysHardwareLab(&tempRoot, MexTransform3d(),
        MachPhys::LAB_MILITARY, 3, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::SMELTER\n" );

	pC = _NEW(MachPhysSmelter(&tempRoot, MexTransform3d(),
        1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysSmelter(&tempRoot, MexTransform3d(),
        3, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::FACTORY\n" );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::CIVILIAN, 1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::CIVILIAN, 3, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::CIVILIAN, 5, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::MILITARY, 1, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::MILITARY, 3, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::MILITARY, 4, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysFactory(&tempRoot, MexTransform3d(),
        MachPhys::MILITARY, 5, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::MISSILE_EMPLACEMENTS\n" );

	pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::TURRET, 1, MachPhys::RED, MachPhys::T_FLAME_THROWER1 ));
    _DELETE( pC );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::TURRET, 2, MachPhys::RED, MachPhys::LR_AUTO_CANNON_X2 ));
    _DELETE( pC );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::TURRET, 3, MachPhys::RED, MachPhys::LR_PULSE_RIFLE_X2 ));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::SENTRY, 3, MachPhys::RED, MachPhys::LR_MULTI_LAUNCHER5_X2 ));
    _DELETE( pC );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::SENTRY, 4, MachPhys::RED, MachPhys::LR_MULTI_LAUNCHER6_X2 ));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::LAUNCHER, 4, MachPhys::RED, MachPhys::LR_LARGE_MISSILE_X2 ));
    _DELETE( pC );

    pC = _NEW(MachPhysMissileEmplacement(&tempRoot, MexTransform3d(),
        MachPhys::ICBM, 5, MachPhys::RED, MachPhys::T_NUCLEAR_MISSILE ));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::GARRISON\n" );

	pC = _NEW(MachPhysGarrison(&tempRoot, MexTransform3d(),
        1, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::MINE\n" );

	pC = _NEW(MachPhysMine(&tempRoot, MexTransform3d(),
        1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysMine(&tempRoot, MexTransform3d(),
        3, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysMine(&tempRoot, MexTransform3d(),
        5, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("MachPreload::BEACON\n" );

	pC = _NEW(MachPhysBeacon(&tempRoot, MexTransform3d(),
        1, MachPhys::RED));
    _DELETE( pC );

	pC = _NEW(MachPhysBeacon(&tempRoot, MexTransform3d(),
        3, MachPhys::RED));
    _DELETE( pC );

    doLoadingProgressUpdate( doUpdate );

    //Preload missile

	HAL_STREAM("MachPreload::Missile\n" );
    MachPhysMissile* pMissile1 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE1 ) );
    _DELETE( pMissile1 );

    MachPhysMissile* pMissile2 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE2 ) );
    _DELETE( pMissile2 );

    MachPhysMissile* pMissile3 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE3 ) );
    _DELETE( pMissile3 );

    doLoadingProgressUpdate( doUpdate );
	HAL_STREAM("MachPreload::Missile\n" );
    MachPhysMissile* pMissile4 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE4 ) );
    _DELETE( pMissile4 );

    MachPhysMissile* pMissile5 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE5 ) );
    _DELETE( pMissile5 );

    MachPhysMissile* pMissile6 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE6 ) );
    _DELETE( pMissile6 );

    doLoadingProgressUpdate( doUpdate );
	HAL_STREAM("MachPreload::Missile7\n" );
    MachPhysMissile* pMissile7 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE7 ) );
    _DELETE( pMissile7 );

	HAL_STREAM("MachPreload::Missile8\n" );
    MachPhysMissile* pMissile8 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::MISSILE_LARGE ) );
    _DELETE( pMissile8 );

	HAL_STREAM("MachPreload::Missile9\n" );
    MachPhysMissile* pMissile9 = _NEW( MachPhysMissile( &tempRoot, MexTransform3d(), MachPhysMissile::NUCLEAR_MISSILE ) );
    _DELETE( pMissile9 );

    doLoadingProgressUpdate( doUpdate );

	//Clear the unused meshes
	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();

    //extrasPreload();
}

//static
void MachPhysPreload::preloadWeaponsAndEffects( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
    DevTimer    timer;
    W4dRoot tempRoot( 100000 );

    //Ore holograph
	HAL_STREAM("MachPreload::holo\n" );
    MachPhysOreHolograph* pHolo = _NEW( MachPhysOreHolograph( &tempRoot, MexTransform3d(), MachPhys::RED, 1, 2 ) );
    _DELETE( pHolo );

	HAL_STREAM("construction stuff\n" );
	//construction constructing stuff
	//MachPhysHemiSphere::exemplar();
	MachPhysJetRing::exemplar();
	MachPhysSparks::exemplar();

    //destruction stuff
	HAL_STREAM("destruction stuff\n" );
    MachPhysDebrisUnit::factory();
    doLoadingProgressUpdate( doUpdate );

	//Plasma stuff
	HAL_STREAM("Plasma stuff\n" );
	MachPhysPlasmaWeapon::exemplar( MachPhys::PLASMA_RIFLE );
	MachPhysPlasmaWeapon::exemplar( MachPhys::PLASMA_CANNON1 );
	MachPhysPlasmaWeapon::exemplar( MachPhys::PLASMA_CANNON2 );
	MachPhysPlasmaBolt::exemplar( MachPhys::PLASMA_RIFLE );
    doLoadingProgressUpdate( doUpdate );
	MachPhysPlasmaBolt::exemplar( MachPhys::PLASMA_CANNON1 );
	MachPhysPlasmaBolt::exemplar( MachPhys::PLASMA_CANNON2 );
	MachPhysPlasmaAura::exemplar();
	MachPhysPlasmaSplat::exemplar();
    doLoadingProgressUpdate( doUpdate );

	//Flamethrower stuff
	HAL_STREAM("Flamethrower stuff\n" );
	MachPhysFlameThrower::exemplar( MachPhys::FLAME_THROWER1 );
	MachPhysFlameThrower::exemplar( MachPhys::FLAME_THROWER2 );
	MachPhysFlameBall::exemplar( MachPhysFlameBall::GRUNT_FLAME_BALL );
	MachPhysFlameBall::exemplar( MachPhysFlameBall::TURRET_FLAME_BALL );
    doLoadingProgressUpdate( doUpdate );

	//bolter/autocannon stuff
	HAL_STREAM("Bolter stuff\n" );
	MachPhysBolter::exemplar( MachPhys::BOLTER );
	MachPhysBolter::exemplar( MachPhys::HEAVY_BOLTER1 );
	MachPhysBolter::exemplar( MachPhys::HEAVY_BOLTER2 );
	MachPhysBolter::exemplar( MachPhys::AUTO_CANNON );
    doLoadingProgressUpdate( doUpdate );
	MachPhysBolter::exemplarFlash(MachPhys::BOLTER);
	MachPhysBolter::exemplarFlash(MachPhys::HEAVY_BOLTER1);
	MachPhysBolter::exemplarFlash(MachPhys::HEAVY_BOLTER2);
	MachPhysBolter::exemplarFlash(MachPhys::AUTO_CANNON);
	MachPhysLight::exemplar();
    doLoadingProgressUpdate( doUpdate );

	//garilla punch
	HAL_STREAM("garilla punch stuff\n" );
	MachPhysPunchWeapon::exemplar();
	MachPhysBlast::exemplar();
	MachPhysCrackFire::exemplar( MachPhysCrackFire::GARILLA_PUNCH_CHASM );
	MachPhysCrackFire::exemplar(MachPhysCrackFire::BEE_BOMB_CHASM );

	//Virus
	HAL_STREAM("Virus stuff\n" );
	//MachPhysVirusSpreader::exemplar( MachPhys::RIGHT );
    doLoadingProgressUpdate( doUpdate );

	//Multi-launchers
	HAL_STREAM("Multi-launchers stuff\n" );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER1 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER2 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER3 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER4 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER5 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER6 );
	MachPhysMultiLauncher::exemplar( MachPhys::MULTI_LAUNCHER7 );
    doLoadingProgressUpdate( doUpdate );

	//Pulse stuff
	HAL_STREAM("Pulse stuff\n" );
	MachPhysPulseRifle::exemplar();
	MachPhysPulseCannon::exemplar();
	MachPhysPulseBlob::exemplar(MachPhys::PULSE_RIFLE);
	MachPhysPulseBlob::exemplar(MachPhys::PULSE_CANNON);
	MachPhysPulseAura::exemplar();
	MachPhysPulseSplat::exemplar();
    doLoadingProgressUpdate( doUpdate );

	//Supercharger stuff
	HAL_STREAM("Supercharger stuff\n" );
	MachPhysSuperCharger::exemplar( MachPhys::SUPERCHARGE_ADVANCED );
	MachPhysSuperCharger::exemplar( MachPhys::SUPERCHARGE_SUPER );
	MachPhysHelix::exemplar( MachPhysHelix::LEFT_TWIST );
	MachPhysHelix::exemplar( MachPhysHelix::RIGHT_TWIST );
    MachPhysHealAura::centralCrossExemplar();
    MachPhysHealAura::radialCrossExemplar();
    MachPhysHealAura::radialCrossMaterial();
    doLoadingProgressUpdate( doUpdate );

	//large missile
	MachPhysLargeMissile::exemplar();

	//Treachery stuff
	HAL_STREAM("Treachery stuff\n" );
	MachPhysTreacheryWeapon::exemplar();
    MachPhysRadialDisc::exemplar();
    doLoadingProgressUpdate( doUpdate );

    //Spy/landmine
    MachPhysLandMine::exemplar();

	//MachPhysTerrainTile::preloadWaterTextures();
	//MachPhysWaterfall::exemplar();

    //Ion cannon stuff
	HAL_STREAM("Ion cannon stuff\n" );
	MachPhysIonWeapon::exemplar(MachPhys::ION_ORBITAL_CANNON);
    MachPhysBeam::exemplar();
    //MachPhysGroundScorch::exemplar();
    doLoadingProgressUpdate( doUpdate );

    //Vortex stuff
	HAL_STREAM("Vortex stuff\n" );
    MachPhysVortexWeapon::exemplar();
    MachPhysBlackSphere::exemplar();
    MachPhysWhiteSphere::exemplar();
    MachPhysVortexSphere::exemplar();
    doLoadingProgressUpdate( doUpdate );

	//Nuke stuff
	HAL_STREAM("Nuke stuff\n" );
	MachPhysNuclearWeapon::exemplar();
	MachPhysMushroomShaft::exemplar();
	MachPhysMushroomEdge::exemplar();
	MachPhysMushroomTop::exemplar();
	MachPhysGroundSplat::exemplar();
    doLoadingProgressUpdate( doUpdate );
    MachPhysShockWave::exemplar();
    MachPhysDoublesidedFlame::exemplar();
    MachPhysBurstWave::exemplar();
	MachPhysNukeWave::exemplar();
    doLoadingProgressUpdate( doUpdate );

	//electric charge stuff
	HAL_STREAM("electric charge stuff\n" );
    MachPhysElectroCharger::exemplar( MachPhys::RIGHT );
    MachPhysLightningHalo::exemplar( MachPhysLightningHalo::ELECTRIC );
    //MachPhysLightningHalo::exemplar( MachPhysLightningHalo::VIRUS );
	MachPhysElectro::exemplar( MachPhysElectro::ELECTRIC );
	//MachPhysElectro::exemplar( MachPhysElectro::VIRUS );
    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("bee ans wasp stuff\n" );

	MachPhysBeeBombWave::exemplar();
	MachPhysBeeBomb::exemplar();
	MachPhysBeeBomber::exemplar();
    doLoadingProgressUpdate( doUpdate );

	MachPhysLightStingAura::exemplar();
	MachPhysLightStingSplat::exemplar();
	MachPhysLightSting::exemplar();
	MachPhysLightStingWeapon::exemplar();
    doLoadingProgressUpdate( doUpdate );

	MachPhysMetalSting::exemplar();
	MachPhysMetalStingWeapon::exemplar();
    doLoadingProgressUpdate( doUpdate );

	MachPhysLoadingLight::exemplar();
	MachPhysLoadingOver::exemplar();
	MachPhysLoadingUnder::exemplar();
	MachPhysScavenger::exemplar();
	MachPhysLocator::exemplar();
    doLoadingProgressUpdate( doUpdate );

	HAL_STREAM("Object demolition stuff\n" );

	MachPhys::DemolitionType cType;
	cType.objectType=MachPhys::NOT_CONSTRUCTION;
    MachPhysObjDemolish::exemplar( cType );

	cType.objectType=MachPhys::CONSTRUCTION;

	cType.constructionType=MachPhys::HARDWARE_LAB;
	for ( cType.subType=0; cType.subType<2; ++cType.subType )
	{
	  cType.level=1;
	  MachPhysObjDemolish::exemplar( cType );
	  cType.level=3;
	  MachPhysObjDemolish::exemplar( cType );
	}

 	cType.constructionType=MachPhys::SMELTER;
	cType.level=1;
    MachPhysObjDemolish::exemplar( cType );
	cType.level=3;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::FACTORY;
	for ( cType.subType=0; cType.subType<2; ++cType.subType )
	{
	  cType.level=1;
	  MachPhysObjDemolish::exemplar( cType );
	  cType.level=3;
	  MachPhysObjDemolish::exemplar( cType );
	  cType.level=5;
	  MachPhysObjDemolish::exemplar( cType );
    }

    cType.subType=MachPhys::MILITARY;
    cType.level=4;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::MISSILE_EMPLACEMENT;
	cType.subType=0;
    cType.level=1;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=2;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=3;
    MachPhysObjDemolish::exemplar( cType );

	cType.subType=1;
    cType.level=3;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=4;
    MachPhysObjDemolish::exemplar( cType );

	cType.subType=2;
    cType.level=4;
    MachPhysObjDemolish::exemplar( cType );

	cType.subType=3;
    cType.level=5;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::GARRISON;
    cType.level=1;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::MINE;
    cType.level=1;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=3;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=5;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::BEACON;
    cType.level=1;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=3;
    MachPhysObjDemolish::exemplar( cType );
    cType.level=5;
    MachPhysObjDemolish::exemplar( cType );

	cType.constructionType=MachPhys::POD;
    cType.level=1;
    MachPhysObjDemolish::exemplar( cType );

    doLoadingProgressUpdate( doUpdate );
	//Clear the unused meshes
	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();
}

//static
void MachPhysPreload::extrasPreload( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
    DevTimer    timer;
    W4dRoot tempRoot( 100000 );

	HAL_STREAM("MachPreload::extrasPreload()\n" );
	//Various textures
	MachPhysEffects::smokeTextures();
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::GREY );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::BLACK );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::WHITE );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::PURPLE );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::YELLOW );
    doLoadingProgressUpdate( doUpdate );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::RUST );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::CIVILIAN_WHITE );
	MachPhysEffects::smokeMaterialPlan( MachPhysEffects::MILITARY_RED );
	MachPhysEffects::sparksMaterialPlan();
	MachPhysEffects::flameBallTextures();
	MachPhysEffects::flameBallMaterialPlan();
	MachPhysEffects::flameTextures();
    doLoadingProgressUpdate( doUpdate );

    MachPhysDebrisUnit::preloadTextures();
    MachPhysFlame::preload();
    MachPhysFireball::preload();
	MachPhysPlasmaSplat::preloadTextures();
	MachPhysBolter::sparkMaterial();
	MachPhysVapourPuff::preload();
	MachPhysPulseSplat::preloadTextures();
	MachPhysHealHelix::helixMaterial();
    MachPhysRadialDisc::circleTexture();
	MachPhysShockWave::materialsPtr();
    MachPhysSmokePuff::preload();
	MachPhysNukeWave::materialsPtr();
	MachPhysLightStingSplat::preloadTextures();

	//constructing plans
	MachPhysConstruction::preloadConstructingPlans();

	HAL_STREAM("SndMixer stuff\n" );

    //Preload the explosion sounds that are used with fireballs
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/wheeled.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/tracked.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/ahover.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/legged.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/minewrks.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/factory.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/labs.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/beacon.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/hlab.wav" ) ) );
////	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/pod.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/smelter.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/xplode1.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/xplode2.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/xplode3.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/xplode4.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/xplode5.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/mincone.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/move05.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/fire.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/vortex.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/puchit.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/pulserif.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/pulsecan.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/bolter.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/hevybolt.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/autocan.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/autobk.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/suprheal.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/ioncan.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/flamethr.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/treach.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/electchg.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/mutilnch.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/misflyg.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/virus.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/plasmrif.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/plasmcan.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/plrhit.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/plchit.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/burning.wav" ) ) );
//	SndMixer::instance().loadWaveform( SndWaveformId( SysPathName( "sounds/waterf.wav" ) ) );

	HAL_STREAM("all stuff!\n" );

    std::cout << "Time for extras preload " << timer.time() << std::endl;
}

// static
SysPathName MachPhysPreload::persistentFileName()
{
    return storedPersistentFileName();
}

void MachPhysPreload::registerSounds()
{
	W4dSoundManager::instance().registerSoundID("SID_POD", SID_POD);
	W4dSoundManager::instance().registerSoundID("SID_BEACON", SID_BEACON);
	W4dSoundManager::instance().registerSoundID("SID_BOLTER", SID_BOLTER);
	W4dSoundManager::instance().registerSoundID("SID_HEAVYBOLTER", SID_HEAVYBOLTER);
	W4dSoundManager::instance().registerSoundID("SID_AUTOCANNON", SID_AUTOCANNON);
	W4dSoundManager::instance().registerSoundID("SID_AUTOBK", SID_AUTOBK);
	W4dSoundManager::instance().registerSoundID("SID_SUPERHEAL", SID_SUPERHEAL);
	W4dSoundManager::instance().registerSoundID("SID_FIRE", SID_FIRE);
	W4dSoundManager::instance().registerSoundID("SID_ELECTRICCHARGE", SID_ELECTRICCHARGE);
	W4dSoundManager::instance().registerSoundID("SID_DOOR", SID_DOOR);
	W4dSoundManager::instance().registerSoundID("SID_FACTORY", SID_FACTORY);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE1", SID_XPLODE1);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE2", SID_XPLODE2);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE3", SID_XPLODE3);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE4", SID_XPLODE4);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE5", SID_XPLODE5);
	W4dSoundManager::instance().registerSoundID("SID_FLAMETHROWER", SID_FLAMETHROWER);
	W4dSoundManager::instance().registerSoundID("SID_HWARELAB", SID_HWARELAB);
	W4dSoundManager::instance().registerSoundID("SID_IONCANNON", SID_IONCANNON);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH", SID_MLAUNCH);
	W4dSoundManager::instance().registerSoundID("SID_MFLY", SID_MFLY);
	W4dSoundManager::instance().registerSoundID("SID_BURNING", SID_BURNING);
	W4dSoundManager::instance().registerSoundID("SID_WHEELS", SID_WHEELS);
	W4dSoundManager::instance().registerSoundID("SID_TRACKS", SID_TRACKS);
	W4dSoundManager::instance().registerSoundID("SID_HOVER", SID_HOVER);
	W4dSoundManager::instance().registerSoundID("SID_SPIDER", SID_SPIDER);
	W4dSoundManager::instance().registerSoundID("SID_MINE", SID_MINE);
	W4dSoundManager::instance().registerSoundID("SID_VORTEX", SID_VORTEX);
	W4dSoundManager::instance().registerSoundID("SID_OREHOLO", SID_OREHOLO);
	W4dSoundManager::instance().registerSoundID("SID_PLASMARIFLE", SID_PLASMARIFLE);
	W4dSoundManager::instance().registerSoundID("SID_PLASMACANNON", SID_PLASMACANNON);
	W4dSoundManager::instance().registerSoundID("SID_PLRHIT", SID_PLRHIT);
	W4dSoundManager::instance().registerSoundID("SID_PLCHIT", SID_PLCHIT);
	W4dSoundManager::instance().registerSoundID("SID_PUCHIT", SID_PUCHIT);
	W4dSoundManager::instance().registerSoundID("SID_PULSERIFLE", SID_PULSERIFLE);
	W4dSoundManager::instance().registerSoundID("SID_PULSECANNON", SID_PULSECANNON);
	W4dSoundManager::instance().registerSoundID("SID_SMELTER", SID_SMELTER);
	W4dSoundManager::instance().registerSoundID("SID_MINEWRKS", SID_MINEWRKS);
	W4dSoundManager::instance().registerSoundID("SID_LABS", SID_LABS);
	W4dSoundManager::instance().registerSoundID("SID_TREACHERY", SID_TREACHERY);
	W4dSoundManager::instance().registerSoundID("SID_VIRUS", SID_VIRUS);
	W4dSoundManager::instance().registerSoundID("SID_WATERFALL", SID_WATERFALL);
	W4dSoundManager::instance().registerSoundID("SID_NUKE", SID_NUKE);
	W4dSoundManager::instance().registerSoundID("SID_CRACK", SID_CRACK);
	W4dSoundManager::instance().registerSoundID("SID_MISSDOMECLOSE", SID_MISSDOMECLOSE);
	W4dSoundManager::instance().registerSoundID("SID_MISSDOMEOPEN", SID_MISSDOMEOPEN);
	W4dSoundManager::instance().registerSoundID("SID_NUKEMLAUNCH", SID_NUKEMLAUNCH);
	W4dSoundManager::instance().registerSoundID("SID_NUKEMFLY", SID_NUKEMFLY);
	W4dSoundManager::instance().registerSoundID("SID_GARRISON", SID_GARRISON);
	W4dSoundManager::instance().registerSoundID("SID_NINJA", SID_NINJA);
	W4dSoundManager::instance().registerSoundID("SID_MINE1", SID_MINE1);
	W4dSoundManager::instance().registerSoundID("SID_MINE3", SID_MINE3);
	W4dSoundManager::instance().registerSoundID("SID_MINE5", SID_MINE5);
	W4dSoundManager::instance().registerSoundID("SID_SMELTER1AND3", SID_SMELTER1AND3);
	W4dSoundManager::instance().registerSoundID("SID_SMELTER5", SID_SMELTER5);
	W4dSoundManager::instance().registerSoundID("SID_BEACON1", SID_BEACON1);
	W4dSoundManager::instance().registerSoundID("SID_BEACON3", SID_BEACON3);
	W4dSoundManager::instance().registerSoundID("SID_CIVILIANFACTORY", SID_CIVILIANFACTORY);
	W4dSoundManager::instance().registerSoundID("SID_MILITARYFACTORY", SID_MILITARYFACTORY);
	W4dSoundManager::instance().registerSoundID("SID_MILITARYFACTORYFAN", SID_MILITARYFACTORYFAN);
	W4dSoundManager::instance().registerSoundID("SID_CIVILIANLAB", SID_CIVILIANLAB);
	W4dSoundManager::instance().registerSoundID("SID_CIVILIANLAB_FLASH", SID_CIVILIANLAB_FLASH);
	W4dSoundManager::instance().registerSoundID("SID_MILITARYLAB", SID_MILITARYLAB);
	W4dSoundManager::instance().registerSoundID("SID_MILITARYLAB_GRILLLIGHT", SID_MILITARYLAB_GRILLLIGHT);
	W4dSoundManager::instance().registerSoundID("SID_BOLTER_HIT", SID_BOLTER_HIT);
	W4dSoundManager::instance().registerSoundID("SID_HEAVYBOLTER_HIT", SID_HEAVYBOLTER_HIT);
	W4dSoundManager::instance().registerSoundID("SID_PUNCH", SID_PUNCH);
	W4dSoundManager::instance().registerSoundID("SID_GEOLOCATE", SID_GEOLOCATE);
	W4dSoundManager::instance().registerSoundID("SID_LMINE_DEPLOY", SID_LMINE_DEPLOY);
    W4dSoundManager::instance().registerSoundID("SID_CONSTRUCTING",SID_CONSTRUCTING);
    W4dSoundManager::instance().registerSoundID("SID_RESEARCHING",SID_RESEARCHING);
    W4dSoundManager::instance().registerSoundID("SID_ENTERED_APC",SID_ENTERED_APC);
    W4dSoundManager::instance().registerSoundID("SID_MINE_DROP",SID_MINE_DROP);
	W4dSoundManager::instance().registerSoundID("SID_MACHINE_SELF_DESTRUCT", SID_MACHINE_SELF_DESTRUCT);
	W4dSoundManager::instance().registerSoundID("SID_RESOURCE_PICKUP", SID_RESOURCE_PICKUP);
	W4dSoundManager::instance().registerSoundID("SID_IDLE_CONSTRUCTION", SID_IDLE_CONSTRUCTION);
	W4dSoundManager::instance().registerSoundID("SID_CONSTRUCTION_SELF_DESTRUCT", SID_CONSTRUCTION_SELF_DESTRUCT);
    W4dSoundManager::instance().registerSoundID("SID_WHEEL_MOVEMENT_LARGE", SID_WHEEL_MOVEMENT_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_WHEEL_IDLE_LARGE", SID_WHEEL_IDLE_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_WHEEL_MOVEMENT_SMALL", SID_WHEEL_MOVEMENT_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_WHEEL_IDLE_SMALL", SID_WHEEL_IDLE_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_TRACK_MOVEMENT_LARGE", SID_TRACK_MOVEMENT_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_TRACK_IDLE_LARGE", SID_TRACK_IDLE_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_TRACK_MOVEMENT_SMALL", SID_TRACK_MOVEMENT_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_TRACK_IDLE_SMALL", SID_TRACK_IDLE_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_MOVEMENT_LARGE", SID_SPIDER_MOVEMENT_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_IDLE_LARGE", SID_SPIDER_IDLE_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_MOVEMENT_SMALL", SID_SPIDER_MOVEMENT_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_IDLE_SMALL", SID_SPIDER_IDLE_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_HOVER_MOVEMENT_LARGE", SID_HOVER_MOVEMENT_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_HOVER_IDLE_LARGE", SID_HOVER_IDLE_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_HOVER_MOVEMENT_SMALL", SID_HOVER_MOVEMENT_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_HOVER_IDLE_SMALL", SID_HOVER_IDLE_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_NINJA_MOVEMENT", SID_NINJA_MOVEMENT);
    W4dSoundManager::instance().registerSoundID("SID_NINJA_IDLE", SID_NINJA_IDLE);
    W4dSoundManager::instance().registerSoundID("SID_BEE_BOMB", SID_BEE_BOMB);
    W4dSoundManager::instance().registerSoundID("SID_BEE_DETONATE", SID_BEE_DETONATE);
    W4dSoundManager::instance().registerSoundID("SID_FLY_BLAST_LARGE", SID_FLY_BLAST_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_FLY_BRAKE_LARGE", SID_FLY_BRAKE_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_ION_DEATH", SID_ION_DEATH);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE1_MACHINE", SID_XPLODE1_MACHINE);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE2_MACHINE", SID_XPLODE2_MACHINE);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE3_MACHINE", SID_XPLODE3_MACHINE);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE4_MACHINE", SID_XPLODE4_MACHINE);
    W4dSoundManager::instance().registerSoundID("SID_FLY_BLAST", SID_FLY_BLAST);
    W4dSoundManager::instance().registerSoundID("SID_WASP_FLY", SID_WASP_FLY);
    W4dSoundManager::instance().registerSoundID("SID_BEE_FLY", SID_BEE_FLY);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_MOVEMENT_VERY_LARGE", SID_SPIDER_MOVEMENT_VERY_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_SPIDER_IDLE_VERY_LARGE", SID_SPIDER_IDLE_VERY_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_APC_FLY_LARGE", SID_APC_FLY_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_APC_FLY_SMALL", SID_APC_FLY_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_GEO_FLY", SID_GEO_FLY);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE_CONSTR_LARGE", SID_XPLODE_CONSTR_LARGE);
    W4dSoundManager::instance().registerSoundID("SID_SMELTER1", SID_SMELTER1);
    W4dSoundManager::instance().registerSoundID("SID_SMELTER3", SID_SMELTER3);
    W4dSoundManager::instance().registerSoundID("SID_WASP_STING_METAL", SID_WASP_STING_METAL);
    W4dSoundManager::instance().registerSoundID("SID_WASP_STING_LIGHT", SID_WASP_STING_LIGHT);
    W4dSoundManager::instance().registerSoundID("SID_VORTEX_DEATH", SID_VORTEX_DEATH);
    W4dSoundManager::instance().registerSoundID("SID_RECYCLE", SID_RECYCLE);
    W4dSoundManager::instance().registerSoundID("SID_INHEAD", SID_INHEAD);
    W4dSoundManager::instance().registerSoundID("SID_XPLODE_CONSTR_SMALL", SID_XPLODE_CONSTR_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_FLY_BLAST_SMALL", SID_FLY_BLAST_SMALL);
    W4dSoundManager::instance().registerSoundID("SID_FLY_BRAKE_SMALL", SID_FLY_BRAKE_SMALL);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE5_MACHINE", SID_XPLODE5_MACHINE);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE6_MACHINE", SID_XPLODE6_MACHINE);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE1_MISSILE", SID_XPLODE1_MISSILE);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE2_MISSILE", SID_XPLODE2_MISSILE);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE3_MISSILE", SID_XPLODE3_MISSILE);
	W4dSoundManager::instance().registerSoundID("SID_XPLODE4_MISSILE", SID_XPLODE4_MISSILE);
	W4dSoundManager::instance().registerSoundID("SID_MULTILAUNCH", SID_MULTILAUNCH);
	W4dSoundManager::instance().registerSoundID("SID_LARGEMLAUNCH", SID_LARGEMLAUNCH);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH1", SID_MLAUNCH1);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH2", SID_MLAUNCH2);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH3", SID_MLAUNCH3);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH4", SID_MLAUNCH4);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH5", SID_MLAUNCH5);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH6", SID_MLAUNCH6);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH7", SID_MLAUNCH7);
	W4dSoundManager::instance().registerSoundID("SID_MLAUNCH8", SID_MLAUNCH8);
	W4dSoundManager::instance().registerSoundID("SID_CONTROL", SID_CONTROL);
	W4dSoundManager::instance().registerSoundID("SID_COOL1", SID_COOL1);
	W4dSoundManager::instance().registerSoundID("SID_POWER", SID_POWER);
	W4dSoundManager::instance().registerSoundID("SID_CONTRL", SID_CONTRL);
	W4dSoundManager::instance().registerSoundID("SID_SHIELD", SID_SHIELD);
	W4dSoundManager::instance().registerSoundID("SID_TILE22", SID_TILE22);
	W4dSoundManager::instance().registerSoundID("SID_CMAIN", SID_CMAIN);
	W4dSoundManager::instance().registerSoundID("SID_CDOM", SID_CDOM);
}

void MachPhysPreload::persistentSave( MachPhysPreload::DoLoadingProgressUpdate /*doUpdate*/ )
{
//    PER_LOG_ADDRESSES( true );

    //  TBD: Remove this - Bob
//    ProProfiler::instance().enableProfiling();

	std::ofstream ofstr( persistentFileName().c_str(), std::ios::binary );
	PerOstream ostr( ofstr );

	ostr << MachPhysPersistence::instance();
}

void MachPhysPreload::persistentPreload( MachPhysPreload::DoLoadingProgressUpdate doUpdate, PerIstreamReporter* pReporter )
{
    PRE( persistentFileName().existsAsFile() );

//  PER_LOG_ADDRESSES( true );

    doLoadingProgressUpdate( doUpdate );
	std::ifstream ifstr( persistentFileName().c_str(), std::ios::binary );
	PerIstream istr( ifstr, pReporter );

	istr >> MachPhysPersistence::instance();
    doLoadingProgressUpdate( doUpdate );
    extrasPreload( doUpdate );
}

void MachPhysPreload::doLoadingProgressUpdate( MachPhysPreload::DoLoadingProgressUpdate doUpdate )
{
	if( doUpdate == QUIET_PROGRESS )
		return;

	// No longer required due to new BaseProgressReporter. NA 19/11/98
	//static unsigned count = 0;
	//if( ( ++count % 2 ) == 0 )
	//	RenDevice::current()->displayImage("data/wait.bmp");
	//else
	//	RenDevice::current()->displayImage("data/wait2.bmp");
}

void MachPhysPreload::persistentFileName( const SysPathName& name )
{
	storedPersistentFileName() = name;
}

SysPathName& MachPhysPreload::storedPersistentFileName()
{
	static SysPathName fileName = "models.bin";
    return fileName;
}

/* End PRELOAD.CPP **************************************************/
