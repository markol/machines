/*
 * A N I M R E S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogResearchAnimation

    Players a research animation..typing keyboard etc
*/

#ifndef _MACHLOG_ANIMRES_HPP
#define _MACHLOG_ANIMRES_HPP

#include "base/base.hpp"
#include "machlog/animatio.hpp"

class MachLogTechnician;

class MachLogResearchAnimation
: public MachLogAnimation
// cannonical form revoked
{
public:
	
	MachLogResearchAnimation( MachLogTechnician * pActor );

	virtual ~MachLogResearchAnimation();
	
	///////////////////////////////
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogResearchAnimation );
	PER_FRIEND_READ_WRITE( MachLogResearchAnimation );

protected:
	
	virtual PhysRelativeTime doStartAnimation();
	
	virtual PhysRelativeTime doUpdate( );

	virtual void doOutputOperator( ostream& ) const;
	
private:

	MachLogResearchAnimation( const MachLogResearchAnimation& );
	MachLogResearchAnimation& operator =( const MachLogResearchAnimation& );
	bool operator ==( const MachLogResearchAnimation& ) const;

	MachLogTechnician*						pActor_;
};

PER_DECLARE_PERSISTENT( MachLogResearchAnimation );

#endif

/* End ANIMRES.HPP **************************************************/
