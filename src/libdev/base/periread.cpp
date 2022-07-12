/*
 * P E R I R E A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/periread.hpp"
#include "base/poolallo.hpp"
#include "base/cballoc.hpp"
#include <iomanip>
#include <algorithm>

#include "stdlib/string.hpp"

//  Comment out this line to avoid doing the timing

// #define TIME_OBJECTS

#ifdef TIME_OBJECTS

    static  void    startTiming( const string& className );
    static  void    finishTiming( const string& className );

    #define START_TIMING_OBJECT( className )        startTiming( className );
    #define FINISH_TIMING_OBJECT( className )       finishTiming( className );
#else
    #define START_TIMING_OBJECT( className )
    #define FINISH_TIMING_OBJECT( className )
#endif

PersistenceImplementationRead::PersistenceImplementationRead()
: inputCount_( 0 ),
  indentStream_( Diag::instance().perReadStream(), "" ),
  readAsRawCount_( 0 ),
  logAddresses_( false ),
  nOpenIstreams_( 0 ),
  pIdMapAllocator_( NULL )
{
    unfixedPointers_.reserve( 512 );

    TEST_INVARIANT;
}

PersistenceImplementationRead::~PersistenceImplementationRead()
{
    TEST_INVARIANT;

}

void PersistenceImplementationRead::registerObject( const void* ptr, PerReadIdentifier idPair )
{

    PRE( pointerIdValid( ptr, idPair ) );

    pushCurrentPoolAllocator( pIdMapAllocator_ );
    //idToPointer_.insert( id, _STATIC_CAST( PerMapPtrType, ptr ) );
    readIdToPointer_.emplace( idPair, _REINTERPRET_CAST( PerMapPtrType, ptr ) );
    idToPointer_.emplace( std::make_pair(idPair.first, _REINTERPRET_CAST( PerMapPtrType, ptr ) ) );
    popCurrentPoolAllocator();
}

//  Checks that if a pointer has been registered with this id before it
//  is the same pointer as the one we are passing in.
bool PersistenceImplementationRead::pointerIdValid( const void* ptr, PerReadIdentifier idPair )
{
    bool    result = true;

  	ReadIdToPointerMap::const_iterator i = readIdToPointer_.find( idPair );

    if( i != readIdToPointer_.end() )
    {
        //if( _STATIC_CAST( void*, (*i).second ) != ptr )
        void* val = _REINTERPRET_CAST( void*, (*i).second);
        if(  val != ptr )
         {
            result = false;
         }
    }

    return result;
}

void PersistenceImplementationRead::fixupPointer( void** pPtr, PerReadIdentifier idPair)
{
	if( idPair.first == 0 )
		*pPtr = NULL;
    else
    {
    	ReadIdToPointerMap::const_iterator i = readIdToPointer_.find( idPair );

	    if( i == readIdToPointer_.end() )
        {
            // search by id only
            /*struct IdCmp : public std::unary_function< std::pair<PerReadIdentifier,PerMapPtrType>, bool>
            {
                explicit IdCmp(const PerIdentifier baseline ):baseline(baseline){};
                bool operator()(const std::pair<PerReadIdentifier,PerMapPtrType>& a) const
                {
                    return a.first.first == baseline;
                }
                PerIdentifier baseline;
            };
            i = std::find_if(idToPointer_.begin(), idToPointer_.end(), IdCmp(idPair.first));
            */

            //std::pair<IdToPointerMap::const_iterator,IdToPointerMap::const_iterator > jrange =
            //    idToPointer_.equal_range( idPair.first );
            /*
            if( j.first == j.second && j.second == idToPointer_.end() )
            {
                //  Object not yet loaded, push this pointer onto the list for later fixup.
                unfixedPointers_.push_back( UnfixedPointer( pPtr, idPair ) );
            }
            else if ( std::distance(j.first, j.second) == 1 )
            {
                //void*   ptr = _REINTERPRET_CAST( void*, (*j).first.second );
                void*   ptr = _REINTERPRET_CAST( void*, j.first->second );
                *pPtr = ptr;
            }
            else
            {
                std::cout << "Multiple choices for: "  << (*i).first.first
                    << " "  << (*i).first.second << std::endl;
            }
            */
            IdToPointerMap::const_iterator j = idToPointer_.find( idPair.first );
            if( j == idToPointer_.end() )
            {
                //  Object not yet loaded, push this pointer onto the list for later fixup.
                unfixedPointers_.push_back( UnfixedPointer( pPtr, idPair ) );
            }
            else
            {
                /*
                size_t choices = std::distance(jrange.first, jrange.second);
                if( choices > 1 )
                {
                    std::cout << "Multiple " << choices << " choices for: "  << idPair.second
                        << " id " << idPair.first;

                    for( IdToPointerMap::const_iterator ji = jrange.first; ji != jrange.second; ++ji )
                    {
                        std::cout << " : addr "  << ji->second << " " << ji->first ;
                        j = ji;
                    }
                    std::cout << " selected address: "<< j->second << std::endl;
                    // select second option
                    void*   ptr = _REINTERPRET_CAST( void*, j->second );
                    *pPtr = ptr;
                }
                else
                    */
                {
                    //void*   ptr = _REINTERPRET_CAST( void*, (*j).first.second );
                    void*   ptr = _REINTERPRET_CAST( void*, j->second );
                    *pPtr = ptr;
                }
            }
        }
        else
        {
            //  Object already loaded, set up the pointer correctly

            //void*   ptr = _STATIC_CAST( void*, idToPointer_[ id ] );
            //was
            //void*   ptr = _STATIC_CAST( void*, (*i).second );
            void*   ptr = _REINTERPRET_CAST( void*, (*i).second );

            *pPtr = ptr;
        }
    }
}

