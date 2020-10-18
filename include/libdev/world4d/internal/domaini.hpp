/*
 * D O M A I N I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dDomainImpl

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_DOMAINI_HPP
#define _WORLD4D_DOMAINI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "world4d/domain.hpp"
#include "world4d/portal.hpp"
#include "world4d/subjecfw.hpp"
#include "world4d/observfw.hpp"

class W4dPortal;

class W4dDomainImpl
// Canonical form revoked
{
public:
    W4dDomainImpl( void );
    ~W4dDomainImpl( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( W4dDomainImpl );
    PER_FRIEND_READ_WRITE( W4dDomainImpl );

private:
//    friend void W4dPortal::partner( W4dDomain* );
//    friend W4dPortal::W4dPortal(W4dDomain*, W4dDomain*, const MexQuad3d&, const W4dTransform3d&);
//    friend W4dPortal::~W4dPortal( void );
    friend class W4dDomain;
            
    friend ostream& operator <<( ostream& o, const W4dDomainImpl& t );

    W4dDomainImpl( const W4dDomainImpl& );
    W4dDomainImpl& operator =( const W4dDomainImpl& );

    W4dDomain::W4dPortals		portals_;
    W4dEntity::W4dEntities		intersectingEntities_;
	W4dObservers 				observers_; //Observers interested in the domain's subjects
	W4dSubjects 				subjects_; //logical objects in the domain

};

PER_DECLARE_PERSISTENT( W4dDomainImpl );

#endif

/* End DOMAINI.HPP **************************************************/
