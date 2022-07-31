/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPersistence

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_PERSIST_HPP
#define _MACHLOG_PERSIST_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"

class MachLogMachinePersistence;
class MachLogConstructionPersistence;
class MachLogOtherPersistence;
class W4dSceneManager;
class MachLogController;
class MachLogLoadSaveGameExtras;

class MachLogPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachLogPersistence& instance();
    ~MachLogPersistence();

	const MachLogMachinePersistence& machines() const;
	MachLogMachinePersistence& machines();

	const MachLogConstructionPersistence& constructions() const;
	MachLogConstructionPersistence& constructions();

	const MachLogOtherPersistence& others() const;
	MachLogOtherPersistence& others();

	//the scene manager must be set prior to loading a game. This is asserted in the debug version
	void sceneManager( W4dSceneManager* );
	//if you want extras then this pointer must be passed through from races.
	void extras( MachLogLoadSaveGameExtras* );

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogPersistence );	
	PER_FRIEND_READ_WRITE( MachLogPersistence );

private:
	//this method is used to populate various bits of data from external sources
	//just before the write is done.
	void setDataForWrite() const;
	void registerDerivedClasses();

    friend ostream& operator <<( ostream& o, const MachLogPersistence& t );

    MachLogPersistence( const MachLogPersistence& );
    MachLogPersistence& operator =( const MachLogPersistence& );

    MachLogPersistence();
	W4dSceneManager*										pSceneManager_;
	ctl_pvector< MachLogController >						controllers_;
	MachLogLoadSaveGameExtras*								pExtras_;
};

PER_DECLARE_PERSISTENT( MachLogPersistence );


#endif

/* End PERSIST.HPP **************************************************/
