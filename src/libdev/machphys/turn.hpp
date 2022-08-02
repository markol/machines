/*
 * T U R N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for physical objects that support an
 * turning animation.
 */

#ifndef _MACHPHYS_TURN_HPP
#define _MACHPHYS_TURN_HPP

#include "mathex/mathex.hpp"

#include "phys/phys.hpp"

//Forward declarations
class MexRadians;

/* //////////////////////////////////////////////////////////////// */

class MachPhysCanTurn
{
public:

	virtual ~MachPhysCanTurn() {};

    //Turn through angle radians about the z axis. Motion is relative,
    //not absolute.
    //Returns time it will take to complete.
	virtual PhysRelativeTime turn( const MexRadians& angle ) = 0;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHPHYS_TURN_HPP	*/

/* End TURN.HPP *****************************************************/
