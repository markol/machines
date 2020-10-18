/*
 * O B S E R V E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dObserver

    ABC/mixin class representing an observer of a W4dDomain and/or a W4dSubject.
*/

#ifndef _WORLD4D_OBSERVER_HPP
#define _WORLD4D_OBSERVER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/subject.hpp"

//bitwise canonical
class W4dObserver
{
public:
	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 ) = 0;

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain ) = 0;

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( W4dObserver );
};

PER_READ_WRITE( W4dObserver );
PER_DECLARE_PERSISTENT( W4dObserver );

#ifdef _INLINE
    #include "world4d/observer.ipp"
#endif


#endif

/* End OBSERVER.HPP *************************************************/