void PersistenceImplementationRead::fixupOutstandingPointers()
{
    TEST_INVARIANT;
    //  Fixup all of those pointers that we couldn't do when they were first encountered

    for( UnfixedPointers::const_iterator i = unfixedPointers_.begin(); i != unfixedPointers_.end(); ++i )
    {

    	ReadIdToPointerMap::const_iterator ptrIterator = readIdToPointer_.find( (*i).second );
        if(ptrIterator == readIdToPointer_.end())
        {
            IdToPointerMap::const_iterator j = idToPointer_.find( (*i).second.first  );
            if( j == idToPointer_.end() )
            {
                for( ReadIdToPointerMap::const_iterator i = readIdToPointer_.begin(); i != readIdToPointer_.end(); ++i )
                    std::cout << (*i).first.first << " "  << (*i).first.second << std::endl;

                std::cout << (*i).second.first << " NOT found "  << (*i).second.second << std::endl;
                ASSERT_FAIL( "Object not in file" );
            }
            else
            {
                *((*i).first) = _REINTERPRET_CAST( void*, j->second );
            }
        }
        else
        {
            ASSERT_INFO( (*i).second.first );
            //ASSERT( ptrIterator != readIdToPointer_.end(), "Object not in file" );

            //*((*i).first) = _STATIC_CAST( void*, idToPointer_[ (*i).second ] );
            //was:
            //*((*i).first) = _STATIC_CAST( void*, (*ptrIterator).second );
            *((*i).first) = _REINTERPRET_CAST( void*, (*ptrIterator).second );
        }
    }
}

void    PersistenceImplementationRead::clear()
{
    if( pIdMapAllocator_ )
        pushCurrentPoolAllocator( pIdMapAllocator_ );

    idToPointer_.erase( idToPointer_.begin(), idToPointer_.end() );
    readIdToPointer_.erase( readIdToPointer_.begin(), readIdToPointer_.end() );

    if( pIdMapAllocator_ )
        popCurrentPoolAllocator();

    unfixedPointers_.erase( unfixedPointers_.begin(), unfixedPointers_.end() );
}

