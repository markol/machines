/*
 * H W L A B I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPodImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_PODI_HPP
#define _MACHLOG_PODI_HPP

#include "base/base.hpp"
#include "machlog/pod.hpp"

class MachLogPodImpl
// Canonical form revoked
{
public:
    MachLogPodImpl();
    ~MachLogPodImpl( void );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogPodImpl );
	PER_FRIEND_READ_WRITE( MachLogPodImpl );


private:
    friend ostream& operator <<( ostream& o, const MachLogPodImpl& t );
	friend class MachLogPod;

	// operations deliberately revoked
    MachLogPodImpl( const MachLogPodImpl& );
    MachLogPodImpl& operator =( const MachLogPodImpl& );
	bool operator ==( const MachLogPodImpl& );

	
	// data members
	
	PhysAbsoluteTime		lastUpdateTime_;
	bool					hasIonCannon_;
	bool					ionCannonCharged_;
	bool					addedBMUStorageToRace_;

};

PER_DECLARE_PERSISTENT( MachLogPodImpl );

#endif

/* End PODI.HPP ***************************************************/
