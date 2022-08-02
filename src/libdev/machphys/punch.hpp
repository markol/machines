/*
 * P U N C H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPunchBlast

    Models the gorrila punch animation
*/

#ifndef _MACHPHYS_PUNCH_HPP
#define _MACHPHYS_PUNCH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"
#include "ctl/vector.hpp"
#include "mathex/point3d.hpp"

class MachPhysPlanetSurface;

class MachPhysPunchBlast : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysPunchBlast(W4dEntity* pParent, const MexTransform3d& localTransform);

    //dtor
    virtual ~MachPhysPunchBlast();

	enum  Crack_Type : unsigned char { PUNCH, NUKE }; //a nuclear explosion also creates cracks and fire( but not blast).
    //Begin the explosion animation at startTime.
    //Returns the duration of the explosion.
    PhysRelativeTime startPunchBlast( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface, const Crack_Type& type=PUNCH );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	static const MATHEX_SCALAR& punchFrameRate();
	static bool reachDistance(const MATHEX_SCALAR& distance, PhysRelativeTime* pTime);
	static MATHEX_SCALAR range();
	static MexRadians absMaxDestructionAngle();             // assumes near-symmetrical fan damage pattern
	static PhysRelativeTime duration();
	static MATHEX_SCALAR radius( const PhysRelativeTime& timeOffset );

	PhysAbsoluteTime blastBeginTime() const;
    // PRE( startPunchBlastCalled() );
    PhysAbsoluteTime blastEndTime() const;
    // PRE( startPunchBlastCalled() );
    bool startPunchBlastCalled() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPunchBlast& t );
    typedef ctl_vector<MexPoint3d> Points;	// MexPoint3d( x, y, startFrame)

    PER_MEMBER_PERSISTENT( MachPhysPunchBlast );
    PER_FRIEND_READ_WRITE( MachPhysPunchBlast );

private:

    //Deliberately revoked
    MachPhysPunchBlast( const MachPhysPunchBlast& );
    MachPhysPunchBlast& operator =( const MachPhysPunchBlast& );
    bool operator ==( const MachPhysPunchBlast& );

	static const uint nCrackFire();

	//typedef ctl_vector<MexPoint3d> Points;	// MexPoint3d( x, y, startFrame)

	// data members
    PhysAbsoluteTime blastBeginTime_;               // defaults to 0, initialised by startPunchBlast method
    PhysAbsoluteTime blastEndTime_;                 // defaults to 0, initialised by startPunchBlast method
    bool startPunchBlastCalled_;
};

PER_DECLARE_PERSISTENT( MachPhysPunchBlast );
PER_ENUM_PERSISTENT( MachPhysPunchBlast::Crack_Type );
#endif

/* End PUNCH.HPP *************************************************/
