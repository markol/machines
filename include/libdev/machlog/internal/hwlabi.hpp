/*
 * H W L A B I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogHardwareLabImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_HWLABI_HPP
#define _MACHLOG_HWLABI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/hwlab.hpp"

class MachLogHardwareLabImpl
// Canonical form revoked
{
public:
    MachLogHardwareLabImpl( MachPhys::HardwareLabSubType subType );
    ~MachLogHardwareLabImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogHardwareLabImpl );
	PER_FRIEND_READ_WRITE( MachLogHardwareLabImpl );


private:
    friend ostream& operator <<( ostream& o, const MachLogHardwareLabImpl& t );
	friend class MachLogHardwareLab;

    MachLogHardwareLabImpl( const MachLogHardwareLabImpl& );
    MachLogHardwareLabImpl& operator =( const MachLogHardwareLabImpl& );

	MachLogResearchTree::ResearchItems 	availableResearchItems_;
	MachLogResearchTree::ResearchItems 	currentResearchQueue_;
	PhysAbsoluteTime					lastUpdateTime_;
	MachPhys::HardwareLabSubType 		subType_;
};

PER_DECLARE_PERSISTENT( MachLogHardwareLabImpl );

#endif

/* End HWLABI.HPP ***************************************************/
