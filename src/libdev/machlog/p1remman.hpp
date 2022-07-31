/*
 * P 1 R E M M A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogRemoteFirstPersonManager

    Acts as interface to first-person controlled machines running on remote nodes in a network game.
    Updates the handlers for these machines every frame.
*/

#ifndef _MACHLOG_P1REMMAN_HPP
#define _MACHLOG_P1REMMAN_HPP

#include "base/base.hpp"
#include "utility/id.hpp"
#include "world4d/observer.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MachLogRemoteFirstPersonManagerImpl;
class MachPhysFirstPersonStateVector;

class MachLogRemoteFirstPersonManager : public W4dObserver
// Canonical form revoked
{
public:
    MachLogRemoteFirstPersonManager();

    virtual ~MachLogRemoteFirstPersonManager();

    //Needs to be called per frame
    void update();

    //Remote actor with specified id has entered first person
    void actorOpen( UtlId actorId, MachPhys::Race race );

    //Remote actor with specified id has exited first person
    void actorClose( UtlId actorId, MachPhys::Race race );

    //All remote actors to exit first person
    void closeAll();

    void actorState( UtlId actorId, MachPhys::Race race, const MachPhysFirstPersonStateVector& state );

    /////////////////////////////////////////////
    //Inherited from W4dObserver

	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain );

    /////////////////////////////////////////////

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogRemoteFirstPersonManager& t );

    //revoked
    MachLogRemoteFirstPersonManager( const MachLogRemoteFirstPersonManager& );
    MachLogRemoteFirstPersonManager& operator =( const MachLogRemoteFirstPersonManager& );

    //data members
    MachLogRemoteFirstPersonManagerImpl* pImpl_; //data implementation object
};


#endif

/* End P1REMMAN.HPP *************************************************/
