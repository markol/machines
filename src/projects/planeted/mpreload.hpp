/*
 * M P R E L O A D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPreload

    Preloads all machine objects then deletes them so things like wheels will
	work i.e. animate.
*/

#ifndef _MPRELOAD_HPP
#define _MPRELOAD_HPP

#include "base/base.hpp"

class W4dSceneManager;

class MachPreload
{
public:
    
    MachPreload( W4dSceneManager * pManager );

    ~MachPreload();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPreload& t );

private:
    // Operation deliberately revoked
    MachPreload( const MachPreload& );

    // Operation deliberately revoked
    MachPreload& operator =( const MachPreload& );

    // Operation deliberately revoked
    bool operator ==( const MachPreload& );

};


#endif

/* End MPRELOAD.HPP *************************************************/
