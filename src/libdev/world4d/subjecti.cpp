/*
 * S U B J E C T I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "world4d/internal/subjecti.hpp"
#include "world4d/subject.hpp"
#include "world4d/entity.hpp"
#include "world4d/domain.hpp"
#include "world4d/observer.hpp"
#include "ctl/algorith.hpp"

W4dSubjectImpl::W4dSubjectImpl(W4dSubject* pSubject, W4dEntity* pPhysObject):
	physObjectPtr_( pPhysObject ),
	pSubject_(pSubject)
{
    PRE(pSubject);
    PRE(pPhysObject == NULL  or  not pPhysObject->hasSubject());

    //Choose sensible collection size
    observers_.reserve( 4 );

    if( pPhysObject )
    {
        //Set the cross pointer in the physical rep
        pPhysObject->subject(pSubject_);
        pPhysObject->nowOwnedByCountedPtr();

        //Log the new subject in the domain mechanism, so we can notify the domain's
        //observers on next W4dManager::update() call.
        W4dDomain::addNewSubject(pSubject_);
    }
}

W4dSubjectImpl::~W4dSubjectImpl()
{
    PRE( observers_.size() == 0 );

	//the subject will expire before the entity
	//remove the subject from the entity.
    if( physObjectPtr_.isDefined() )
	    physObjectPtr_->subject( NULL );

    //Flush from the list of new subjects in case it's still in there
    W4dDomain::flushDeletedSubjectFromNewSubjects( pSubject_ );

    //Notify all observers it's being deleted
    //Too late to do it here!! Subclass already destructed. Notification now
    //sent from Sim library.
    //notifyObservers( DELETED );
}

void W4dSubjectImpl::initialiseDomainObservers()
{
    //Get the owning domain
    const W4dEntity& physObject = *physObjectPtr_;
    W4dDomain* pDomain = physObject.containingDomain();

    //Get the list of observers and notify them all
    const W4dObservers& domainObservers = pDomain->observers();
    for( W4dObservers::const_iterator cit = domainObservers.begin();
         cit != domainObservers.end(); ++cit )
    {
        //If required, add the observer to the subject's list of direct observers
        W4dObserver* pObserver = *cit;
        if( pObserver->beNotified( pSubject_, W4dSubject::IN_DOMAIN ) )
            attach( pObserver );
    }
}

void W4dSubjectImpl::updateDomainObservers( const W4dEntity& oldParent )
{
//	OBSERVER_STREAM("(" << id() << ") W4dSubject::updateDomainObservers " << std::endl );

    //Get the new and old domains and check for a change
    W4dDomain* pOldDomain = (oldParent.isDomain() ?
                             ((W4dEntity&)oldParent).getAsDomain() :
                             oldParent.containingDomain() );
    W4dDomain* pNewDomain = physObjectPtr_->containingDomain();

    if( pOldDomain != pNewDomain )
    {
        //Construct a list of the direct observers of this subject
        W4dObservers allObservers;
        allObservers.reserve( 32 );
        allObservers = observers_;

        //Add any extra observers of the new domain
        const W4dObservers& newDomainObservers = pNewDomain->observers();
        for( W4dObservers::const_iterator cit = newDomainObservers.begin();
             cit != newDomainObservers.end(); ++cit )
        {
            W4dObserver* pObserver = *cit;
            if( find( observers_.begin(), observers_.end(), pObserver ) == observers_.end() )
                allObservers.push_back( pObserver );
        }

        //Get the observers of the old domain. Any not in the list so far are
        //informed of the leaving domain event.
        const W4dObservers& oldDomainObservers = pOldDomain->observers();
        for( W4dObservers::const_iterator cit = oldDomainObservers.begin();
             cit != oldDomainObservers.end(); ++cit )
        {
            W4dObserver* pObserver = *cit;
            if( find( allObservers.begin(), allObservers.end(), pObserver ) == allObservers.end() )
			{
//				OBSERVER_STREAM(" notify observer " << (void*)pObserver << " LEFT_DOMAIN\n" );
                pObserver->beNotified( pSubject_, W4dSubject::LEFT_DOMAIN );
			}
        }

        //Inform the union of direct observers and new domain observers of the
        //entry event. Adjust the observer list according to return values.
        for( W4dObservers::iterator it = allObservers.begin();
             it != allObservers.end(); ++it )
        {
            W4dObserver* pObserver = *it;
//			OBSERVER_STREAM(" notify observer " << (void*)pObserver << " IN_DOMAIN\n" );
            if( pObserver->beNotified( pSubject_, W4dSubject::IN_DOMAIN ) )
                attach( pObserver );
            else
                detach( pObserver );
        }
    }

//	OBSERVER_STREAM("W4dSubject::updateDomainObservers DONE\n" );
}

void W4dSubjectImpl::attach( W4dObserver* pObserver )
{

//very useful for trapping observer related problems
//	OBSERVER_STREAM("(" << id() << ") W4dSubject::attach " << (void*)pObserver << std::endl );
//  OBSERVER_STREAM( ProStackTracer() );

    PRE( pObserver != NULL );
    TEST_INVARIANT;

    //Check to see if the observer is not already an observer before adding it
    W4dObservers::iterator it =
        find( observers_.begin(), observers_.end(), pObserver );

    if( it == observers_.end() )
    {
        observers_.push_back( pObserver );
    }
}

void W4dSubjectImpl::detach( W4dObserver* pObserver )
{
//very useful for trapping observer related problems
//	OBSERVER_STREAM("(" << id() << ") W4dSubject::detach " << (void*)pObserver << std::endl );
//  OBSERVER_STREAM( ProStackTracer() );

    PRE( pObserver != NULL );
    TEST_INVARIANT;

    //Find the argument in the collection, then erase it
    W4dObservers::iterator it =
        find( observers_.begin(), observers_.end(), pObserver );

    if( it != observers_.end() )
    {
        observers_.erase( it );
    }
}

void W4dSubjectImpl::notifyObservers( NotificationEvent event, int clientData )
{

//very useful for trapping observer related problems
//	OBSERVER_STREAM("(" << id() << ") W4dSubject::notifyObservers number " << observers_.size() << std::endl );
//  OBSERVER_STREAM( ProStackTracer() );

    //Notify each observer
    size_t i = observers_.size();
    while( i-- != 0 )
    {
        //Notify this observer, and erase if no longer required.
        //Store the size of the collection so we can detect naughty detaches that occurred during the beNotified call
//		OBSERVER_STREAM(" w4dsubject is derefing observer no. " << i );
        W4dObserver* pObserver = observers_[i];
//		OBSERVER_STREAM(" " << (void*)pObserver << std::endl );

        size_t beforeObserverCount = observers_.size();
        bool eraseCurrent = (not pObserver->beNotified( pSubject_, event, clientData )) or event == W4dSubject::DELETED;
        bool sizeChanged = observers_.size() != beforeObserverCount;

        if( eraseCurrent )
        {
            //Remove current observer. If the size of the collection hasn't changed, we know where it is.
            //Otherwise we should use detach which checks for existence before deleting.
            if( sizeChanged )
                detach( pObserver );
            else
                observers_.erase( observers_.begin() + i ); //May or may not change observer count again
        }

        //If the size has changed, ensure our index isn't off the end.
        if( sizeChanged )
        {
            size_t nObserversLeft = observers_.size();
            if( i > nObserversLeft )
                i = nObserversLeft;
        }
    }
}

void W4dSubjectImpl::setObjectPtr( W4dEntity* pNewEntity, const MexTransform3d& newGlobalTransform )
{
    PRE( pNewEntity != NULL );
	physObjectPtr_	= pNewEntity;

	//Set the cross pointer in the physical rep
    pNewEntity->subject( pSubject_ );
    pNewEntity->nowOwnedByCountedPtr();

    //Position it
	physObjectPtr_->globalTransform( newGlobalTransform );

    //Log the new subject in the domain mechanism, so we can notify the domain's
    //observers on next W4dManager::update() call.
    W4dDomain::addNewSubject( pSubject_ );
}

ostream& W4dSubjectImpl::write(ostream& o) const
{
    o << "W4dSubject "  << (void*) pSubject_ << std::endl;
    o << "  W4dEntity " << (void*)(&(*physObjectPtr_)) << std::endl << "  Observers" << std::endl;

    W4dObservers::const_iterator cit = observers_.begin();
    while( cit != observers_.end() )
        o << "    " << (void*)(*cit) << std::endl;

    return o;
}

void W4dSubjectImpl::CLASS_INVARIANT
{
    INVARIANT(this);
    INVARIANT(pSubject_);
}

/* End SUBJECTI.CPP *************************************************/
