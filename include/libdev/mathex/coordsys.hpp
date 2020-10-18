/*
 * C O O R D S Y S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _MATHEX_COORDSYS_HPP
#define _MATHEX_COORDSYS_HPP

#include "base/base.hpp"

// Represents the sense of the 3D co-ordinate system.  It can only be set once 
// (you can't change your mind mid-execution).  It is used by the render and 
// world4d libraries for orienting models and cameras.  It's also used by the 
// phys library for orienting motion controls.  Typically these libs have pre-
// conditions which require that the co-ordinate system is set before any of
// the dependent classes are created.
class MexCoordSystem
{
public:
    //  Singleton class
    static MexCoordSystem& instance();

	enum Type { LEFT_HANDED, RIGHT_HANDED };

	// PRE(!isSet()); POST(isSet());
	void set(Type);
	operator const Type&() const;
	bool isSet() const;
	
	// Checks for an environment variable called CB_COORD_SYS.  If it is 
	// present and set to "right", it will pick a right-handed system.
	// Otherwise, it chooses a left-handed system (the default if the 
	// varible is not set).
	// POST(isSet());
	void setFromEnvironment();

private:
	bool set_;
	Type t_;

    MexCoordSystem( const MexCoordSystem& );
    MexCoordSystem& operator =( const MexCoordSystem& );
    MexCoordSystem() {}
};

ostream& operator <<( ostream& o, const MexCoordSystem& t );

#ifdef _INLINE
    #include "mathex/coordsys.ipp"
#endif

#endif

/* End COORDSYS.HPP *************************************************/
