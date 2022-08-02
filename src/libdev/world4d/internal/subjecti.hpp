/*
 * S U B J E C T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSubjectImpl

    The pimple implementation of W4dSubject.  This isn't a straightforward
	pimple because it supports methods for internal clients which aren't in
	the public interface of W4dSubject.
*/

#ifndef _WORLD4D_SUBJECTI_HPP
#define _WORLD4D_SUBJECTI_HPP

#include "base/base.hpp"
#include "world4d/observfw.hpp"
#include "world4d/subject.hpp"
#include "ctl/pvector.hpp"
#include "ctl/countptr.hpp"

class W4dEntity;
class W4dSubject;
class MexTransform3d;

class W4dSubjectImpl
{
public:
    W4dSubjectImpl(W4dSubject* pSubject, W4dEntity* pPhysObject);
    ~W4dSubjectImpl();

    //Called when parent has changed to update domain observers if necessary
    void updateDomainObservers( const W4dEntity& oldParent ); 

    //Inform owning domain's observers after construction.
    //This has to be done by the framework via W4dManager::update(), because
    //we can't notify observers during W4dSubject construction. (Otherwise
    //virtual methods are likely to be called).
    void initialiseDomainObservers();

	void attach( W4dObserver* pObserver );
	void detach( W4dObserver* pObserver );

	//Notify all interested observers of the event.
    //The clientData may have meaning at the client level, but only in the
    //case of event == CLIENT_SPECIFIC.
	typedef W4dSubject::NotificationEvent NotificationEvent;
	void notifyObservers( NotificationEvent event, int clientData = 0 );

	void setObjectPtr( W4dEntity* pNewEntity, const MexTransform3d& newGlobalTransform );

	ostream& write(ostream& o) const;

    void CLASS_INVARIANT;

private:
	// W4dSubject has direct access to the member variables.
	friend class W4dSubject;
    CtlCountedPtr<W4dEntity>	physObjectPtr_;		// Physical rep
	W4dObservers				observers_;			// Collection of observers of this
	W4dSubject*					pSubject_;			// back-ptr to non-pimpled class

    W4dSubjectImpl( const W4dSubjectImpl& );
    W4dSubjectImpl& operator =( const W4dSubjectImpl& );
};

#endif

/* End SUBJECTI.HPP *************************************************/
