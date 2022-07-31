/*
 * O P L O C A T E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogLocateOperation

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_OPLOCATE_HPP
#define _MACHLOG_OPLOCATE_HPP

#include "base/base.hpp"

#include "ctl/list.hpp"

#include "mathex/point3d.hpp"
#include "machlog/operatio.hpp"



class MachLogGeoLocator;

// orthodox canonical (revoked)

class MachLogLocateOperation
: public MachLogOperation
{
public:

	typedef ctl_list< MexPoint2d > Path;
	MachLogLocateOperation( MachLogGeoLocator * pActor, const MexPoint3d& dest );
	MachLogLocateOperation( MachLogGeoLocator * pActor, const Path& externalPath );

	~MachLogLocateOperation( );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLocateOperation );
	PER_FRIEND_READ_WRITE( MachLogLocateOperation );

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
    MachLogLocateOperation( const MachLogLocateOperation& );
    MachLogLocateOperation& operator =( const MachLogLocateOperation& );
    bool operator ==( const MachLogLocateOperation& );
	
	void buildPathFromExternalPath();

	MachLogGeoLocator *	pActor_;
	MexPoint3d			dest_;
	Path				path_;
	Path				externalPath_;
	bool				hasExternalPath_;
	int					currentElement_;
};

PER_DECLARE_PERSISTENT( MachLogLocateOperation );


#endif

/* End OPLOCATE.HPP *************************************************/
