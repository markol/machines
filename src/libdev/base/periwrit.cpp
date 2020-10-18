/*
 * P E R I W R I T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/periwrit.hpp"
#include "base/ostr.hpp"
#include "base/persist.hpp"
#include "base/internal/perolog.hpp"

// persistent string
#include "stdlib/string.hpp"

PersistenceImplementationWrite::PersistenceImplementationWrite()
: outputCount_( 0 ),
  indentStream_( Diag::instance().perWriteStream(), "" ),
  writeAsRawCount_( 0 ),
  logAddresses_( false ),
  nOpenOstreams_( 0 )
{
    clear();

    TEST_INVARIANT;
}

PersistenceImplementationWrite::~PersistenceImplementationWrite()
{
    TEST_INVARIANT;

}

PerIdentifier  PersistenceImplementationWrite::identifier( const void* ptr )
{
	PerIdentifier	identifier = 0;

	if( ptr == 0 )
		identifier = 0;
	else
	{
        //PerMapPtrType  mapPtr = _STATIC_CAST( PerMapPtrType, ptr );
        PerMapPtrType  mapPtr = _REINTERPRET_CAST( PerMapPtrType, ptr );

		PointerToIdMap::const_iterator i = pointerToId_.find( mapPtr );

		if( i == pointerToId_.end() )
		{
			identifier = currentIdentifier_++;
			pointerToId_.insert( mapPtr, identifier );
		}
		else
			identifier = pointerToId_[ mapPtr ];
	}

	return identifier;
}

void    PersistenceImplementationWrite::logWrittenAsObject( const char* classname, const void* ptr )
{
    ASSERT_INFO( "A" );
    ASSERT_INFO( classname );
    ASSERT_INFO( (void*)ptr );

    writtenAsObject_.insert( PersistenceObjectLog( classname, ptr ) );

    ASSERT_INFO( "A" );
}

void    PersistenceImplementationWrite::logWrittenViaPointer( const void* ptr )
{
    writtenViaPointer_.insert( ptr );
}

bool    PersistenceImplementationWrite::writtenAsObject( const char* classname, const void* ptr )
{
    return writtenAsObject_.find( PersistenceObjectLog( classname, ptr ) ) != writtenAsObject_.end();
}

bool    PersistenceImplementationWrite::writtenViaPointer( const void* ptr ) const
{
    return writtenViaPointer_.find( ptr ) != writtenViaPointer_.end();
}

void    PersistenceImplementationWrite::clear()
{
    pointerToId_.erase( pointerToId_.begin(), pointerToId_.end() );
    writtenAsObject_.erase( writtenAsObject_.begin(), writtenAsObject_.end() );
    writtenViaPointer_.erase( writtenViaPointer_.begin(), writtenViaPointer_.end() );

    currentIdentifier_ = 100;
}

void	PersistenceImplementationWrite::writeName( PerOstream& ostr, const char* name )
{
    PER_WRITE_RAW_OBJECT( ostr, string( name ) );
}

//  Do all preparetory work necessary for writing out an object
void PersistenceImplementationWrite::writeObjectPre( PerOstream& ostr, const void* pOb, const char* className )
{
    ASSERT_INFO( "a" );

	startWrite();

    PER_WRITE_INDENT_STREAM( "Start  object " << className << " " );

    ASSERT_INFO( className );
    ASSERT_INFO( (void*)pOb );

    if( not writeAsRaw() )
    {
        ASSERT( not writtenAsObject( className, pOb ), "Object has already been written as an object" );
    }

    write( ostr, PER_OBJECT );

    if( not writeAsRaw() )
    {
    	PerIdentifier	id = identifier( pOb );
        writeId( ostr, id );

        logWrittenAsObject( className, pOb );

        PER_WRITE_INDENT_STREAM( "id " << id );
        writeAddress( pOb );
        PER_WRITE_INDENT_STREAM( std::endl );
    }
    else
    {
        PER_WRITE_INDENT_STREAM( std::endl );
    }
    ASSERT_INFO( "a" );
}

//  Do all cleanup work necessary after writing out an object
void PersistenceImplementationWrite::writeObjectPost( const void* pOb, const char* className )
{
    ASSERT_INFO( "a" );

    PER_WRITE_INDENT_STREAM( "Finish object " << className );
    writeAddress( pOb );
    PER_WRITE_INDENT_STREAM( std::endl );

	finishWrite();

    ASSERT_INFO( "a" );
}

//  Do all preparetory work necessary for writing out a pointer to an object
Persistence::PointerWriteResult PersistenceImplementationWrite::writePointerPre(
    PerOstream& ostr, const void* pOb, const char* className,
    const void* pMostDerivedOb, const char* mostDerivedClassName )
{

	startWrite();
    PER_WRITE_INDENT_STREAM( "Start  pointer to " << className << std::endl );

    ASSERT_INFO( className );
    ASSERT_INFO( mostDerivedClassName );
    ASSERT_INFO( (void*)pOb );
    ASSERT_INFO( (void*)pMostDerivedOb );

    Persistence::PointerWriteResult  result = Persistence::DONT_WRITE_OBJECT;

    if( writtenViaPointer( pMostDerivedOb ) or
        pMostDerivedOb == NULL or
        writtenAsObject( mostDerivedClassName, pMostDerivedOb ) )
    {
        logWrittenViaPointer( pMostDerivedOb );

        write( ostr, PER_POINTER );
    }
    else
    {
        logWrittenViaPointer( pMostDerivedOb );

        if( strcmp( className, mostDerivedClassName ) == 0 )
        {
            //  Object is being written out as its most derived class
            result = Persistence::WRITE_OBJECT;
            write( ostr, PER_OBJECT_POINTER );
        }
        else
        {
            write( ostr, PER_NAMED_OBJECT_POINTER );
            writeNamedObject( ostr, mostDerivedClassName, pMostDerivedOb );
        }
    }
    return result;
}

//  Do all cleanup work necessary after writing out a pointer to an object
void PersistenceImplementationWrite::writePointerPost( PerOstream& ostr,
    const void* pOb, const char* className, const void* /* pMostDerivedOb */ )
{
	PerIdentifier	id = identifier( (void*)pOb );
    writeId( ostr, id );

    PER_WRITE_INDENT_STREAM( "Finish pointer to " << className << " id " << id );
    writeAddress( pOb );
    PER_WRITE_INDENT_STREAM( std::endl );

    finishWrite();
}

