/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dPersistence

    Initialise the world4d persistence mechanism
*/

#ifndef _WORLD4D_PERSIST_HPP
#define _WORLD4D_PERSIST_HPP

#include "base/base.hpp"

class W4dPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static W4dPersistence& instance( void );
    ~W4dPersistence( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dPersistence& t );

private:
    W4dPersistence( const W4dPersistence& );
    W4dPersistence& operator =( const W4dPersistence& );
    bool operator ==( const W4dPersistence& );

    W4dPersistence( void );
};


#endif

/* End PERSIST.HPP **************************************************/
