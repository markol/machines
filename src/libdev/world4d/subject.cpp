/*
 * S U B J E C T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/subject.hpp"
#include "world4d/entity.hpp"
#include "world4d/internal/subjecti.hpp"

#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

#define TEST_INVARIANTS

PER_DEFINE_PERSISTENT( W4dSubject );

W4dSubject::W4dSubject( W4dEntity* pPhysObject ):
	pImpl_(_NEW(W4dSubjectImpl(this, pPhysObject)))
{
    PRE( pPhysObject != NULL );

    TEST_INVARIANT;
}

W4dSubject::~W4dSubject()
{
    TEST_INVARIANT;
	_DELETE(pImpl_);
}

void W4dSubject::CLASS_INVARIANT
{
    INVARIANT(this);
    INVARIANT(pImpl_);

//    pImpl_->physObjectPtr_.isDefined();
}

ostream& operator <<( ostream& o, const W4dSubject& t )
{
	return t.impl().write(o);
}

MexPoint3d W4dSubject::position() const
{
    TEST_INVARIANT;
    return pImpl_->physObjectPtr_->globalTransform().position();
}

const MexTransform3d& W4dSubject::globalTransform() const
{
    TEST_INVARIANT;
    return pImpl_->physObjectPtr_->globalTransform();
}

W4dDomain* W4dSubject::pCurrentDomain() const
{
    TEST_INVARIANT;
    return pImpl_->physObjectPtr_->containingDomain();
}

void W4dSubject::attach( W4dObserver* pObserver )
{
	pImpl_->attach(pObserver);
}

void W4dSubject::detach( W4dObserver* pObserver )
{
	pImpl_->detach(pObserver);
}

void W4dSubject::notifyObservers( NotificationEvent event, int clientData )
{
	pImpl_->notifyObservers(event, clientData);
}

//virtual
void W4dSubject::doOutputOperator( ostream& ) const
{
    //Default implementation does nothing
}

W4dId W4dSubject::id() const
{
    return pImpl_->physObjectPtr_->id();
}

void W4dSubject::id( W4dId id )
{
//	OBSERVER_STREAM("W4dSubject::id( " << id << ") with entity " << (void*)&entity << endl );
    pImpl_->physObjectPtr_->id( id );
}

void perWrite(PerOstream&, const W4dSubject&)
{
	// Persistence is definitely *not* desired for the physical bits to which
	// the subject refers.
	// ostr << subject.physObjectPtr_;
	// ostr << subject.observers_;
}

void perRead(PerIstream&, W4dSubject&)
{
	// Persistence is definitely *not* desired for the physical bits to which
	// the subject refers.
	// istr >> subject.physObjectPtr_;
	// istr >> subject.observers_;
}

W4dSubject::W4dSubject( PerConstructor )
: pImpl_(_NEW(W4dSubjectImpl(this, NULL)))
{
}

void W4dSubject::setObjectPtr( W4dEntity* pNewEntity, const MexTransform3d& newGlobalTransform )
{
    TEST_INVARIANT;
	pImpl_->setObjectPtr(pNewEntity, newGlobalTransform);
}

CtlConstCountedPtr< W4dEntity > W4dSubject::physObjectPtr() const
{
    TEST_INVARIANT;
    return pImpl_->physObjectPtr_;
}

CtlCountedPtr< W4dEntity >& W4dSubject::physObjectPtr()
{
    TEST_INVARIANT;
    return pImpl_->physObjectPtr_;
}

W4dEntity& W4dSubject::physObject()
{
    TEST_INVARIANT;
    PRE( pImpl_ != 0 );
    PRE( &(*(pImpl_->physObjectPtr_) ) != 0 );

    return *(pImpl_->physObjectPtr_);
}

const W4dEntity& W4dSubject::physObject() const
{
    TEST_INVARIANT;
    return *(pImpl_->physObjectPtr_);
}

W4dSubjectImpl& W4dSubject::impl()
{
    TEST_INVARIANT;
	return *pImpl_;
}

const W4dSubjectImpl& W4dSubject::impl() const
{
    TEST_INVARIANT;
	return *pImpl_;
}

/* End SUBJECT.CPP **************************************************/
