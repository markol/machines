/*
 * L S G E X T R A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiLoadSaveGameExtras

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_LSGEXTRA_HPP
#define _MACHGUI_LSGEXTRA_HPP

#include "base/base.hpp"
#include "machlog/lsgextra.hpp"

class MachInGameScreen;

class MachGuiLoadSaveGameExtras : public MachLogLoadSaveGameExtras
// Canonical form revoked
{
public:
    MachGuiLoadSaveGameExtras( MachInGameScreen* pInGameScreen );
    virtual ~MachGuiLoadSaveGameExtras();
	virtual void perReadExtras( PerIstream& );
	virtual void perWriteExtras( PerOstream& );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiLoadSaveGameExtras& t );

    MachGuiLoadSaveGameExtras( const MachGuiLoadSaveGameExtras& );
    MachGuiLoadSaveGameExtras& operator =( const MachGuiLoadSaveGameExtras& );

	// Data members...
	MachInGameScreen* pInGameScreen_;
};

#endif

/* End LSGEXTRA.HPP *************************************************/
