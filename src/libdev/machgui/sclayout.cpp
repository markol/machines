/*
 * S C L A Y O U T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/sclayout.hpp"

// static
MachGuiScreenLayoutInfo& MachGuiScreenLayoutInfo::instance()
{
    static MachGuiScreenLayoutInfo instance_;
    return instance_;
}

MachGuiScreenLayoutInfo::MachGuiScreenLayoutInfo()
{

    TEST_INVARIANT;
}

MachGuiScreenLayoutInfo::~MachGuiScreenLayoutInfo()
{
    TEST_INVARIANT;

}

void MachGuiScreenLayoutInfo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiScreenLayoutInfo& t )
{

    o << "MachGuiScreenLayoutInfo " << (void*)&t << " start" << std::endl;
    o << "MachGuiScreenLayoutInfo " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SCLAYOUT.CPP *************************************************/
