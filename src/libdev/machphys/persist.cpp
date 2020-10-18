/*
 * P E R S I S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/persist.hpp"

#include "stdlib/string.hpp"

#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/persist.hpp"

#include "machphys/vappuff.hpp"

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

#include "machphys/smokplum.hpp"
#include "machphys/smokpuff.hpp"
#include "machphys/oreholo.hpp"
#include "machphys/weapon.hpp"
#include "machphys/lineproj.hpp"

#include "machphys/wheels.hpp"
#include "machphys/spider.hpp"
#include "machphys/tracks.hpp"
#include "machphys/hover.hpp"
#include "machphys/glider.hpp"

#include "machphys/internal/wheelsi.hpp"
#include "machphys/internal/spideri.hpp"
#include "machphys/internal/tracksi.hpp"
#include "machphys/internal/hoveri.hpp"
#include "machphys/internal/glideri.hpp"

#include "machphys/pulsaura.hpp"
#include "machphys/pulsplat.hpp"
#include "machphys/plasaura.hpp"
#include "machphys/plasplat.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/plasmbol.hpp"
#include "machphys/electro.hpp"
#include "machphys/flames.hpp"
#include "machphys/stflames.hpp"
#include "machphys/flamebal.hpp"
#include "machphys/pulseblb.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/trechorb.hpp"
#include "machphys/fireball.hpp"
#include "machphys/radidisc.hpp"
#include "machphys/flamethr.hpp"
#include "machphys/elecharg.hpp"
#include "machphys/virusspr.hpp"
#include "machphys/mulaunch.hpp"
#include "machphys/treachry.hpp"
#include "machphys/bolter.hpp"

#include "machphys/beam.hpp"
#include "machphys/bsphere.hpp"
#include "machphys/debris.hpp"
#include "machphys/debrisun.hpp"
#include "machphys/flash.hpp"
#include "machphys/halo.hpp"
#include "machphys/healaura.hpp"
#include "machphys/healhelx.hpp"
#include "machphys/helix.hpp"
#include "machphys/ionbeam.hpp"
#include "machphys/lmine.hpp"
#include "machphys/particle.hpp"
#include "machphys/radidisc.hpp"
#include "machphys/smkcloud.hpp"
#include "machphys/terrtile.hpp"
#include "machphys/vortbomb.hpp"
#include "machphys/wsphere.hpp"
#include "machphys/vappuff.hpp"
#include "machphys/vsphere.hpp"

#include "machphys/missile.hpp"
#include "machphys/largemsl.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/nuclweap.hpp"
#include "machphys/vortweap.hpp"
#include "machphys/charger.hpp"

#include "machphys/nuclbomb.hpp"
#include "machphys/mushtop.hpp"
#include "machphys/mushedge.hpp"
#include "machphys/mushshaf.hpp"
#include "machphys/splat.hpp"
#include "machphys/shockwav.hpp"
#include "machphys/damagwav.hpp"
#include "machphys/nukewave.hpp"

#include "machphys/jetringm.hpp"
#include "machphys/hsphere.hpp"
#include "machphys/sparks.hpp"
#include "machphys/marker.hpp"

#include "machphys/dblflame.hpp"
#include "machphys/waterfal.hpp"

#include "machphys/punweap.hpp"
#include "machphys/punch.hpp"
#include "machphys/blast.hpp"
#include "machphys/crakfire.hpp"
#include "machphys/light.hpp"

#include "machphys/objdemol.hpp"
#include "machphys/facepl.hpp"
#include "machphys/beeweap.hpp"
#include "machphys/beebomb.hpp"
#include "machphys/beewave.hpp"

#include "machphys/stingl.hpp"
#include "machphys/stingm.hpp"
#include "machphys/stglwep.hpp"
#include "machphys/stgmwep.hpp"
#include "machphys/stlaura.hpp"
#include "machphys/stlsplat.hpp"

#include "machphys/loading.hpp"
#include "machphys/loadligh.hpp"
#include "machphys/loadunde.hpp"
#include "machphys/loadover.hpp"
#include "machphys/scavenge.hpp"
#include "machphys/locator.hpp"

#include "machphys/private/machper.hpp"
#include "machphys/private/consper.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/private/otherper.hpp"

#include "world4d/persist.hpp"

PER_DEFINE_PERSISTENT( MachPhysPersistence );

// static
MachPhysPersistence& MachPhysPersistence::instance()
{
    static MachPhysPersistence instance_;
    return instance_;
}

MachPhysPersistence::MachPhysPersistence()
{
    W4dPersistence::instance();
    PhysPersistence::instance();

    PER_REGISTER_DERIVED_CLASS( MachPhysAggressor );
    PER_REGISTER_DERIVED_CLASS( MachPhysGeoLocator );
    PER_REGISTER_DERIVED_CLASS( MachPhysSpyLocator );
    PER_REGISTER_DERIVED_CLASS( MachPhysResourceCarrier );
    PER_REGISTER_DERIVED_CLASS( MachPhysAPC );
    PER_REGISTER_DERIVED_CLASS( MachPhysTechnician );
    PER_REGISTER_DERIVED_CLASS( MachPhysConstructor );
    PER_REGISTER_DERIVED_CLASS( MachPhysAdministrator );
    PER_REGISTER_DERIVED_CLASS( MachPhysBeacon );
    PER_REGISTER_DERIVED_CLASS( MachPhysFactory );
    PER_REGISTER_DERIVED_CLASS( MachPhysGarrison );
    PER_REGISTER_DERIVED_CLASS( MachPhysMine );
    PER_REGISTER_DERIVED_CLASS( MachPhysMissileEmplacement );
    PER_REGISTER_DERIVED_CLASS( MachPhysPod );
    PER_REGISTER_DERIVED_CLASS( MachPhysSmelter );
    PER_REGISTER_DERIVED_CLASS( MachPhysMissile );
    PER_REGISTER_DERIVED_CLASS( MachPhysFacePlate );
    PER_REGISTER_DERIVED_CLASS( MachPhysHardwareLab );

    PER_REGISTER_DERIVED_CLASS( MachPhysSmokePlume );
    PER_REGISTER_DERIVED_CLASS( MachPhysSmokePuff );
    PER_REGISTER_DERIVED_CLASS( MachPhysOreHolograph );
    PER_REGISTER_DERIVED_CLASS( MachPhysWeapon );

    PER_REGISTER_DERIVED_CLASS( MachPhysWheels );
    PER_REGISTER_DERIVED_CLASS( MachPhysHoverBoots );
    PER_REGISTER_DERIVED_CLASS( MachPhysSpiderLegs );
    PER_REGISTER_DERIVED_CLASS( MachPhysTracks );
    PER_REGISTER_DERIVED_CLASS( MachPhysGlider );

    PER_REGISTER_DERIVED_CLASS( MachPhysWheelsImpl );
    PER_REGISTER_DERIVED_CLASS( MachPhysHoverBootsImpl );
    PER_REGISTER_DERIVED_CLASS( MachPhysSpiderLegsImpl );
    PER_REGISTER_DERIVED_CLASS( MachPhysTracksImpl );
    PER_REGISTER_DERIVED_CLASS( MachPhysGliderImpl );

    PER_REGISTER_DERIVED_CLASS( MachPhysLinearWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysLinearProjectile );
    PER_REGISTER_DERIVED_CLASS( MachPhysWeapon );

    PER_REGISTER_DERIVED_CLASS( MachPhysPlasmaWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysPlasmaBolt );
    PER_REGISTER_DERIVED_CLASS( MachPhysElectro );
    PER_REGISTER_DERIVED_CLASS( MachPhysFlameBall );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseBlob );
    PER_REGISTER_DERIVED_CLASS( MachPhysTreacheryOrb );
    PER_REGISTER_DERIVED_CLASS( MachPhysFireball );
    PER_REGISTER_DERIVED_CLASS( MachPhysRadialDisc );
    PER_REGISTER_DERIVED_CLASS( MachPhysPlasmaSplat );
    PER_REGISTER_DERIVED_CLASS( MachPhysPlasmaAura );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseSplat );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseAura );

    PER_REGISTER_DERIVED_CLASS( MachPhysElectroCharger );
    PER_REGISTER_DERIVED_CLASS( MachPhysBolter );
    PER_REGISTER_DERIVED_CLASS( MachPhysFlameThrower );
    PER_REGISTER_DERIVED_CLASS( MachPhysMultiLauncher );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseRifle );
    PER_REGISTER_DERIVED_CLASS( MachPhysPulseCannon );
    PER_REGISTER_DERIVED_CLASS( MachPhysVirusSpreader );
    PER_REGISTER_DERIVED_CLASS( MachPhysTreacheryWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysLargeMissile );
    PER_REGISTER_DERIVED_CLASS( MachPhysIonWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysVortexWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysNuclearWeapon );
    PER_REGISTER_DERIVED_CLASS( MachPhysSuperCharger );
    PER_REGISTER_DERIVED_CLASS( MachPhysPunchWeapon );

    PER_REGISTER_DERIVED_CLASS( MachPhysLight );
    PER_REGISTER_DERIVED_CLASS( MachPhysWhiteSphere );
    PER_REGISTER_DERIVED_CLASS( MachPhysVortexSphere );
    PER_REGISTER_DERIVED_CLASS( MachPhysVortexBomb );
    PER_REGISTER_DERIVED_CLASS( MachPhysTerrainTile );
    PER_REGISTER_DERIVED_CLASS( MachPhysSmokeCloud );
    PER_REGISTER_DERIVED_CLASS( MachPhysRadialDisc );
    PER_REGISTER_DERIVED_CLASS( MachPhysParticles );
    PER_REGISTER_DERIVED_CLASS( MachPhysLandMine );
    PER_REGISTER_DERIVED_CLASS( MachPhysIonBeam );
    PER_REGISTER_DERIVED_CLASS( MachPhysHealHelix );
    PER_REGISTER_DERIVED_CLASS( MachPhysHelix );
    PER_REGISTER_DERIVED_CLASS( MachPhysHealAura );
    PER_REGISTER_DERIVED_CLASS( MachPhysFlashDisc );
    PER_REGISTER_DERIVED_CLASS( MachPhysDebrisUnit );
    PER_REGISTER_DERIVED_CLASS( MachPhysDebris );
    PER_REGISTER_DERIVED_CLASS( MachPhysBeam );
    PER_REGISTER_DERIVED_CLASS( MachPhysBlackSphere );
    PER_REGISTER_DERIVED_CLASS( MachPhysLightningHalo );
    PER_REGISTER_DERIVED_CLASS( MachPhysVapourPuff );

    PER_REGISTER_DERIVED_CLASS( MachPhysNuclearBomb );
    PER_REGISTER_DERIVED_CLASS( MachPhysMushroomTop );
    PER_REGISTER_DERIVED_CLASS( MachPhysMushroomEdge );
    PER_REGISTER_DERIVED_CLASS( MachPhysMushroomShaft );
    PER_REGISTER_DERIVED_CLASS( MachPhysGroundSplat );
    PER_REGISTER_DERIVED_CLASS( MachPhysShockWave );
    PER_REGISTER_DERIVED_CLASS( MachPhysBurstWave );
    PER_REGISTER_DERIVED_CLASS( MachPhysNukeWave );

    PER_REGISTER_DERIVED_CLASS( MachPhysHemiSphere );
    PER_REGISTER_DERIVED_CLASS( MachPhysJetRing );
    PER_REGISTER_DERIVED_CLASS( MachPhysSparks );

    PER_REGISTER_DERIVED_CLASS( MachPhysPunchBlast );
    PER_REGISTER_DERIVED_CLASS( MachPhysBlast );
    PER_REGISTER_DERIVED_CLASS( MachPhysCrackFire );

    PER_REGISTER_DERIVED_CLASS( MachPhysDoublesidedFlame );
    PER_REGISTER_DERIVED_CLASS( MachPhysFlame );
    PER_REGISTER_DERIVED_CLASS( MachPhysSTFFlame );
    PER_REGISTER_DERIVED_CLASS( MachPhysWaterfall );
    PER_REGISTER_DERIVED_CLASS( MachPhysMarker );

    PER_REGISTER_DERIVED_CLASS( MachPhysObjDemolish );

    PER_REGISTER_DERIVED_CLASS( MachPhysBeeBomber );
    PER_REGISTER_DERIVED_CLASS( MachPhysBeeBomb );
    PER_REGISTER_DERIVED_CLASS( MachPhysBeeBombWave );

    PER_REGISTER_DERIVED_CLASS( MachPhysLightSting );
    PER_REGISTER_DERIVED_CLASS( MachPhysLightStingAura );
    PER_REGISTER_DERIVED_CLASS( MachPhysLightStingSplat );
    PER_REGISTER_DERIVED_CLASS( MachPhysLightStingWeapon );

    PER_REGISTER_DERIVED_CLASS( MachPhysMetalSting );
    PER_REGISTER_DERIVED_CLASS( MachPhysMetalStingWeapon );

    PER_REGISTER_DERIVED_CLASS( MachPhysResourceLoading );
    PER_REGISTER_DERIVED_CLASS( MachPhysLoadingLight );
    PER_REGISTER_DERIVED_CLASS( MachPhysLoadingUnder );
    PER_REGISTER_DERIVED_CLASS( MachPhysLoadingOver );
    PER_REGISTER_DERIVED_CLASS( MachPhysScavenger );
    PER_REGISTER_DERIVED_CLASS( MachPhysLocator );

    TEST_INVARIANT;
}

MachPhysPersistence::~MachPhysPersistence()
{
    TEST_INVARIANT;

}

const MachPhysMachinePersistence& MachPhysPersistence::machines() const
{
    return MachPhysMachinePersistence::instance();
}

const MachPhysConstructionPersistence& MachPhysPersistence::constructions() const
{
    return MachPhysConstructionPersistence::instance();
}

const MachPhysWeaponPersistence& MachPhysPersistence::weapons() const
{
    return MachPhysWeaponPersistence::instance();
}

const MachPhysOtherPersistence& MachPhysPersistence::others() const
{
    return MachPhysOtherPersistence::instance();
}

MachPhysOtherPersistence& MachPhysPersistence::others()
{
    return MachPhysOtherPersistence::instance();
}

MachPhysConstructionPersistence& MachPhysPersistence::constructions()
{
    return MachPhysConstructionPersistence::instance();
}

MachPhysWeaponPersistence& MachPhysPersistence::weapons()
{
    return MachPhysWeaponPersistence::instance();
}

MachPhysMachinePersistence& MachPhysPersistence::machines()
{
    return MachPhysMachinePersistence::instance();
}

void MachPhysPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPersistence& t )
{

    o << "MachPhysPersistence " << (void*)&t << " start" << std::endl;
    o << "MachPhysPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysPersistence& per )
{
// PER_LOG_WRITE( true );

    ostr << per.machines();
    ostr << per.constructions();
    ostr << per.weapons();
    ostr << per.others();
}

void perRead( PerIstream& istr, MachPhysPersistence& per )
{
// PER_LOG_READ( true );

    istr >> per.machines();
    istr >> per.constructions();
    istr >> per.weapons();
    istr >> per.others();
}

/* End PERSIST.CPP **************************************************/
