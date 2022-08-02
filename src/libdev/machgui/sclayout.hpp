/*
 * S C L A Y O U T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiScreenLayoutInfo

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SCLAYOUT_HPP
#define _MACHGUI_SCLAYOUT_HPP

#include "base/base.hpp"

class MachGuiScreenLayoutInfo
// Canonical form revoked
{
public:
    //  Singleton class
    static MachGuiScreenLayoutInfo& instance();
    ~MachGuiScreenLayoutInfo();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiScreenLayoutInfo& t );

private:
    MachGuiScreenLayoutInfo( const MachGuiScreenLayoutInfo& );
    MachGuiScreenLayoutInfo& operator =( const MachGuiScreenLayoutInfo& );
    bool operator ==( const MachGuiScreenLayoutInfo& );

    MachGuiScreenLayoutInfo();
};


#endif

/* End SCLAYOUT.HPP *************************************************/
