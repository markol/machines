/*
 * R E G I S T R Y . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysRegistry

    Allows access to the windows registry.
*/

#ifndef _SYSTEM_REGISTRY_HPP
#define _SYSTEM_REGISTRY_HPP

#include "base/base.hpp"
//#include "stdlib/strfwd.hpp"

class SysRegistryImpl;

class SysRegistry
// Canonical form revoked
{
public:
    //  Singleton class
    static SysRegistry& instance();
    ~SysRegistry();

	//complex interface for accessing the registry properly but with no windows dependancies
	enum ReturnValue { SUCCESS, FAILED };
	enum DataType { BINARY, INTEGER, MULTI_STRINGS, STRING };
	enum Root { LOCAL_MACHINE, CURRENT_USER };
	typedef uint KeyHandle;

	//Most systems will have a default stub for example machines is:
	//"SOFTWARE\Acclaim Entertainment\Machines"
	//So rather than always fully specifying the key the stub maybe set in one place
	const std::string& currentStubKey() const;
	void currentStubKey( const std::string& );

	//Opens a key value but only if it is present in registry
	ReturnValue	onlyOpenKey( const std::string& keyName, KeyHandle* pOpenedKey, Root root = LOCAL_MACHINE );
	//This is the same as above but will create the key if necessary - this is normal use
	ReturnValue	openKey( const std::string& keyName, KeyHandle* pOpenedKey, Root root = LOCAL_MACHINE );
	ReturnValue	deleteKey( const std::string& keyName, Root root = LOCAL_MACHINE );
	ReturnValue	queryValue( KeyHandle, const std::string& valueName, DataType, void* pBuffer, int* pBufferSize );
	ReturnValue	setValue( KeyHandle, const std::string& valueName, const std::string& value);
	ReturnValue deleteValue( KeyHandle, const std::string& valueName );
	ReturnValue	closeKey( KeyHandle );

	//more simple interface but less efficient as it will open and close keys and each call
	std::string queryStringValue( const std::string& keyName, const std::string& valueName, Root root = LOCAL_MACHINE );
	int queryIntegerValue( const std::string& keyName, const std::string& valueName, Root root = LOCAL_MACHINE );
	//The set functions will create the key if it isn't present
	void setStringValue( const std::string& keyName, const std::string& valueName, const std::string& value, Root root = LOCAL_MACHINE );
	void setIntegerValue( const std::string& keyName, const std::string& valueName, int value, Root root = LOCAL_MACHINE );

    ReturnValue	queryValueNoRecord( KeyHandle, const std::string& valueName, std::string& );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const SysRegistry& t );
    friend ostream& operator <<( ostream& o, const SysRegistry::Root& t );

    SysRegistry( const SysRegistry& );
    SysRegistry& operator =( const SysRegistry& );

    //  The NoRecord versions of these functions do not do any recording. They
    //  are for use within functions which are already doing their own recording
    //  - e.g. queryStringValue and queryIntegerValue.
	ReturnValue	onlyOpenKeyNoRecord( const std::string& keyName, KeyHandle* pOpenedKey, Root root = LOCAL_MACHINE );
    // Query is now public. F the recorder. =)
	ReturnValue	closeKeyNoRecord( KeyHandle );

    SysRegistry();
	SysRegistryImpl*		pImpl_;
};


#endif

/* End REGISTRY.HPP *************************************************/
