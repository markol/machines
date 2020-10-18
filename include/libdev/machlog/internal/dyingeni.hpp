/*
 * D Y I N G E N I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogDyingEntityEventImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_DYINGENI_HPP
#define _MACHLOG_DYINGENI_HPP

#include "base/base.hpp"
#include "machlog/dyingent.hpp"
#include "ctl/nbvector.hpp"

class MachLogDyingEntityEventImpl
// Canonical form revoked
{
public:
    MachLogDyingEntityEventImpl( void );
    ~MachLogDyingEntityEventImpl( void );

    void CLASS_INVARIANT;

private:
	friend class MachLogDyingEntityEvent;
    friend ostream& operator <<( ostream& o, const MachLogDyingEntityEventImpl& t );

    MachLogDyingEntityEventImpl( const MachLogDyingEntityEventImpl& );
    MachLogDyingEntityEventImpl& operator =( const MachLogDyingEntityEventImpl& );

	MachLogDyingEntityEvent::Polygons	polygons_;
	CtlConstCountedPtr< W4dEntity >		physObjectPtr_;
	MachLogConstruction*				pConstruction_;
	bool								insideBuilding_;

};


#endif

/* End DYINGENI.HPP *************************************************/
