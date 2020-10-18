/*
 * L S G E X T R A . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/persist.hpp"
#include "machgui/lsgextra.hpp"
#include "machgui/ingame.hpp"
#include "machlog/races.hpp"

MachGuiLoadSaveGameExtras::MachGuiLoadSaveGameExtras( MachInGameScreen* pInGameScreen ) :
	pInGameScreen_( pInGameScreen )
{
    TEST_INVARIANT;
}

MachGuiLoadSaveGameExtras::~MachGuiLoadSaveGameExtras()
{
    TEST_INVARIANT;

}

void MachGuiLoadSaveGameExtras::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiLoadSaveGameExtras& t )
{

    o << "MachGuiLoadSaveGameExtras " << (void*)&t << " start" << std::endl;
    o << "MachGuiLoadSaveGameExtras " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiLoadSaveGameExtras::perWriteExtras( PerOstream& ostr )
{
	pInGameScreen_->saveGame( ostr );
}

void MachGuiLoadSaveGameExtras::perReadExtras( PerIstream& istr )
{
	pInGameScreen_->loadSavedGame( MachLogRaces::instance().currentPlanet(), istr );
}

/* End LSGEXTRA.CPP *************************************************/
