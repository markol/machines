/*
 * C O M M A N D I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/commandi.hpp"

MachGuiCommandImpl::MachGuiCommandImpl( MachInGameScreen* pInGameScreen )
: pInGameScreen_( pInGameScreen )
{

    TEST_INVARIANT;
}

MachGuiCommandImpl::~MachGuiCommandImpl()
{
    TEST_INVARIANT;

}

void MachGuiCommandImpl::add( const MachLogMachineOperation& machineOperation )
{
    machineOperations_.push_back( machineOperation );
}

void MachGuiCommandImpl::clearMachineOperations()
{
    machineOperations_.erase( 
      machineOperations_.begin(), 
      machineOperations_.end() );
}

const MachLogMachineOperations& MachGuiCommandImpl::machineOperations() const
{
    return machineOperations_;
}

void MachGuiCommandImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCommandImpl& t )
{

    o << "MachGuiCommandImpl " << (void*)&t << " start" << std::endl;
    o << "MachGuiCommandImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End COMMANDI.CPP *************************************************/
