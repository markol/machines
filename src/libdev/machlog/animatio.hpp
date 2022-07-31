/*
 * A N I M A T I O . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
 
#ifndef _MACH_ANIMATIO_HPP
#define _MACH_ANIMATIO_HPP

#include "mathex/radians.hpp"

#include "machphys/machphys.hpp"

#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogAnimation
: public MachLogOperation
// cannonical form revoked
{
public:
	
	MachLogAnimation();

	virtual ~MachLogAnimation();
	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogAnimation );
	PER_FRIEND_READ_WRITE( MachLogAnimation );

protected:

	virtual PhysRelativeTime doStartAnimation() = 0;

	virtual bool doStart();

	// redefinition optional, invocation optional
	virtual PhysRelativeTime doUpdate( );

	virtual void doFinish();

	// inherited from MachIrreducibleOperation	
	// redefinition optional, invocation optional
	virtual bool doBeInterrupted();

	virtual bool doIsFinished() const;
		
	virtual void doOutputOperator( ostream& ) const = 0;

private:

	MachLogAnimation( const MachLogAnimation& );
	MachLogAnimation& operator =( const MachLogAnimation& );
	bool operator ==( const MachLogAnimation& ) const;

	PhysAbsoluteTime	endTime_;
};

PER_DECLARE_PERSISTENT( MachLogAnimation );

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogCanTurn;

class MachLogTurnAnimation
: public MachLogAnimation
// cannonical form revoked
{
public:
	
	MachLogTurnAnimation( MachLogCanTurn * pActor, MexRadians angle );

	virtual ~MachLogTurnAnimation();
	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTurnAnimation );
	PER_FRIEND_READ_WRITE( MachLogTurnAnimation );

protected:

	virtual PhysRelativeTime doStartAnimation();
	virtual void doOutputOperator( ostream& ) const;

private:

	MachLogTurnAnimation( const MachLogTurnAnimation& );
	MachLogTurnAnimation& operator =( const MachLogTurnAnimation& );
	bool operator ==( const MachLogTurnAnimation& ) const;

	MachLogCanTurn *	pActor_;
	MexRadians			turnBy_;
};

PER_DECLARE_PERSISTENT( MachLogTurnAnimation );


/* //////////////////////////////////////////////////////////////// */

/*
class MachLogConstructor;
class MachLogConstruction;

class MachLogConstructAnimation
: public MachLogAnimation
// cannonical form revoked
{
public:
	
	MachLogConstructAnimation( MachLogConstructor * pActor, 
							MachLogConstruction *pConstr, 
							MachPhys::BuildingMaterialUnits units );

	virtual ~MachLogConstructAnimation();
	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogConstructAnimation );
	PER_FRIEND_READ_WRITE( MachLogConstructAnimation );

protected:
	
	virtual PhysRelativeTime doStartAnimation();
	
	virtual PhysRelativeTime doUpdate( );

	virtual void doOutputOperator( ostream& ) const;
	
private:

	MachLogConstructAnimation( const MachLogConstructAnimation& );
	MachLogConstructAnimation& operator =( const MachLogConstructAnimation& );
	bool operator ==( const MachLogConstructAnimation& ) const;

	MachLogConstructor *						pActor_;
	MachLogConstruction *						pConstruction_;
	MachPhys::BuildingMaterialUnits				units_;
	PhysAbsoluteTime							lastUpdateTime_;
};

PER_DECLARE_PERSISTENT( MachLogConstructAnimation );
*/

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachActor;

class MachLogBeDestroyedAnimation
: public MachLogAnimation
// cannonical form revoked
{
public:
	
	MachLogBeDestroyedAnimation( MachActor * pActor );

	virtual ~MachLogBeDestroyedAnimation();
	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBeDestroyedAnimation );
	PER_FRIEND_READ_WRITE( MachLogBeDestroyedAnimation );

protected:
	
	virtual PhysRelativeTime doStartAnimation();
	virtual void doFinish();
	
	virtual void doOutputOperator( ostream& ) const;

private:

	MachLogBeDestroyedAnimation( const MachLogBeDestroyedAnimation& );
	MachLogBeDestroyedAnimation& operator =( const MachLogBeDestroyedAnimation& );
	bool operator ==( const MachLogBeDestroyedAnimation& ) const;

	MachActor *	pActor_;
};

PER_DECLARE_PERSISTENT( MachLogBeDestroyedAnimation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACH_ANIMATIO_HPP	*/

/* End ANIMATIO.HPP *************************************************/