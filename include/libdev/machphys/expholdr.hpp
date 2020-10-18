/*
 * E X P H O L D R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysGenericExplosionData

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_EXPHOLDR_HPP
#define _MACHPHYS_EXPHOLDR_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"

class MachPhysConstructionExplosionData;

class MachPhysGenericExplosionData
{
public:
    ~MachPhysGenericExplosionData( void );
	static const CtlConstCountedPtr< MachPhysConstructionExplosionData >& genericExplosion( size_t level );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysGenericExplosionData& t );

private:
    MachPhysGenericExplosionData( void );
    // Operation deliberately revoked
    MachPhysGenericExplosionData( const MachPhysGenericExplosionData& );

    // Operation deliberately revoked
    MachPhysGenericExplosionData& operator =( const MachPhysGenericExplosionData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysGenericExplosionData& );

};


#endif

/* End EXPHOLDR.HPP *************************************************/
