/*
 * M A P P E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMapper

    General utility functions for mapping between sets of values
*/

#ifndef _MACHLOG_MAPPER_HPP
#define _MACHLOG_MAPPER_HPP

#include "base/base.hpp"

#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"

class MachLogMapper
// Canonical form revoked
{
public:
    static  MachPhys::MachineType mapToPhysMachine( MachLog::ObjectType type );
    //  PRE( isMachine( type ) );
    static  MachPhys::ConstructionType mapToPhysConstruction( MachLog::ObjectType );
    //  PRE( isConstruction( type ) );

    static  bool    isMachine( MachLog::ObjectType );
    static  bool    isConstruction( MachLog::ObjectType );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogMapper& t );

    MachLogMapper();
    ~MachLogMapper();

    MachLogMapper( const MachLogMapper& );
    MachLogMapper& operator =( const MachLogMapper& );

};


#endif

/* End MAPPER.HPP ***************************************************/
