/*
 * P E R I R E A D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PersistenceImplementationRead

    An internal class designed to hide the implementation details of
    the persistence read mechanism from the outside world.
*/

#ifndef _BASE_PERIREAD_HPP
#define _BASE_PERIREAD_HPP

#include "base/base.hpp"
#include "base/private/pertypes.hpp"
#include "base/private/indent.hpp"
#include "base/internal/persist.hpp"
#include "ctl/vector.hpp"
#include "ctl/map.hpp"
#include <unordered_map>
#include <string>
#include <functional>

class BasePoolAllocator;

class PersistenceImplementationRead
{
public:
    PersistenceImplementationRead();
    ~PersistenceImplementationRead();

    typedef std::pair< PerIdentifier, uint >  PerReadIdentifier;

    //  Register an object's address and its associated ID
    void registerObject( const void* ptr, PerReadIdentifier id );

    //  If the given id has already been registered fixup the given
    //  pointer with the correct address, otherwise add the point to
    //  the list of unfixed pointers for later fixup.
    void fixupPointer( void** pPtr, PerReadIdentifier id );

    //  Fixup any pointers on the list of outstanding pointers
    void fixupOutstandingPointers();

    //  Clear out all of the internal tables used for pointer fixup.
    void    clear();

    //  Read in an object whose name has been written to the stream.
    //  This means that the object is a derived class object that was
    //  written out using a base class pointer. The appropriate function
    //  to use for the read is given by the functions stored in the read
    //  function map.
    void	readNamedObject( PerIstream& istr );

    //  Register a derived class that might be read in via a base class pointer
	void    registerDerivedClass( const char* className, PerReadFnPtr );

    //  Do all preparetory work necessary for reading in an object
    void readObjectPre( PerIstream& istr, const void* pOb, const char* className );
    //  Do all cleanup work necessary after reading in an object
    void readObjectPost( const void* pOb, const char* className );

    //  Do all preparetory work necessary for reading in a pointer
    Persistence::PointerReadResult readPointerPre( PerIstream& istr, void** ptr, const char* className );
    //  Do all cleanup work necessary after reading in a pointer
    void readPointerPost( PerIstream& istr, void** ptr, const char* className );

    void readPointerAbstract( PerIstream&, void** ptr, const char* className );

    void startRead();
    void finishRead();

    void readRawData( PerIstream& istr, char* ptr, size_t nBytes );

    void readAsRaw( bool );

    void CLASS_INVARIANT;

    ostream& debugStream();

    friend ostream& operator <<( ostream& o, const PersistenceImplementationRead& t );

    void readRawPointer( PerIstream& ostr, void** ptr, const char* className );
    void logAddresses( bool );

    void    registerOpenIstream();
    void    registerCloseIstream();

private:
    // Operation deliberately revoked
    PersistenceImplementationRead( const PersistenceImplementationRead& );

    // Operation deliberately revoked
    PersistenceImplementationRead& operator =( const PersistenceImplementationRead& );

    // Operation deliberately revoked
    bool operator ==( const PersistenceImplementationRead& );

    std::string	readName( PerIstream& istr );
    bool    pointerIdValid( const void* ptr, PerReadIdentifier id );

    PerDataType     readDataType( PerIstream& istr );
    PerIdentifier   readId( PerIstream& istr );

    bool readAsRaw() const;
    bool logAddresses() const;

    void writeAddress( const void* ptr ) const;

    struct PairHash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ h2;
        }
    };
	typedef	std::unordered_map< PerReadIdentifier, PerMapPtrType, PairHash >	ReadIdToPointerMap;
    typedef	std::unordered_multimap< PerIdentifier, PerMapPtrType >	IdToPointerMap;
	typedef	std::unordered_map< std::string, PerReadFnPtr >	ReadFnMap;

    typedef std::pair< void**, PerReadIdentifier >  UnfixedPointer;

    typedef ctl_vector< UnfixedPointer >    UnfixedPointers;

	std::hash<std::string> strHasher_;
	size_t	            inputCount_;
    BaseIndentOstream   indentStream_;

    IdToPointerMap      idToPointer_;
    ReadIdToPointerMap  readIdToPointer_;
    UnfixedPointers     unfixedPointers_;

    ReadFnMap           readFnMap_;

    uint                readAsRawCount_;
    bool                logAddresses_;

    size_t              nOpenIstreams_;
    BasePoolAllocator* pIdMapAllocator_; //used for the id map so we can reclaim all the space used
};


#endif

/* End PERIREAD.HPP *************************************************/
