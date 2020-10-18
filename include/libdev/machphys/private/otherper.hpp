/*
 * O T H E R P E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysOtherPersistence

    Perist all of the physical weapon models
*/

#ifndef _MACHPHYS_OTHERPER_HPP
#define _MACHPHYS_OTHERPER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/root.hpp"

#include "machphys/machphys.hpp"

#include "machphys/electro.hpp"
#include "machphys/helix.hpp"
#include "machphys/halo.hpp"
#include "machphys/crakfire.hpp"
#include "machphys/flash.hpp"
#include "machphys/flamebal.hpp"

class MachPhysPlasmaBolt;
class MachPhysPulseBlob;

class MachPhysBeam;
class MachPhysBlast;
class MachPhysBlackSphere;
class MachPhysCrackFire;
class MachPhysBurstWave;
class MachPhysDoublesidedFlame;
class MachPhysElectro;
class MachPhysLightningHalo;
class MachPhysHealAura;
class MachPhysHelix;
class MachPhysHemiSphere;
class MachPhysJetRing;
class MachPhysMissile;
class MachPhysMushroomEdge;
class MachPhysMushroomShaft;
class MachPhysMushroomTop;
class MachPhysParticles;
class MachPhysRadialDisc;
//class MachPhysGroundScorch;
class MachPhysShockWave;
class MachPhysSparks;
class MachPhysGroundSplat;
class MachPhysWaterfall;
class MachPhysWhiteSphere;
class MachPhysNukeWave;
//class MachPhysPulseBlob;

class MachPhysPlasmaAura;
class MachPhysPlasmaSplat;
class MachPhysPulseAura;
class MachPhysPulseSplat;

class MachPhysVortexSphere;
class MachPhysBeeBomb;
class MachPhysBeeBombWave;
class MachPhysBeeBlast;

class MachPhysLightSting;
class MachPhysLightStingSplat;
class MachPhysLightStingAura;
class MachPhysMetalSting;

class MachPhysFlashDisc;

class MachPhysLoadingLight;
class MachPhysLoadingOver;
class MachPhysLoadingUnder;

class MachPhysScavenger;
class MachPhysLocator;
class MachPhysDebrisUnit;
class MachPhysOreHolograph;

class MachPhysLight;
class MachPhysLandMine;

class W4dGeneric;

class MachPhysOtherPersistenceImpl;

class MachPhysOtherPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysOtherPersistence& instance( void );
    ~MachPhysOtherPersistence( void );

    W4dRoot* pRoot();

	const MachPhysPulseBlob& pulseBlobExemplar(MachPhys::WeaponType type);
	const MachPhysPlasmaBolt& plasmaBoltExemplar(MachPhys::WeaponType type);

	const MachPhysBeam& beamExemplar();
	const MachPhysParticles& particlesExemplar();

	const MachPhysBlackSphere& blackSphereExemplar();
	const MachPhysWhiteSphere& whiteSphereExemplar();

	const MachPhysCrackFire& crackFireExemplar( MachPhysCrackFire::ChasmType type);
	const MachPhysBlast& blastExemplar();

	const MachPhysElectro& electroExemplar( MachPhysElectro::FlashType type );
	const MachPhysLightningHalo& haloExemplar(MachPhysLightningHalo::HaloType type);

	const W4dGeneric& healAuraCentralCrossExemplar();
	const W4dGeneric& healAuraRadialCrossExemplar();

	const MachPhysHelix& helixExemplar(MachPhysHelix::Twist twist);

	const MachPhysJetRing& jetRingExemplar();
	const MachPhysSparks& sparksExemplar();
	const MachPhysHemiSphere& hemiSphereExemplar();

	const MachPhysMushroomEdge&  mushroomEdgeExemplar();
	const MachPhysMushroomShaft& mushroomShaftExemplar();
	const MachPhysMushroomTop&   mushroomTopExemplar();

	const MachPhysRadialDisc& radialDiscExemplar();
	const MachPhysWaterfall& waterfallExemplar();

	const MachPhysDoublesidedFlame& doubleSidedFlameExemplar();

	const MachPhysShockWave& shockWaveExemplar();
	const MachPhysBurstWave& burstWaveExemplar();
	const MachPhysNukeWave& nukeWaveExemplar();

	//const MachPhysGroundScorch& groundScorchExemplar();
	const MachPhysGroundSplat& groundSplatExemplar();

	const MachPhysPlasmaAura& 		plasmaAuraExemplar();
	const MachPhysPlasmaSplat&  	plasmaSplatExemplar();
	const MachPhysPulseAura&  		pulseAuraExemplar();
	const MachPhysPulseSplat& 	 	pulseSplatExemplar();

	const MachPhysVortexSphere& 	vortexSphereExemplar();

	const MachPhysBeeBomb&		beeBombExemplar();
	const MachPhysBeeBlast&		beeBlastExemplar();
	const MachPhysBeeBombWave&  beeBombWaveExemplar();

	const MachPhysLightSting&			lightStingExemplar();
	const MachPhysLightStingSplat&		lightStingSplatExemplar();
	const MachPhysLightStingAura&		lightStingAuraExemplar();
	const MachPhysMetalSting&			metalStingExemplar();

	const MachPhysFlashDisc& 	flashDiscExemplar( const MATHEX_SCALAR& size, MachPhysFlashDisc::ColourType colour );

	const MachPhysLoadingLight&	lightLoadingExemplar();
	const MachPhysLoadingOver&	overLoadingExemplar();
	const MachPhysLoadingUnder&	underLoadingExemplar();

	const MachPhysScavenger& scavengerExemplar();
	const MachPhysLocator& locatorExemplar();

	const MachPhysFlameBall& flameBallExemplar( MachPhysFlameBall::FlameBallType type );
	const MachPhysDebrisUnit& debrisExemplar();
	const MachPhysOreHolograph& holographExemplar();

	const MachPhysLight& lightShadowExemplar();
	const MachPhysLandMine& landMineExemplar();


    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysOtherPersistence& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysOtherPersistence );
    PER_FRIEND_READ_WRITE( MachPhysOtherPersistence );

private:
    MachPhysOtherPersistence( const MachPhysOtherPersistence& );
    MachPhysOtherPersistence& operator =( const MachPhysOtherPersistence& );
    bool operator ==( const MachPhysOtherPersistence& );

    MachPhysOtherPersistence( void );

	MachPhysOtherPersistenceImpl* pImpl_;
    W4dRoot root_;
};

PER_DECLARE_PERSISTENT( MachPhysOtherPersistence );

#endif

/* End OTHERPER.HPP **************************************************/
