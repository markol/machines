/*
 * C O M M A N D I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCommandImpl

    Implementation class for MachGuiCommand
*/

#ifndef _MACHGUI_COMMANDI_HPP
#define _MACHGUI_COMMANDI_HPP

#include "base/base.hpp"
#include "machlog/machop.hpp"

class MachInGameScreen;

class MachGuiCommandImpl
// Canonical form revoked
{
public:
    MachGuiCommandImpl( MachInGameScreen* );
    ~MachGuiCommandImpl();

    void add( const MachLogMachineOperation& );

    void clearMachineOperations();
    const MachLogMachineOperations& machineOperations() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiCommandImpl& t );
    friend class MachGuiCommand;

    MachGuiCommandImpl( const MachGuiCommandImpl& );
    MachGuiCommandImpl& operator =( const MachGuiCommandImpl& );

    MachLogMachineOperations  machineOperations_;
    
    MachInGameScreen* pInGameScreen_; //The in-game screen
};


#endif

/* End COMMANDI.HPP *************************************************/
