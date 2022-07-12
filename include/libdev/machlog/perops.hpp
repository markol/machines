/*
 * P E R W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogOperationPersistence

    This class is not in itself (for now) persistent - it simply handles aspects to do with Operations
	such as registering derived classes.
*/

#ifndef _MACHLOG_PEROPS_HPP
#define _MACHLOG_PEROPS_HPP

#include "base/base.hpp"

class MachLogOperationPersistence
// Canonical form revoked
{
public:
    ~MachLogOperationPersistence();

	static void registerDerivedClasses();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogOperationPersistence& t );

    MachLogOperationPersistence();
    MachLogOperationPersistence( const MachLogOperationPersistence& );
    MachLogOperationPersistence& operator =( const MachLogOperationPersistence& );

};


#endif

/* End PERWEAP.HPP **************************************************/