std::string	PersistenceImplementationRead::readName( PerIstream& istr )
{
    // this is ctl stdlib string
    string  name;

    PER_READ_RAW_OBJECT( istr, name );

    std::string result(name.c_str());
    return result;
}
//#undef PER_READ_INDENT_STREAM
//#define PER_READ_INDENT_STREAM( exp ) std::cout << exp
void	PersistenceImplementationRead::readNamedObject( PerIstream& istr )
{
    std::string className = readName( istr );

    PER_READ_INDENT_STREAM( "Named object " << className << std::endl );

    ReadFnMap::const_iterator i = readFnMap_.find( className );

    void*   pObject = NULL;

    if( i != readFnMap_.end() )
    {
        readFnMap_[ className ]( istr );
    }
    else
    {
        ASSERT_INFO( className );
        ASSERT_FAIL( "Could not find read function for object. You must call 'PER_REGISTER_DERIVED_CLASS' for all derived classes." );
    }
}

void    PersistenceImplementationRead::registerDerivedClass( const char* className, PerReadFnPtr ptr )
{
    readFnMap_.emplace( className, ptr );
}


void    PersistenceImplementationRead::readObjectPre( PerIstream& istr, const void* pOb, const char* className )
{
    START_TIMING_OBJECT( className );

	startRead();
    PER_READ_INDENT_STREAM( "Start  object " << className << " " );
    //std::cout << "POS:" << istr.tellg() << std::endl;


    PerDataType type = readDataType( istr );

    ASSERT_INFO( type );
    ASSERT_INFO( className );
    ASSERT( type == PER_OBJECT, "Object expected but not found" );

    if( not readAsRaw() )
    {
    	PerIdentifier	id = readId( istr );

        PER_READ_INDENT_STREAM( "id " << id );
        writeAddress( pOb );
            PER_READ_INDENT_STREAM( std::endl );

        PerReadIdentifier idPair = PerReadIdentifier(id, strHasher_(className));
        registerObject( pOb, idPair );
    }
    else
    {
        PER_READ_INDENT_STREAM( std::endl );
    }
}

void    PersistenceImplementationRead::readObjectPost( const void* pOb, const char* className )
{
    PER_READ_INDENT_STREAM( "Finish object " << className );
    writeAddress( pOb );
    PER_READ_INDENT_STREAM( std::endl );

    finishRead();

    FINISH_TIMING_OBJECT( className );
}

void	PersistenceImplementationRead::startRead()
{
	if( inputCount_ == 0 )
	{
        clear();
        PER_READ_INDENT_STREAM( "------- Start read ------------------------------------------------------" << std::endl );
        pIdMapAllocator_ = _NEW( BasePoolAllocator( 1000000, 1000000 ) );
	}

	++inputCount_;

    if( DiagInternal::streamEnabled( DIAG_PER_READ ) )
        indentStream_.indentString( "  ", inputCount_ );
}

void	PersistenceImplementationRead::finishRead()
{
	PRE( inputCount_ != 0 );

	--inputCount_;

    if( DiagInternal::streamEnabled( DIAG_PER_READ ) )
        indentStream_.indentString( "  ", inputCount_ );

	if( inputCount_ == 0 )
	{
        fixupOutstandingPointers();
        clear();

        _DELETE( pIdMapAllocator_ );
        pIdMapAllocator_ = NULL;

        PER_READ_INDENT_STREAM( "------- Finish read -----------------------------------------------------" << std::endl );
	}
}

