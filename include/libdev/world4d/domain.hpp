/*
 * D O M A I N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    W4dDomain

    Represents a bounded volume of space. A camera within the domain can
    only see out of it via a W4dPortal.
*/

#ifndef _REN_DOMAIN_HPP
#define _REN_DOMAIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/subjecfw.hpp"
#include "world4d/observfw.hpp"
#include "world4d/entity.hpp"

template <class T> class ctl_list;
class MexQuad3d;
class W4dEntityFilter;
class W4dPortal;
class W4dDomainImpl;

class W4dDomain : public W4dEntity
{
public:
    W4dDomain( W4dEntity* parent, const W4dTransform3d& localTransform );
    // Constructor attaches to parent, and stores copy of localTransform.

    ~W4dDomain( void );

    typedef ctl_list< W4dPortal* >  W4dPortals;

    const W4dPortals& portals() const;
    // Returns the list of portals which are associated with 'this'.

    bool intersects( const W4dEntity& entity ) const;
    // Returns true if an intersect relation exists between 'this' and entity

    const W4dEntities& intersectingEntities() const;
    // Exports a list of entities having an intersects relation with this domain

    W4dPortal* addPortal( W4dDomain* pPartner, const MexQuad3d& aperture,
      const W4dTransform3d& newLocalTransform );
    // Add a portal between this domain and pPartner domain. The portal
    // will be owned by this domain and take its global transform relative
    // to this domain.
    // PRE( pPartner != NULL );

    void removePortal( W4dPortal* pPortal );
    // Remove a portal that was attached to this domain. The portal itself
    // will be destroyed and it will be removed from the other domain it
    // was attached to.
    // PRE( pPortal != NULL );
    // PRE( hasPortal( pPortal ) );

    bool hasPortal( const W4dPortal* pPortal ) const;
    // Return true iff the given portal is attached to this domain.

    //The collection of subjects in the domain
    const W4dSubjects& subjects() const;

    //The collection of observers of the domain
    const W4dObservers& observers() const;

	//maintains list of interested observers. These observers will ne notified
    //when subjects enter or leave the domain.
	void attach( W4dObserver* pObserver );
	void detach( W4dObserver* pObserver );


    //True if there is an entity in the domain which intersects line.
    //If so, the entity's pointer is returned in ppEntity, and the distance
    //from line's start point to the intersection point is returned in pDistance.
    //Any entity with intersection point further than limit is ignored.
    //Any entity with check id already set to checkId is ignored, and entity's which
    //are checked have their check id set to checkId.
    //The accuracy of the intersection test is subjectively indicated by accuracy.
    //The entities to be checked can be filtered as defined by the filter argument,
    //using a bitwise combination of the EntityFilter enumeration.
    enum EntityFilter
    {
        DO_ALL = 0,
        EXCLUDE_SOLID = 0x0001,
        EXCLUDE_NOT_SOLID = 0x0002,
        EXCLUDE_STATIONARY = 0x0004,
        EXCLUDE_NOT_STATIONARY = 0x0008
    };

    bool findNearerEntity( const MexLine3d& line, MATHEX_SCALAR limit,
                           ulong checkId, W4dEntity::Accuracy accuracy,
                           W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
                           int filter = EXCLUDE_NOT_SOLID );

    //True if there is an entity in the domain which intersects line.
    //If so, the entity's pointer is returned in ppEntity, and the distance
    //from line's start point to the intersection point is returned in pDistance.
    //Any entity with intersection point further than limit is ignored.
    //Any entity with check id already set to checkId is ignored, and entity's which
    //are checked have their check id set to checkId.
    //The accuracy of the intersection test is subjectively indicated by accuracy.
    //The entities to be checked are filtered as defined by pEntityFilter.
    bool findNearerEntity( const MexLine3d& line, MATHEX_SCALAR limit,
                           ulong checkId, W4dEntity::Accuracy accuracy,
                           W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
                           W4dEntityFilter* pEntityFilter );


    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dDomain& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dDomain );
    PER_FRIEND_READ_WRITE( W4dDomain );

protected:
    virtual void doDebugOutput( ostream& ostr );
    // Write out the details of 'this' for debugging purposes.

private:
    //data members

    friend class W4dManager;
    friend class W4dPortal;
    friend class W4dSubjectImpl;

    //The static list of uninitialised subjects
    static W4dSubjects& newSubjects();

    //Maintains list of new W4dSubjects. These are subjects whose domain's have not
    //yet had their observers notified of the entry. Note that we cannot do this in
    //the W4dSubject ctor, in case the observer makes virtual function calls on the
    //subject.
    static void initialiseNewSubjects();
    static void addNewSubject( W4dSubject* pSubject );
    static void flushDeletedSubjectFromNewSubjects( W4dSubject* pSubject );

	friend void W4dEntity::addIntersectingDomain( W4dDomain* );
	friend void W4dEntity::deleteIntersectingDomains();
	friend void W4dEntity::eraseIntersectingDomains();
    friend void W4dEntity::intersects( W4dDomain*, bool );
    friend void W4dEntity::intersectingDomains(const W4dEntity::W4dDomains& newDoms);

    void addPortalToList( W4dPortal* pPortal );
    // Add a portal to this domain's list of portals. Don't attempt to create
    // the portal or do anything fancy, just get it onto the list.
    // PRE( pPortal != NULL );
    // This call only for use by W4dPortal::W4dPortal and W4dPortal::partner

    void removePortalFromList( W4dPortal* pPortal );
    // Remove a portal from this domain's list of portals. Don't attempt to destroy
    // the portal or do anything fancy, just get it off the list.
    // PRE( pPortal != NULL );
    // PRE( hasPortal( pPortal ) );
    // This call only for use by W4dPortal::~W4dPortal

    void intersects( W4dEntity* pEntity, bool intersects );
    // Establishes or removes an intersects relation with an Entity.
    // This call is only for use by W4dEntity::intersects

    // HAS-A list of moving objects that are subordinate to the domain in the tree.

    //Used by findNearerEntity to perform the same function recusrively for pEntity
    //and its descendants.
    bool recursiveFindNearerEntity
    (
        W4dEntity* pEntity,
        const MexLine3d& line, MATHEX_SCALAR limit,
        ulong checkId, W4dEntity::Accuracy accuracy,
        W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
        W4dEntityFilter* pEntityFilter
    );

    // Operation deliberately revoked
    W4dDomain( const W4dDomain& );

    // Operation deliberately revoked
    W4dDomain& operator =( const W4dDomain& );

    // Operation deliberately revoked
    bool operator ==( const W4dDomain& );

	W4dDomainImpl& domainImpl();
	const W4dDomainImpl& domainImpl() const;


	W4dDomainImpl *pImpl_;

};

PER_DECLARE_PERSISTENT( W4dDomain );

#endif

/* End DOMAIN.HPP ***************************************************/
