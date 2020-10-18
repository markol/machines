/*
 * C O N S T R E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/constrei.hpp"

MachLogConstructionTreeImpl::MachLogConstructionTreeImpl()
{

    TEST_INVARIANT;
}

MachLogConstructionTreeImpl::~MachLogConstructionTreeImpl()
{
    TEST_INVARIANT;

}

void MachLogConstructionTreeImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogConstructionTreeImpl& t )
{

    o << "MachLogConstructionTreeImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogConstructionTreeImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CONSTREI.CPP *************************************************/
