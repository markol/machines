/*
 * L S G E X T R A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLoadSaveGameExtras

	This class is designed to be overridden to enable higher libraries than MachLog to save things and load things
*/

#ifndef _MACHLOG_LSGEXTRA_HPP
#define _MACHLOG_LSGEXTRA_HPP

#include "base/base.hpp"
//#include "base/persist.hpp"

class PerIstream;
class PerOstream;

class MachLogLoadSaveGameExtras
// Canonical form revoked
{
public:
    MachLogLoadSaveGameExtras();
    virtual ~MachLogLoadSaveGameExtras() = 0;
	virtual void perReadExtras( PerIstream& ) = 0;
	virtual void perWriteExtras( PerOstream& ) = 0;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogLoadSaveGameExtras& t );

    MachLogLoadSaveGameExtras( const MachLogLoadSaveGameExtras& );
    MachLogLoadSaveGameExtras& operator =( const MachLogLoadSaveGameExtras& );

};

#endif

/* End LSGEXTRA.HPP *************************************************/
