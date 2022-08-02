/*
 * W E A P O N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWeapon

    ABC for a physical weapon model. Performs all weapon animations.
*/

#ifndef _MACHPHYS_WEAPON_HPP
#define _MACHPHYS_WEAPON_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"
#include "machphys/machphys.hpp"

class MexLine3d;
class MexTransform3d;
class MachPhysWeaponData;
class MachPhysMachine;
class MachPhysConstruction;
class MachPhysLinearProjectile;
class SysPathName;
class RenColour;
class W4dRoot;
class MexPoint3d;

class PerOstream;
class PerIstream;
class MachPhysWeaponPersistence;

class MachPhysWeapon : public W4dComposite
// Canonical form revoked
{
public:
    //dtor
    virtual ~MachPhysWeapon();

    //Inherited from W4dEntity.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    //Export the weapon type, mounting
    MachPhys::WeaponType type() const;
    MachPhys::Mounting mounting() const;

    //Set/get the machine/construction on which the weapon is mounted
    void machine( MachPhysMachine* pMachine );
    //PRE( not hasConstruction () );
    bool hasMachine() const;
    MachPhysMachine& machine() const;
    //PRE( hasMachine() );

    void construction( MachPhysConstruction* pConstruction );
    //PRE( not hasMachine() );
    bool hasConstruction() const;
    MachPhysConstruction& construction() const;
    //PRE( hasConstruction() );

    //The number of launch offsets defined
    uint nLaunchOffsets() const;

    //The index'th launch offset from gun origin
    const MexPoint3d& localLaunchOffset( uint index ) const;
    //PRE( index < nLaunchOffsets() );

    //Returns the index'th launch position as a global transform in pTransform
    void globalLaunchPosition( uint index, MexTransform3d* pTransform ) const;
    //PRE( index < nLaunchOffsets() );

    //Export the associated weapon data object
    const MachPhysWeaponData& weaponData() const;

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    //Default implementation does nothing.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //Play the victim animation for a machine or construction at startTime.
    //fromDirection indicates the flight path of the hitting projectile.
    //Returns the duration of the animation.
    //Default implementation does nothing.
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysMachine* pMachine ) const;
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysConstruction* pConstruction ) const;

	//change the race of the weapon to that of the machine
	void changeRace();

	//change the colour of the weapon to that of the new race
	void changeColour( MachPhys::Race newRace );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWeapon& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysWeapon );
    PER_FRIEND_READ_WRITE( MachPhysWeapon );
protected:
    //ctor.
    MachPhysWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                    const SysPathName& compositeFileName,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //Similar copy ctor.
    MachPhysWeapon( const MachPhysWeapon& copyMe, MachPhys::Mounting mounting,
                    W4dEntity* pParent, const W4dTransform3d& localTransform );

    //Perform standard recoil animation as defined in the parmdata file at startTime.
    //return duration of animation.
    PhysRelativeTime recoil( const PhysAbsoluteTime& startTime, int numberInBurst );

    //Get/set the next launch offset index to use
    uint nextLaunchOffset() const;
    void nextLaunchOffset( uint index );

	//when a  gun fires, it create  light(s)  and maybe shadows as well, so that entity is lit up.
	//The lights start to illuminate at startTime within distance
	enum Shadow{NO_SHADOW, SHADOW};
	void lighting(const RenColour& col, const PhysAbsoluteTime& startTime, const MATHEX_SCALAR distance, MachPhysWeapon::Shadow shadow = NO_SHADOW );

	PER_FRIEND_ENUM_PERSISTENT( MachPhysWeapon::Shadow );

	bool firedLightRecently(PhysRelativeTime launchTime) const;
	bool playedSoundRecently(PhysRelativeTime launchTime) const;

private:
    //revoked
    MachPhysWeapon( const MachPhysWeapon& );
    MachPhysWeapon& operator =( const MachPhysWeapon& );
    bool operator ==( const MachPhysWeapon& );

    //Set up the permanent race colour overrides according to the race of the
    //machine the weapon is mounted on
    void convertMaterials();

    //data members
    MachPhys::WeaponType type_; //Type of weapon of derived class
    MachPhys::Mounting mounting_; //Mounting location on the machine
    int recoilId_; //Identifies entity to recoil (0 => none; 1 => whole weapon; n =. link[n-2])
    uint nextLaunchOffset_; //Index of the next launch offset to use
    MachPhysMachine* pMachine_; //The machine the weapon is mounted on, if any
    MachPhysConstruction* pConstruction_; //The construction the weapon is mounted on, if any
};

PER_DECLARE_PERSISTENT( MachPhysWeapon );

#ifdef _INLINE
    #include "machphys/weapon.ipp"
#endif


#endif

/* End WEAPON.HPP ***************************************************/
