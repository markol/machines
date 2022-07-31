/*
 * O P L M I N E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogDropLandMineOperation

    operation for spy locators which says go and drop a land mine here.
*/

#ifndef _MACHLOG_OPDROPLANDMINE_HPP
#define _MACHLOG_OPDROPLANDMINE_HPP

#include "base/base.hpp"

#include "machlog/operatio.hpp"

// forward declarations
class MachLogDropLandMineOperationImpl;
class MachLogSpyLocator;
class MexPoint2d;
template< class T > class ctl_list;

// orthodox canonical (revoked)

class MachLogDropLandMineOperation
: public MachLogOperation
{
public:

	typedef ctl_list< MexPoint2d > Path;
	MachLogDropLandMineOperation( MachLogSpyLocator * pActor, const MexPoint3d& dest );
	MachLogDropLandMineOperation( MachLogSpyLocator * pActor, const Path& externalPath );

	~MachLogDropLandMineOperation( );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogDropLandMineOperation );
	PER_FRIEND_READ_WRITE( MachLogDropLandMineOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual bool doBeInterrupted();
	
	virtual void doOutputOperator( ostream& ) const;

	// redefinition required, invocation revoked	
	virtual PhysRelativeTime doUpdate( );

private:

	// Operations deliberately revoked
    MachLogDropLandMineOperation( const MachLogDropLandMineOperation& );
    MachLogDropLandMineOperation& operator =( const MachLogDropLandMineOperation& );
    bool operator ==( const MachLogDropLandMineOperation& );
	
	void buildPathFromExternalPath();

	MachLogDropLandMineOperationImpl*	pImpl_;	
};

PER_DECLARE_PERSISTENT( MachLogDropLandMineOperation );


#endif

/* End OPDropLandMine.HPP *************************************************/
