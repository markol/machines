/*
 * G A R R I S O I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGarrisonImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_GARRISOI_HPP
#define _MACHLOG_GARRISOI_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "machlog/garrison.hpp"

class MachLogGarrisonImpl
// Canonical form revoked
{
public:
    MachLogGarrisonImpl( void );
    ~MachLogGarrisonImpl( void );

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogGarrisonImpl );
	PER_FRIEND_READ_WRITE( MachLogGarrisonImpl );

private:
    friend ostream& operator <<( ostream& o, const MachLogGarrisonImpl& t );
	friend class MachLogGarrison;

    MachLogGarrisonImpl( const MachLogGarrisonImpl& );
    MachLogGarrisonImpl& operator =( const MachLogGarrisonImpl& );

	MachLogConstruction::Machines			machinesBeingHealed_;
	PhysAbsoluteTime						lastUpdateTime_;	
};

PER_DECLARE_PERSISTENT( MachLogGarrisonImpl );

#endif

/* End GARRISOI.HPP *************************************************/
