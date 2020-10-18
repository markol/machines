/*
 * V M M A N I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/vmmani.hpp"
			  
MachLogVoiceMailManagerImpl::MachLogVoiceMailManagerImpl()
{

    TEST_INVARIANT;
}

MachLogVoiceMailManagerImpl::~MachLogVoiceMailManagerImpl()
{
    TEST_INVARIANT;

}

void MachLogVoiceMailManagerImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogVoiceMailManagerImpl& t )
{

    o << "MachLogVoiceMailManagerImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogVoiceMailManagerImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End VMMANI.CPP ***************************************************/
