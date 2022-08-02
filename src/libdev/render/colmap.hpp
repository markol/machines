/*
 * C O L M A P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    RenColourMap

    A brief description of the class should go in here
*/

#ifndef _RENDER_COLMAP_HPP
#define _RENDER_COLMAP_HPP

#include "base/base.hpp"

class RenColourMap
{
public:
    RenColourMap();
    ~RenColourMap();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenColourMap& t );

private:
    // Operation deliberately revoked
    RenColourMap( const RenColourMap& );

    // Operation deliberately revoked
    RenColourMap& operator =( const RenColourMap& );

    // Operation deliberately revoked
    bool operator ==( const RenColourMap& );

};


#endif

/* End COLMAP.HPP ***************************************************/
