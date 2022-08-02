/*
 * S U B J E C T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dSubject

    Represents a logical view of a physical entity.
    Can be observed by W4dObservers, and broadcasts notifications of
    events to these observers.
    HAS-A W4dEntity physical representation
*/

#ifndef _WORLD4D_SUBJECT_HPP
#define _WORLD4D_SUBJECT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/world4d.hpp"

//forward refs
template< class T > class CtlCountedPtr;
template< class T > class CtlConstCountedPtr;
class W4dObserver;
class W4dDomain;
class W4dEntity;
class W4dSubjectImpl;
class MexPoint3d;
class MexTransform3d;

//Orthodox canonical (revoked)
class W4dSubject
{
public:
    //Must have a physical manifestation, pPhysObject. If this has no visualisation,
    //use a W4dEntity with no mesh.
    W4dSubject( W4dEntity* pPhysObject );
    //PRE( pPhysObject != NULL )
    //PRE( not pPhysObject->hasSubject() )

    virtual ~W4dSubject();
	
	///////////////////////////////
    //The global location of the origin
	MexPoint3d position() const;

    //The global position
	const MexTransform3d& globalTransform() const;
	
	///////////////////////////////
    //The physical manifestation
	CtlConstCountedPtr< W4dEntity > physObjectPtr() const;
    const W4dEntity& physObject() const;

	//Note that non-const version is in interface quite deliberately.
	//this allows mechanisms such as networking to manipulate the physical
	//representations of subjects directly.
	CtlCountedPtr< W4dEntity >& physObjectPtr();
    W4dEntity& physObject();

    //The domain in which this object currently resides
	W4dDomain* pCurrentDomain() const;

    //The id of the associated physical object
    W4dId id() const;

    //Set the id of the associated physical object
    void id( W4dId id );

	///////////////////////////////

	//maintains list of interested observers
	void attach( W4dObserver* pObserver );
	void detach( W4dObserver* pObserver );

	//Notify all interested observers of the event.
    //The clientData may have meaning at the client level, but only in the
    //case of event == CLIENT_SPECIFIC.
	enum NotificationEvent { DELETED, IN_DOMAIN, LEFT_DOMAIN, CLIENT_SPECIFIC };
	void notifyObservers( NotificationEvent event, int clientData = 0 );

	///////////////////////////////

    void CLASS_INVARIANT;

	//set a new object pointer and transform. This is used in the persistence mechanism
	//as the entity is not stored to the file - it is recreated.
	void setObjectPtr( W4dEntity*, const MexTransform3d& );

	W4dSubjectImpl& impl();
	const W4dSubjectImpl& impl() const;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dSubject );
    PER_FRIEND_READ_WRITE( W4dSubject );
    
protected:
    //Override to implement output for the derived class			
	virtual void doOutputOperator( ostream& ) const;

private:
    // Operations deliberately revoked
    W4dSubject( const W4dSubject& );
    W4dSubject& operator =( const W4dSubject& );
    bool operator ==( const W4dSubject& );

	W4dSubjectImpl* pImpl_;
};

ostream& operator <<( ostream& o, const W4dSubject& t );
PER_DECLARE_PERSISTENT( W4dSubject );

#endif

/* End SUBJECT.HPP **************************************************/