void PersistenceImplementationWrite::writeRawPointer( PerOstream& ostr, const void* pOb )
{
	PerIdentifier	id = identifier( pOb );

    write( ostr, PER_RAW_POINTER );
    writeId( ostr, id );
    PER_WRITE_INDENT_STREAM( "Raw pointer " << id );
    writeAddress( pOb );
    PER_WRITE_INDENT_STREAM( std::endl );
}

void	PersistenceImplementationWrite::writeNamedObject( PerOstream& ostr, const char* className, const void* ptr )
{
	writeName( ostr, className );

    PER_WRITE_INDENT_STREAM( "Named object " << className << std::endl );

    WriteFnMap::const_iterator i = writeFnMap_.find( className );

    if( i != writeFnMap_.end() )
    {
        writeFnMap_[ className ]( ostr, ptr );
    }
    else
    {
        ASSERT_INFO( className );
        INSPECT_ON( std::cout, className );
        ASSERT_FAIL( "Could not find write function for object. You must call 'PER_REGISTER_DERIVED_CLASS' for all derived classes." );
    }
}

//  Write out a data type marker. This is isolated into a separate function to allow human
//  readable data types to be used for debugging later.
void PersistenceImplementationWrite::write( PerOstream& ostr, PerDataType type )
{
    ASSERT( type < 256, "Internal error - PerDataType > 256" );

	char c = _STATIC_CAST( char, type );
	ostr.write( &c, 1 );
}

//  Write out an object id.
//  This is isolated into a separate function to allow human readable ids to be used
//  later for debugging purposes
void PersistenceImplementationWrite::writeId( PerOstream& ostr, PerIdentifier id )
{
    ostr.write( (char*)&id, sizeof( PerIdentifier ) );
}

void    PersistenceImplementationWrite::registerDerivedClass( const char* className, PerWriteFnPtr ptr )
{
    writeFnMap_.insert( className, ptr );
}

void	PersistenceImplementationWrite::startWrite( void )
{
	if( outputCount_ == 0 )
	{
        clear();
        PER_WRITE_INDENT_STREAM( "------- Start write -----------------------------------------------------" << std::endl );
	}

	++outputCount_;

    if( DiagInternal::streamEnabled( DIAG_PER_WRITE ) )
        indentStream_.indentString( "  ", outputCount_ );
}

void	PersistenceImplementationWrite::finishWrite( void )
{
	PRE( outputCount_ != 0 );

	--outputCount_;

    if( DiagInternal::streamEnabled( DIAG_PER_WRITE ) )
        indentStream_.indentString( "  ", outputCount_ );

	if( outputCount_ == 0 )
	{
        clear();
        PER_WRITE_INDENT_STREAM( "------- Finish write ----------------------------------------------------" << std::endl );
	}

}

void PersistenceImplementationWrite::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void PersistenceImplementationWrite::writeAsRaw( bool raw )
{
    if( raw )
    {
        ++writeAsRawCount_;
    }
    else
    {
        ASSERT( writeAsRawCount_ > 0, "Unmatched writeAsRaw calls" );
        --writeAsRawCount_;
    }
}

bool PersistenceImplementationWrite::writeAsRaw( void ) const
{
    return writeAsRawCount_ > 0;
}

void PersistenceImplementationWrite::writeRawData( PerOstream& ostr, const char* ptr, size_t nBytes )
{
    ostr.write( ptr, nBytes );
}

void PersistenceImplementationWrite::logAddresses( bool flag )
{
    logAddresses_ = flag;
}

bool PersistenceImplementationWrite::logAddresses() const
{
    return logAddresses_;
}

void PersistenceImplementationWrite::writeAddress( const void* ptr ) const
{
    if( logAddresses() )
        //PER_WRITE_INDENT_STREAM( " (" << _STATIC_CAST( void*, ptr ) << ") " );
        PER_WRITE_INDENT_STREAM( " (" << _CONST_CAST( void*, ptr ) << ") " );
}

ostream& PersistenceImplementationWrite::debugStream()
{
    return indentStream_;
}

void    PersistenceImplementationWrite::registerOpenOstream()
{
    //  Only allow one output stream to be in existance at a time
    PRE( nOpenOstreams_ == 0 );

    ++nOpenOstreams_;
}

void    PersistenceImplementationWrite::registerCloseOstream()
{
    PRE( nOpenOstreams_ != 0 );

    --nOpenOstreams_;
}

ostream& operator <<( ostream& o, const PersistenceImplementationWrite& t )
{

    o << "PersistenceImplementationWrite " << (void*)&t << " start" << std::endl;
    o << "PersistenceImplementationWrite " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PERIWRIT.CPP *************************************************/
