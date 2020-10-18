/*
 * D O M A I N I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
#include "world4d/subject.hpp"
#include "world4d/observer.hpp"
#include "world4d/internal/domaini.hpp"
#include "world4d/entyfilt.hpp"

PER_DEFINE_PERSISTENT( W4dDomainImpl );

W4dDomainImpl::W4dDomainImpl()
{

    TEST_INVARIANT;
}

W4dDomainImpl::~W4dDomainImpl()
{
    TEST_INVARIANT;

}
void W4dDomainImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dDomainImpl& t )
{

    o << "W4dDomainImpl " << (void*)&t << " start" << std::endl;
    o << "W4dDomainImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dDomainImpl& t )
{
    ostr << t.portals_;

    //Not persisted
    //intersectingEntities_
    //observers_
    //subjects_
}

void perRead( PerIstream& istr, W4dDomainImpl& t )
{
    istr >> t.portals_;
}

W4dDomainImpl::W4dDomainImpl( PerConstructor )
{
}

/* End DOMAINI.CPP **************************************************/
