/*
 * G R P M V I N F . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/grpmvinf.hpp"

#ifndef _INLINE
    #include "machlog/grpmvinf.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogGroupMoveInfo );


void MachLogGroupMoveInfo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGroupMoveInfo& t )
{
    if( t.valid() )
    {
        o << "offset " << t.offset() << " radius " << t.groupRadius();
    }
    else
    {
        o << "not valid";
    }

    return o;
}

void perWrite( PerOstream& ostr, const MachLogGroupMoveInfo& info )
{
    ostr << info.valid_;
    ostr << info.offset_;
    ostr << info.groupRadius_;
}

void perRead( PerIstream& istr, MachLogGroupMoveInfo& info )
{
    istr >> info.valid_;
    istr >> info.offset_;
    istr >> info.groupRadius_;
}

/* End GRPMVINF.CPP *************************************************/
