/*
 * E N T Y F I L T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/entyfilt.hpp"
#include "world4d/domain.hpp"

W4dEntityFilter::W4dEntityFilter()
:   finished_( false )
{
    TEST_INVARIANT;
}

W4dEntityFilter::~W4dEntityFilter()
{
    TEST_INVARIANT;

}

void W4dEntityFilter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void W4dEntityFilter::finish()
{
    finished_ = true;
}

bool W4dEntityFilter::isFinished() const
{
    return finished_;
}

W4dStandardFilter::W4dStandardFilter( int filterFlags )
:   filterFlags_( filterFlags )
{
}

//virtual
W4dStandardFilter::~W4dStandardFilter()
{
}

//virtual
bool W4dStandardFilter::check( const W4dEntity& entity, TreeOption* pOption )
{
    //Always continue processing
    *pOption = PROCESS_SUBTREE;

    //Process the filter options
    bool isExcluded = filterFlags_ != W4dDomain::DO_ALL;

    if( isExcluded )
    {
        isExcluded = false;
        bool isSolid = entity.solid() == W4dEntity::SOLID;

        if( isSolid )
        {
            if( filterFlags_ & W4dDomain::EXCLUDE_SOLID )
                isExcluded = true;
        }
        else if( filterFlags_ & W4dDomain::EXCLUDE_NOT_SOLID )
            isExcluded = true;

        bool isStationary = entity.stationary();

        if( isStationary )
        {
            if( filterFlags_ & W4dDomain::EXCLUDE_STATIONARY )
                isExcluded = true;
        }
        else if( filterFlags_ & W4dDomain::EXCLUDE_NOT_STATIONARY )
            isExcluded = true;
    }

    return not isExcluded;
}
/* End ENTYFILT.CPP *************************************************/
