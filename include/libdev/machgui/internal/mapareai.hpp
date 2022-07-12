/*
 * M A P A R E A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiMapAreaImpl

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MAPAREAI_HPP
#define _MACHGUI_MAPAREAI_HPP

#include "base/base.hpp"

class MachGuiMapAreaMagic;

class MachGuiMapAreaImpl
// Canonical form revoked
{
public:
    MachGuiMapAreaImpl();
    ~MachGuiMapAreaImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiMapAreaImpl& t );

    MachGuiMapAreaImpl( const MachGuiMapAreaImpl& );
    MachGuiMapAreaImpl& operator =( const MachGuiMapAreaImpl& );

	friend class MachGuiMapArea;

	// Data members
	MachGuiMapAreaMagic* pMapAreaMagic_;
};


#endif

/* End MAPAREAI.HPP *************************************************/
