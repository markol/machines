/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysPersistence

    A brief description of the class should go in here
*/

#ifndef _PHYS_PERSIST_HPP
#define _PHYS_PERSIST_HPP

#include "base/base.hpp"

class PhysPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static PhysPersistence& instance();
    ~PhysPersistence();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysPersistence& t );

private:
    PhysPersistence( const PhysPersistence& );
    PhysPersistence& operator =( const PhysPersistence& );
    bool operator ==( const PhysPersistence& );

    PhysPersistence();
};


#endif

/* End PERSIST.HPP **************************************************/
