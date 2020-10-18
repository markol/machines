/*
 * B U I L D P N T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBuildPoint

    Simple struct of boolean and MexPoint2d. 
	Knows whether or not it has a constructor who has "reserved" it.
*/

#ifndef _MACHLOG_BUILDPNT_HPP
#define _MACHLOG_BUILDPNT_HPP

#include "base/base.hpp"

//#include "phys/cspace2.hpp"

#include "machphys/machphys.hpp"

#include "mathex/point2d.hpp"

class MachLogBuildPoint
{
public:
    MachLogBuildPoint( const MexPoint2d& position );
	MachLogBuildPoint( void );		// to satisfy the requirements of ctl_vector - not intended for use
    ~MachLogBuildPoint( void );

	MexPoint2d position() const;
    bool isReserved() const;
	bool reserve( MachPhys::ConstructorSubType );
	MachPhys::ConstructorSubType subTypeOfReserver() const;
	// PRE( isReserved() );
	
	void unReserve();

    void CLASS_INVARIANT;	
	
	friend bool operator <( const MachLogBuildPoint& lhs, const MachLogBuildPoint& rhs );
	friend bool operator ==( const MachLogBuildPoint& lhs, const MachLogBuildPoint& rhs );

    friend ostream& operator <<( ostream& o, const MachLogBuildPoint& t );
	
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogBuildPoint );
	PER_FRIEND_READ_WRITE( MachLogBuildPoint );

private:
    MexPoint2d 						position_;
	bool							reserved_;
	MachPhys::ConstructorSubType	subTypeOfReserver_;
	
};

PER_DECLARE_PERSISTENT( MachLogBuildPoint );

#endif

/* End BUILDPNT.HPP *************************************************/
