/*
 * W E P D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWeaponData

    Encapsulates the profile data for a given type of weapon.
*/

#ifndef _MACHPHYS_WEPDATA_HPP
#define _MACHPHYS_WEPDATA_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/radians.hpp"
#include "phys/phys.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"
#include "stdlib/string.hpp"

class MachPhysWeaponData
// Canonical form revoked
{
public:
    //dtor
    ~MachPhysWeaponData();

    //Offsets to the launch point for projectiles
    typedef ctl_vector< MexPoint3d > LaunchOffsets;
    typedef ctl_vector< MATHEX_SCALAR > Extras;

    //Return the various attributes
    int damagePoints() const;
    MATHEX_SCALAR range() const;
    MATHEX_SCALAR projectileSpeed() const;
    uint nRoundsPerBurst() const;
	bool fixedBurst() const;
    PhysRelativeTime burstDuration() const;
    PhysRelativeTime reloadTime() const;
    const LaunchOffsets& launchOffsets() const;
    MATHEX_SCALAR recoilDistance() const;
    PhysRelativeTime recoilBackTime() const;
    PhysRelativeTime recoilForeTime() const;
    const Extras& extras() const;
    bool canTilt() const;
    const MexRadians& minTiltAngle() const;
    const MexRadians& maxTiltAngle() const;
    const MexRadians& verticalDeviationAngle() const;
    const MexRadians& horizontalDeviationAngle() const;
    double sinVerticalDeviationAngle() const;
    double sinHorizontalDeviationAngle() const;
    double tanVerticalDeviationAngle() const;
    double tanHorizontalDeviationAngle() const;

	bool trailOn() const;
	const RenColour& trailColour() const;
	ctl_vector< RenTexture > trailTextures() const;

	bool flameRotate() const;
	bool flameScale() const;
	MATHEX_SCALAR flameScaleFrom() const;
	MATHEX_SCALAR flameScaleTo() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWeaponData& t );

private:
    //Allows parser to create
	friend class MachPhysDataParser;

    //revoked
    MachPhysWeaponData( const MachPhysWeaponData& );
    MachPhysWeaponData& operator =( const MachPhysWeaponData& );
    bool operator ==( const MachPhysWeaponData& );

    //ctor. Only parser can construct.
    MachPhysWeaponData();

    //Set the various attributes - only done by parser
    void damagePoints( int newDamagePoint );
    void range( MATHEX_SCALAR newRange );
    void projectileSpeed( MATHEX_SCALAR newProjectileSpeed );
    void nRoundsPerBurst( uint newNRoundsPerBurst );
	void fixedBurst( bool status );	
    void burstDuration( PhysRelativeTime newBurstDuration );
    void reloadTime( PhysRelativeTime newReloadTime );
    void launchOffsets( const LaunchOffsets& offsets );
    void recoilDistance( MATHEX_SCALAR distance );
    void recoilBackTime( PhysRelativeTime time );
    void recoilForeTime( PhysRelativeTime time );
    void extras( const Extras& values );
    void tiltAngles( const MexRadians& minAngle, const MexRadians& maxAngle );
    void verticalDeviationAngle( const MexRadians& angle );
    void horizontalDeviationAngle( const MexRadians& angle );

	void trailOn( bool );
	void trailColour(const RenColour&  col );
	void trailTextureKey( const string& key );
	void trailStartTextureIndex( size_t startIndex );
	void trailEndTextureIndex( size_t endIndex);

	void flameRotate( bool );
	void flameScale( bool );
	void flameScaleFrom( MATHEX_SCALAR startScale );
	void flameScaleTo( MATHEX_SCALAR endScale);

    //data members
    int damagePoints_; //Number of points of damage inflicted by a single hit (-ve implies healing)
    MATHEX_SCALAR range_; //Weapon range in metres
    MATHEX_SCALAR projectileSpeed_; //Speed of any projectile
    uint nRoundsPerBurst_; //Number of rounds fired in one burst
	bool fixedBurst_;	// whether or not the number in a burst varies randomly between 1 and nRoundsPerBurst_ or is always nRoundsPerBurst_
    PhysRelativeTime burstDuration_; //Time taken to fire a single burst (all rounds)
    PhysRelativeTime reloadTime_; //Minimum interval between start of consecutive burst
    MATHEX_SCALAR recoilDistance_; //Distance recoil link moves in -x
    PhysRelativeTime recoilBackTime_; //Time recoil link spends moving backwards
    PhysRelativeTime recoilForeTime_; //Time recoil link spends moving forwards
    ctl_vector< MexPoint3d > launchOffsets_; //The offsets to projectile launch points from
                                             //weapon origin.
    ctl_vector< MATHEX_SCALAR > extras_; //Data specific to weapon type
    MexRadians minTiltAngle_; //Extreme up tilt angle
    MexRadians maxTiltAngle_; //Extreme down tilt angle
    MexRadians verticalDeviationAngle_; //Max pitch angle projectile may be fired from straight in front
    MexRadians horizontalDeviationAngle_; //Max yaw angle projectile may be fired from straight in front

	//missile flame and trail data
	bool			flameRotate_;		//if true, then roted randomly (0, 360) round x
	bool			flameScale_;
	MATHEX_SCALAR   flameScaleFrom_;
	MATHEX_SCALAR   flameScaleTo_;

	bool 		trailOn_;			 //if true then use the colou and textures. if not use smoke
	RenColour	trailColour_;
	string		trailTextureKey_;
	size_t		trailStartTextureIndex_;
	size_t		trailEndTextureIndex_;
};

#ifdef _INLINE
    #include "machphys/wepdata.ipp"
#endif


#endif

/* End WEPDATA.HPP **************************************************/
