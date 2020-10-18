/*
 * I S T R R E P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/istrrep.hpp"

PerIstreamReporter::PerIstreamReporter()
{

    TEST_INVARIANT;
}

PerIstreamReporter::~PerIstreamReporter()
{
    TEST_INVARIANT;

}

void PerIstreamReporter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PerIstreamReporter& t )
{

    o << "PerIstreamReporter " << (void*)&t << " start" << std::endl;
    o << "PerIstreamReporter " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ISTRREP.CPP **************************************************/
