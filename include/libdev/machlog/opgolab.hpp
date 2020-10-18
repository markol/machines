/*
 * O P G O L A B . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogGotoLabOperation

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_OPGOLAB_HPP
#define _MACHLOG_OPGOLAB_HPP

#include "base/base.hpp"
#include "machlog/operatio.hpp"

class MachLogTechnician;

// orthodox canonical (revoked)

class MachLogGotoLabOperation : public MachLogOperation
{
public:

    MachLogGotoLabOperation( MachLogTechnician* );

    ~MachLogGotoLabOperation( void );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogGotoLabOperation );
	PER_FRIEND_READ_WRITE( MachLogGotoLabOperation );

protected:


	virtual bool doStart();
	// PRE( not isFinished() );
	virtual void doFinish();
	// PRE( isFinished() );
	
	virtual bool doIsFinished() const;
		
	virtual void doOutputOperator( ostream& ) const;

	virtual PhysRelativeTime doUpdate( );

	virtual bool doBeInterrupted();
	///////////////////////////////
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogGotoLabOperation& t );

private:

    // Operations deliberately revoked
    MachLogGotoLabOperation( const MachLogGotoLabOperation& );
    MachLogGotoLabOperation& operator =( const MachLogGotoLabOperation& );
    bool operator ==( const MachLogGotoLabOperation& );

	MachLogTechnician*		pActor_;
	bool 				finished_;

};

PER_DECLARE_PERSISTENT( MachLogGotoLabOperation );

#endif

/* End OPGOLAB.HPP **************************************************/
