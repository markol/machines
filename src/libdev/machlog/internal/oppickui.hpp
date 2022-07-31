/*
 * O P P I C K U I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPickUpOperationImpl

    Implementation class for MachLogPickUpOperation.

*/

#ifndef _MACHLOG_OPPICKUI_HPP
#define _MACHLOG_OPPICKUI_HPP

#include "machlog/oppickup.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "machlog/actor.hpp"

// forward declarations

class MachLogResourceCarrier;
class MachActor;

class MachLogPickUpOperationImpl
{
public:		
	MachLogPickUpOperationImpl( MachLogResourceCarrier* );
							
	~MachLogPickUpOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogPickUpOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogPickUpOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogPickUpOperation;
	
private:

	MachLogResourceCarrier *				pActor_;
	bool									finished_;	
	MexPoint2d								destinationPadPosition_;	
};

PER_DECLARE_PERSISTENT( MachLogPickUpOperationImpl );

#endif

/* End OPPICKUI.HPP ***************************************************/
