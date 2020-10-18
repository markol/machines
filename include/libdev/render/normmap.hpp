/*
 * N O R M M A P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    RenNormalMap

    A brief description of the class should go in here
*/

#ifndef _RENDER_NORMMAP_HPP
#define _RENDER_NORMMAP_HPP

#include "base/base.hpp"

class RenNormalMap
{
public:
    RenNormalMap( void );
    ~RenNormalMap( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenNormalMap& t );

private:
    // Operation deliberately revoked
    RenNormalMap( const RenNormalMap& );

    // Operation deliberately revoked
    RenNormalMap& operator =( const RenNormalMap& );

    // Operation deliberately revoked
    bool operator ==( const RenNormalMap& );

};


#endif

/* End NORMMAP.HPP **************************************************/
