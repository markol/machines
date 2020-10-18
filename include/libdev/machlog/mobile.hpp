/*
 * M O B I L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_MOBILE_HPP
#define _MACHLOG_MOBILE_HPP

#include "machlog/actor.hpp"
#include "machlog/canmove.hpp"

/* //////////////////////////////////////////////////////////////// */

// #include "machphys/machine.hpp"

// forward refs
class MachPhysMachine;
class MachLogMachineMotionSequencer;

typedef MachPhysMachine MachPhysMobile;

class MachLogMobile
: public MachActor,
  public MachLogCanMove,
  public MachLogCanTurn
// cannonical form revoked
{
public:

	MachLogMobile( MachLogRace * pRace, 
					MachPhysMobile *,
					MachLog::ObjectType,
					MATHEX_SCALAR highClearence, 
					MATHEX_SCALAR lowClearence );

	MachLogMobile( MachLogRace * pRace, 
					MachPhysMobile *,
					MachLog::ObjectType,
					UtlId withId,
					MATHEX_SCALAR highClearence, 
					MATHEX_SCALAR lowClearence );

	virtual ~MachLogMobile();
	
	///////////////////////////////	///////////////////////////////

	// inherited from SimActor...
    // virtual PhysTime update( MATHEX_SCALAR clearanceFromDisplayedVolume ) = 0;

    // Must be overriden to return the W4dDomain in which the actor currently resides
	// virtual const W4dDomain& renDomain( void ) const = 0;

	///////////////////////////////

	virtual PhysRelativeTime beDestroyed();

    //Ensure the physical model doesnt get deleted for the specified time.
    //This is done by adding a dying entity event.
    void preservePhysicalModel( const PhysRelativeTime& forTime );

    //Drop debris model corresponding to this object, starting at specified time.
    //(The time functionality isn't implemented yet - always drops at current time). 
    void dropDebris( const PhysAbsoluteTime& atTime );

	MachPhysMobile& physMobile();
	const MachPhysMobile& physMobile() const;
		
	virtual PhysRelativeTime turn( const MexRadians& );
	PhysRelativeTime turnWithoutEcho( const MexRadians& );
	//virtual PhysRelativeTime move( const MexPoint3d& );
		
	///////////////////////////////

    //Take necessary action if following another entity, to review the destination
    void checkLeaderDestination();

	void createNewMachineMotionSequencer( MachPhysMobile* );
	//has the motion sequencer been created for this mobile yet?
	bool machineMotionSequencerDefined() const;
	
	bool mobileIsGlider() const;

    typedef uint32  ObstacleFlags;
    
    //  Return flags corresponding to obstacles that this mobile object can
    //  ignore (i.e. gliders can move over water and some low obstacles)
    ObstacleFlags   obstacleFlags() const;
    
	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogMobile );
	PER_FRIEND_READ_WRITE( MachLogMobile );

protected:
	
	virtual void doEndExplodingAnimation();
public:
	MachLogMachineMotionSequencer& motionSeq();
	const MachLogMachineMotionSequencer& motionSeq() const;

private:

	MachLogMobile( const MachLogMobile& );
	MachLogMobile& operator =( const MachLogMobile& );
	bool operator ==( const MachLogMobile& ) const;
	
	///////////////////////////////

	MachLogMachineMotionSequencer *	pMotionSeq_;
	friend class MachMoveToOperation;
};

PER_DECLARE_PERSISTENT( MachLogMobile );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_MOBILE_HPP	*/

/* End MOBILE.HPP ***************************************************/

