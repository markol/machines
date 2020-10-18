/*
 * S Y M P A R S E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */


/*
    Symparse

    A collection of static parsing functions for MachPhys objects
*/

#ifndef _SYMPARSE_HPP
#define _SYMPARSE_HPP

#include "machphys/machphys.hpp"

#include "stdlib/string.hpp"

class MachPhysSymbolParser
{
public:
    MachPhysSymbolParser( ){};
    ~MachPhysSymbolParser( void );

    static MachPhys::ConstructionType 			constructionType( const string& token );
    static MachPhys::AggressorSubType      	aggressorSubType( const string& );
    static MachPhys::AdministratorSubType  	administratorSubType( const string& );
    static MachPhys::ConstructorSubType    	constructorSubType( const string& );
    static MachPhys::TechnicianSubType     	technicianSubType( const string& );
	static MachPhys::FactorySubType            factorySubType( const string& );
    static MachPhys::HardwareLabSubType        hardwareLabSubType( const string& );
    static MachPhys::MissileEmplacementSubType missileEmplacementSubType( const string& );
	static int									voidSubType( const string& token );
		
	static int constructionSubType( MachPhys::ConstructionType type,  const string& subType );
	
	static size_t  parseConstructionLevel( const string& token );

private:
    // Operation deliberately revoked
    MachPhysSymbolParser( const MachPhysSymbolParser& );

    // Operation deliberately revoked
    MachPhysSymbolParser& operator =( const MachPhysSymbolParser& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSymbolParser& );

};


#endif

/* End SYMPARSE.HPP *************************************************/
