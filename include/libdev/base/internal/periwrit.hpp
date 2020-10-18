/*
 * P E R I W R I T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PersistenceImplementationWrite

    An internal class designed to hide the implementation details of
    the persistence write mechanism from the outside world.
*/

#ifndef _BASE_PERIWRIT_HPP
#define _BASE_PERIWRIT_HPP

#include "base/base.hpp"
#include "base/lessptr.hpp"
#include "base/private/pertypes.hpp"
#include "base/private/indent.hpp"
#include "base/internal/persist.hpp"
#include "base/private/persist.hpp"
#include "base/internal/perolog.hpp"
#include "ctl/map.hpp"
#include "ctl/set.hpp"
#include <cstring>

class PerOstream;

class PersistenceImplementationWrite
{
public:
    PersistenceImplementationWrite( void );
    ~PersistenceImplementationWrite( void );

    //  Get the identifier for a given pointer. If the pointer
    //  does not currently have an identifier assign it one
    //  and return that.
    PerIdentifier  identifier( const void* ptr );

    //  Log the fact that the object at this address has been written
    //  out explicitly (i.e. as an object)
    void    logWrittenAsObject( const char* classname, const void* ptr );

    //  Log the fact that the object at this address has been written
    //  out explicitly (i.e. via a pointer)
    void    logWrittenViaPointer( const void* ptr );

    //  Returns true if the object at ptr has already been written
    //  explicitly (i.e. as an object)
    bool    writtenAsObject( const char* classname, const void* ptr );

    //  Returns true if the object at ptr has already been written
    //  implicitly (i.e. via a pointer to the object)
    bool    writtenViaPointer( const void* ptr ) const;

    //  Clear all logging and identifier information.
    void    clear();

    //  Write a text string to the stream
    void	writeName( PerOstream& ostr, const char* name );

    //  Do all preparetory work necessary for writing out an object
    void writeObjectPre( PerOstream& ostr, const void* pOb, const char* className );
    //  Do all cleanup work necessary after writing out an object
    void writeObjectPost( const void* pOb, const char* className );

    //  Do all preparetory work necessary for writing out a pointer to an object
    Persistence::PointerWriteResult writePointerPre(
        PerOstream& ostr, const void* pOb, const char* className,
        const void* pMostDerivedOb, const char* mostDerivedClassName );
    //  Do all cleanup work necessary after writing out a pointer to an object
    void writePointerPost( PerOstream& ostr,
        const void* pOb, const char* className, const void* pMostDerivedOb );

	void registerDerivedClass( const char* className, PerWriteFnPtr );

    void writeRawPointer( PerOstream& ostr, const void* ptr );

    void writeAsRaw( bool );

    void writeRawData( PerOstream& ostr, const char* ptr, size_t nBytes );

    ostream& debugStream();

    void logAddresses( bool );

    void    registerOpenOstream();
    void    registerCloseOstream();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PersistenceImplementationWrite& t );

private:
    // Operation deliberately revoked
    PersistenceImplementationWrite( const PersistenceImplementationWrite& );

    // Operation deliberately revoked
    PersistenceImplementationWrite& operator =( const PersistenceImplementationWrite& );

    // Operation deliberately revoked
    bool operator ==( const PersistenceImplementationWrite& );

    void write( PerOstream& ostr, PerDataType type );
    void writeId( PerOstream& ostr, PerIdentifier id );

    void writeNamedObject( PerOstream& istr, const char* className, const void* ptr );

    void startWrite( void );
    void finishWrite( void );

    bool writeAsRaw() const;
    bool logAddresses() const;

    void writeAddress( const void* ptr ) const;

	typedef	ctl_map< PerMapPtrType, PerIdentifier, std::less< PerMapPtrType > >	PointerToIdMap;
    typedef ctl_set< const void*, less_ptr< const void > >     Pointers;
    typedef ctl_set< PersistenceObjectLog, std::less< PersistenceObjectLog > >     Objects;

	typedef	ctl_map< std::string, PerWriteFnPtr, std::less< std::string > >	        WriteFnMap;

	size_t	            outputCount_;
    BaseIndentOstream   indentStream_;

    PointerToIdMap  pointerToId_;

    Objects         writtenAsObject_;
    Pointers        writtenViaPointer_;

    PerIdentifier   currentIdentifier_;

    WriteFnMap      writeFnMap_;

    uint            writeAsRawCount_;
    bool            logAddresses_;

    size_t          nOpenOstreams_;
};



#endif

/* End PERIWRIT.HPP *************************************************/
