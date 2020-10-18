/*
 * L I N E P R O J . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogStretchy

    Base class for stretchy weapons which at present is the set:
		Electric charge
		Virus
		Healing( multiple flavours ).
*/

#ifndef _MACHLOG_STRETCHY_HPP
#define _MACHLOG_STRETCHY_HPP

#include "base/base.hpp"

#include "mathex/point3d.hpp"

#include "phys/phys.hpp"
#include "machphys/machphys.hpp"
#include "sim/projecti.hpp"

class MexPoint3d;
class MachPhysStretchy;
class MachLogRace;
class MachActor;

class MachLogStretchy : public SimProjectile
{
public:
	enum DamageCurveShape {
		CONSTANT_DAMAGE,
		R_RECIPROCAL_DAMAGE,
		R_SQUARED_DAMAGE,
	};

    MachLogStretchy(
        MachLogRace* pRace,
        size_t level,
        const MexPoint3d& startPosition,
        const MexVec3& direction,
        MachPhysStretchy*,
        PhysRelativeTime timeOffset,
        MATHEX_SCALAR speed,
        MATHEX_SCALAR maxRange,
        MachActor* pOwner,
        int damage );

    virtual ~MachLogStretchy( void );

    // Override updates the logical machine's Ai process.
    // Clearance ignored for now.
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogStretchy& t );

    const MexPoint3d&   globalDestructionPosition() const;

	int damage() const;

	static void genericCheckForDamage( const MexPoint3d&, MATHEX_SCALAR range,
										DamageCurveShape, int damage, 
										MachPhys::WeaponType wt = MachPhys::N_WEAPON_TYPES,
										MexLine3d* line = NULL );	

protected:
    MATHEX_SCALAR   rangeToStationaryObject(
        const MexPoint3d& globalStartPosition,
        const MexPoint3d& globalEndPosition,
        MATHEX_SCALAR range );

    MATHEX_SCALAR   rangeToMovingObject(
        const MexPoint3d& globalStartPosition,
        const MexPoint3d& globalEndPosition,
        MATHEX_SCALAR range );

    MachPhysStretchy& physStretchy( void );
    const MachPhysStretchy& physStretchy( void ) const;

	virtual void doBeDestroyed() = 0;

	void checkForDamage( MATHEX_SCALAR range, DamageCurveShape, MachPhys::WeaponType );	

private:
    // Operation deliberately revoked
    MachLogStretchy( const MachLogStretchy& );

    // Operation deliberately revoked
    MachLogStretchy& operator =( const MachLogStretchy& );

    // Operation deliberately revoked
    bool operator ==( const MachLogStretchy& );

//    static MachPhysStretchy* pNewPhysStretchy(
//        size_t level, const MexPoint3d& startPosition );
        
    void destroyAt( const PhysAbsoluteTime& time );


    PhysAbsoluteTime    destructionTime_;
    MexPoint3d          globalDestructionPosition_;
	MATHEX_SCALAR		speed_;
	MATHEX_SCALAR		maxRange_;
	MachActor*			pOwner_;
	MachPhys::StrikeType	strikeType_;
	int					damage_;
};


#endif

/* End LINEAR_PROJECTILE.HPP **************************************************/