Persistence::PointerReadResult    PersistenceImplementationRead::readPointerPre( PerIstream& istr, void** /* ptr */, const char* className )
{
    Persistence::PointerReadResult   result = Persistence::DONT_READ_OBJECT;

	startRead();
    PER_READ_INDENT_STREAM( "Start  pointer to " << className << std::endl );

    PerDataType type = readDataType( istr );

	if( type == PER_POINTER )
	{
        //  Do nothing. The id will be read in by readPointerPost
	}
	else if( type == PER_OBJECT_POINTER )
	{
        result = Persistence::READ_OBJECT;
	}
	else if( type == PER_NAMED_OBJECT_POINTER )
	{
		readNamedObject( istr );
	}
	else
    {
        std::cout << std::endl << "POS:" << istr.tellg() << std::endl;
		ASSERT_FAIL( "Expecting a pointer, found an object" );
    }

    return result;
}

void    PersistenceImplementationRead::readPointerPost( PerIstream& istr, void** ptr, const char* className )
{
    PerIdentifier   id = readId( istr );
    fixupPointer( (void**)ptr, PerReadIdentifier(id, strHasher_(className) ) );

    PER_READ_INDENT_STREAM( "Finish pointer to " << className << " id " << id );
    writeAddress( *ptr );
    PER_READ_INDENT_STREAM( std::endl );

	finishRead();
}

void PersistenceImplementationRead::readRawPointer( PerIstream& istr, void** ptr, const char* className )
{
    PerDataType type = readDataType( istr );

    ASSERT_INFO( type );
    ASSERT( type == PER_RAW_POINTER, "Expecting a raw pointer" );

    PerIdentifier   id = readId( istr );
    fixupPointer( ptr, PerReadIdentifier(id, strHasher_(className) ) );

    PER_READ_INDENT_STREAM( "Raw pointer " << id );
    writeAddress( *ptr );
    PER_READ_INDENT_STREAM( std::endl );
}


void    PersistenceImplementationRead::readPointerAbstract( PerIstream& istr, void** ptr, const char* className )
{
    Persistence::PointerReadResult   result;
    result = readPointerPre( istr, ptr, className );
    ASSERT_INFO( className );
    ASSERT( result == Persistence::DONT_READ_OBJECT, "Tried to read abstract class pointer as concrete class" );
    readPointerPost( istr, ptr, className );
}

//  Read in a data type marker. This is isolated into a separate function to allow human
//  readable data types to be used for debugging later.
PerDataType PersistenceImplementationRead::readDataType( PerIstream& istr )
{
	char c;
	istr.read( &c, 1 );

    PerDataType type = _STATIC_CAST( PerDataType, c );

    return type;
}

//  Read in an object id.
//  This is isolated into a separate function to allow human readable ids to be used
//  later for debugging purposes
PerIdentifier PersistenceImplementationRead::readId( PerIstream& istr )
{
    PerIdentifier   id;
    //istr.read( (char*)&id, sizeof( PerIdentifier ) );
    istr.read( (char*)&id, 4 );

    return id;
}

void PersistenceImplementationRead::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void PersistenceImplementationRead::readAsRaw( bool raw )
{
    if( raw )
    {
        ++readAsRawCount_;
        START_TIMING_OBJECT( "rawObject" );
    }
    else
    {
        ASSERT( readAsRawCount_ > 0, "Unmatched readAsRaw calls" );
        --readAsRawCount_;
        FINISH_TIMING_OBJECT( "rawObject" );
    }
}

bool PersistenceImplementationRead::readAsRaw() const
{
    return readAsRawCount_ > 0;
}

void PersistenceImplementationRead::readRawData( PerIstream& istr, char* ptr, size_t nBytes )
{
    START_TIMING_OBJECT( "rawData" );
    istr.read( ptr, nBytes );
    FINISH_TIMING_OBJECT( "rawData" );
}

ostream& PersistenceImplementationRead::debugStream()
{
    return indentStream_;
}

void PersistenceImplementationRead::logAddresses( bool flag )
{
    logAddresses_ = flag;
}

bool PersistenceImplementationRead::logAddresses() const
{
    return logAddresses_;
}

void PersistenceImplementationRead::writeAddress( const void* ptr ) const
{
    if( logAddresses() )
        //PER_READ_INDENT_STREAM( " (" << _STATIC_CAST( void*, ptr ) << ") " );
        PER_READ_INDENT_STREAM( " (" << _CONST_CAST( void*, ptr ) << ") " );
}

