/*
 * C A N M O V E . H P P 
 * (c) Charybdis Limited, 1997/1998. All Rights Reserved
 */
 
/*
 * Mixin for logical objects that support 
 * a translational operation.
 */

#ifndef _MACHLOG_CANMOVE_HPP
#define _MACHLOG_CANMOVE_HPP

#include "phys/phys.hpp"

/* //////////////////////////////////////////////////////////////// */

class MexRadians;

class MachLogCanTurn
{
public:

	MachLogCanTurn() {};
	virtual ~MachLogCanTurn() {};

    // Turn through angle radians about the z axis. 
    // Motion is relative, not absolute.
    // Returns time it will take to complete.
	virtual PhysRelativeTime turn( const MexRadians& angle ) = 0;
	PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( MachLogCanTurn );
};

PER_READ_WRITE( MachLogCanTurn );
PER_DECLARE_PERSISTENT( MachLogCanTurn );

/* //////////////////////////////////////////////////////////////// */

class MachLogCanMove
{
public:

	MachLogCanMove() {};
	virtual ~MachLogCanMove() {};

    //Move to point, returning the time it will take to arrive
    //virtual PhysRelativeTime move( const MexPoint3d& point ) = 0;
	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( MachLogCanMove );
};

PER_READ_WRITE( MachLogCanMove );
PER_DECLARE_PERSISTENT( MachLogCanMove );

/* //////////////////////////////////////////////////////////////// */

#endif	//	#ifndef _MACHLOG_CANMOVE_HPP

/* End CANMOVE.HPP *****************************************************/

