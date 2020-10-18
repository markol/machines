/*
 * G X G E N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXGen

   This class does nothing... It is just a good friend of the
   GXFile class so that all GXGen sons (GXMesh...) can access
   GXFile private methods.
*/

#ifndef _GXGEN_HPP
#define _GXGEN_HPP

#include "base/base.hpp"

class GXGen
// Canonical form revoked
{
public:
    GXGen( void );
    ~GXGen( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXGen& t );

private:
    GXGen( const GXGen& );
    GXGen& operator =( const GXGen& );
    bool operator ==( const GXGen& );

};


#endif

/* End GXGEN.HPP ****************************************************/