ostream& operator <<( ostream& o, const PersistenceImplementationRead& t )
{

    o << "PersistenceImplementationRead " << (void*)&t << " start" << std::endl;
    o << "PersistenceImplementationRead " << (void*)&t << " end" << std::endl;

    return o;
}

// =====================================================================================

//  Functions to collect timing information for classes that are being read in.

// TODO check
#include "ctl/utility.hpp"
//#include "ctl/algorith.hpp"

CTL_QUAD( Data, string, name, double, time, size_t, count, size_t, recursionCount );

typedef	ctl_map< std::string, Data, std::less< std::string > > TimeMap;
static TimeMap timeMap;
static  std::string  classPath;

#ifdef TIME_OBJECTS

//  TBD: Fix this code to use DevTimers rather than UtlDebugTimers (which no longer exist)

void    startTiming( const string& className )
{
    double currentTime = UtlDebugTimer::masterTimer().time().asDouble();

    if( classPath != "" )
    {
        //  Subtract the time this class is taking to write from the time its "owning"
        //  class took to write to get a true time output
        TimeMap::iterator i = timeMap.find( classPath );
        Data& data = (*i).second;
        data.time( data.time() + currentTime );
    }

    classPath += className;

    TimeMap::iterator i = timeMap.find( classPath );
    if( i == timeMap.end() )
    {
        Data    data( classPath, 0.0, 0, 0 );
        timeMap.insert( classPath, data );
        i = timeMap.find( classPath );
    }

    Data& data = (*i).second;

    data.time( data.time() - currentTime );

    data.count( data.count() + 1 );
    data.recursionCount( data.recursionCount() + 1 );
}

void    finishTiming( const string& className )
{
    double currentTime = UtlDebugTimer::masterTimer().time().asDouble();

    TimeMap::iterator i = timeMap.find( classPath );
    ASSERT( i != timeMap.end(), "" );

    Data& data = (*i).second;
    data.recursionCount( data.recursionCount() - 1 );

    data.time( data.time() + currentTime );

    classPath = classPath.substr( 0, classPath.length() - className.length() );

    if( classPath != "" )
    {
        //  Subtract the time this class is taking to write from the time its "owning"
        //  class took to write to get a true time output
        TimeMap::iterator i = timeMap.find( classPath );
        Data& data = (*i).second;
        data.time( data.time() - currentTime );
    }
}

#endif

static  bool    dataLess( const Data& a, const Data& b )
{
    return a.time() < b.time();
}

void persistenceImplementationReadWriteTimingData( ostream& ostr )
{
    typedef ctl_vector< Data >  DataVec;

    DataVec vec;

    for( TimeMap::iterator i = timeMap.begin(); i != timeMap.end(); ++i )
    {
        vec.push_back( (*i).second );
    }

    std::sort( vec.begin(), vec.end(), dataLess );

    double totalTime = 0.0;

    ostr << "===============================================================================" << std::endl;

    for( DataVec::iterator i = vec.begin(); i != vec.end(); ++i )
    {
        ostr << std::setw( 15 ) << std::setprecision( 10 ) << (*i).time() << " " << std::setw( 8 ) << (*i).count() << " " << (*i).name() << std::endl;
        totalTime += (*i).time();
    }

    ostr << "Total time " << totalTime << std::endl;
    ostr << "===============================================================================" << std::endl;
}

void    PersistenceImplementationRead::registerOpenIstream()
{
    //  Only allow one input stream to be in existance at a time
    PRE( nOpenIstreams_ == 0 );

    ++nOpenIstreams_;
}

void    PersistenceImplementationRead::registerCloseIstream()
{
    PRE( nOpenIstreams_ != 0 );

    --nOpenIstreams_;
}

/* End PERIREAD.CPP *************************************************/
