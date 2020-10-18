/*
 * T E X T D I S P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/textdisp.hpp"

/*
MachGuiDisplayText::MachGuiDisplayText()
{

    TEST_INVARIANT;
}
*/
MachGuiDisplayText::~MachGuiDisplayText()
{
    TEST_INVARIANT;

}

void MachGuiDisplayText::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDisplayText& t )
{

    o << "MachGuiDisplayText " << (void*)&t << " start" << std::endl;
    o << "MachGuiDisplayText " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TEXTDISP.CPP *************************************************/
