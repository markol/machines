/*
 * P E R M A C H . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachinePersistence

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_PERMACH_HPP
#define _MACHLOG_PERMACH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"

class MachLogMachine;

class MachLogMachinePersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachLogMachinePersistence& instance( void );
    ~MachLogMachinePersistence( void );

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogMachinePersistence );
	PER_FRIEND_READ_WRITE( MachLogMachinePersistence );

private:

	void registerDerivedClasses();
	//This const method is really non-const as it will set up the machines data member
	//takes MLRaces views and converts to local vector for persistence
	void setUpLocalVectorFromViews() const;
	//takes local vecotr that has been populated from perRead and sets up other thingies with
	//pointers to actors.
	void setUpViewsFromLocalVector();

    friend ostream& operator <<( ostream& o, const MachLogMachinePersistence& t );

    MachLogMachinePersistence( const MachLogMachinePersistence& );
    MachLogMachinePersistence& operator =( const MachLogMachinePersistence& );

    MachLogMachinePersistence( void );
	typedef ctl_pvector< MachLogMachine >	Machines;
	Machines								machines_;
};

PER_DECLARE_PERSISTENT( MachLogMachinePersistence );

#endif

/* End PERMACH.HPP **************************************************/
