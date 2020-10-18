/*
 * L M I N E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogLandMine

    Represents a proximity based land mine
*/

#ifndef _MACHLOG_OREHOLO_HPP
#define _MACHLOG_OREHOLO_HPP

#include "base/base.hpp"

#include "machlog/actor.hpp"

//forward refs
class MachLogRace;
class MexPoint3d;
class MachPhysLandMine;

//orthodox canonical( revoked )
class MachLogLandMine : public MachActor
{
public:
    //ctor
    MachLogLandMine( MachLogRace* pRace, const MexPoint3d& location );

    MachLogLandMine( MachLogRace* pRace, const MexPoint3d& location, UtlId );

    //dtor.
    virtual ~MachLogLandMine( void );

	//view of MachPhysObject data
	virtual const MachPhysObjectData& objectData() const;

    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

    void CLASS_INVARIANT;

	///////////////////////////////

	// inherited from MachLogCanBeDestroyed
	virtual PhysRelativeTime beDestroyed();

	//Amount of damage is passed in beHit()
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES);
	
	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo );

	int damage() const;	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLandMine );
	PER_FRIEND_READ_WRITE( MachLogLandMine );

protected:
	virtual void doStartExplodingAnimation();
	virtual void doEndExplodingAnimation();

private:
    //Subclass must override to modify the display.
    //Called on change of state.
    virtual void doVisualiseSelectionState();
    // Operations deliberately revoked
    MachLogLandMine( const MachLogLandMine& );
    MachLogLandMine& operator =( const MachLogLandMine& );
    bool operator ==( const MachLogLandMine& );

	bool thisIsClosestLandmineTo( const MexPoint2d& targetPosition ) const;
	
    //Construct a physical holograph at required location and in correct domain
    static MachPhysLandMine* pNewLandMine( MachLogRace* pRace, const MexPoint3d& location );
    //data members
	MachPhysLandMine*		pPhysLandMine_;
	int						damage_;
};

PER_DECLARE_PERSISTENT( MachLogLandMine );

#endif

/* End OREHOLO.HPP **************************************************/
