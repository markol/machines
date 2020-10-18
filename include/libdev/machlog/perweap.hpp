/*
 * P E R W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogWeaponPersistence

    This class is not in itself (for now) persistent - it simply handles aspects to do with weapons
	such as registering derived classes.
*/

#ifndef _MACHLOG_PERWEAP_HPP
#define _MACHLOG_PERWEAP_HPP

#include "base/base.hpp"

class MachLogWeaponPersistence
// Canonical form revoked
{
public:
    ~MachLogWeaponPersistence( void );

	static void registerDerivedClasses();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogWeaponPersistence& t );

    MachLogWeaponPersistence( void );
    MachLogWeaponPersistence( const MachLogWeaponPersistence& );
    MachLogWeaponPersistence& operator =( const MachLogWeaponPersistence& );

};


#endif

/* End PERWEAP.HPP **************************************************/
