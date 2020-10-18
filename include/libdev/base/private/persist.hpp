/*
 * P E R S I S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    Persistence

    Implementations of the persistence macros and headers
    for the classes that the user should never need to know about.
*/

#ifndef _PER_PRIVATE_PERSIST_HPP
#define _PER_PRIVATE_PERSIST_HPP

#include <fstream>
//#include <cxxabi.h>
#include <typeinfo>

#include "base/base.hpp"
#include "base/private/pertypes.hpp"

#include "base/ostr.hpp"
#include "base/istr.hpp"

class	PersistenceImplementationWrite;
class	PersistenceImplementationRead;

class   BaseIndentOstream;

class Persistence
{
public:
    //  Singleton class
    static Persistence& instance( void );
    ~Persistence( void );

	void    registerDerivedClass( const char* className, PerWriteFnPtr, PerReadFnPtr );

    //  Write methods -------------------------------------------------------

    //  Do all preparatory work necessary for writing out an object
    void writeObjectPre( PerOstream& ostr, const void* pOb, const char* className );

    //  Do all cleanup work necessary after writing out an object
    void writeObjectPost( const void* pOb, const char* className );

    enum PointerWriteResult { WRITE_OBJECT, DONT_WRITE_OBJECT };

    //  Do all preparatory work necessary for writing out a pointer to an object
    PointerWriteResult writePointerPre(
        PerOstream& ostr, const void* pOb, const char* className,
        const void* pMostDerivedOb, const char* mostDerivedClassName );

    //  Do all cleanup work necessary after writing out a pointer to an object
    void writePointerPost( PerOstream&,
        const void* pOb, const char* className, const void* pMostDerivedOb );

    void    writeRawPointer( PerOstream&, const void* );

    void    writeAsRaw( bool );

    void    writeRawData( PerOstream& ostr, const char* ptr, size_t nBytes );

    //  Read methods --------------------------------------------------------

    //  Do all preparatory work necessary for reading in an object
    void readObjectPre( PerIstream& istr, const void* pOb, const char* className );
    //  Do all cleanup work necessary after reading in an object
    void readObjectPost( const void* pOb, const char* className );

    enum PointerReadResult { READ_OBJECT, DONT_READ_OBJECT };

    //  Do all preparatory work necessary for reading in a pointer
    PointerReadResult readPointerPre( PerIstream& istr, void** ptr, const char* className );
    //  Do all cleanup work necessary after reading in a pointer
    void readPointerPost( PerIstream& istr, void** ptr, const char* className );
    void readPointerAbstract( PerIstream& istr, void** ptr, const char* className );

    void    readRawPointer( PerIstream&, void** );

    void    readRawData( PerIstream& istr, char* ptr, size_t nBytes );

    void    readAsRaw( bool );

    // ----------------------------------------------------------------------------

    ostream& debugWriteStream();
    ostream& debugReadStream();

    void    logAddresses( bool );

    void    registerOpenIstream();
    void    registerCloseIstream();
    void    registerOpenOstream();
    void    registerCloseOstream();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    Persistence( const Persistence& );

    // Operation deliberately revoked
    Persistence& operator =( const Persistence& );

    // Operation deliberately revoked
    bool operator ==( const Persistence& );

    Persistence( void );

	PersistenceImplementationWrite*	pImplementationWrite_;
	PersistenceImplementationRead*	pImplementationRead_;
};

//  This enum is purely to allow a "persistence default" constructor to be defined.
enum PerConstructor { PERSISTENCE_CONSTRUCTOR };

//================= DEBUG MACROS ===================================================

#define PER_PRIVATE_WRITE_INDENT_STREAM( exp ) DiagInternal::streamEnabled( DIAG_PER_WRITE ) ? Persistence::instance().debugWriteStream() << exp : Persistence::instance().debugWriteStream() << ""
#define PER_PRIVATE_READ_INDENT_STREAM( exp ) DiagInternal::streamEnabled( DIAG_PER_READ ) ? Persistence::instance().debugReadStream() << exp : Persistence::instance().debugReadStream() << ""

//  If yesNo is set to true all characters written will be logged
#define PER_PRIVATE_LOG_WRITE( yesNo )      PerOstream::logWrite() = yesNo;

//  If yesNo is set to true all characters read will be logged
#define PER_PRIVATE_LOG_READ( yesNo )       PerIstream::logRead() = yesNo;

//  If yesNo is set to true all object addresses will be written to PER_WRITE_STREAM & PER_READ_STREAM
#define PER_PRIVATE_LOG_ADDRESSES( yesNo )   Persistence::instance().logAddresses( yesNo );

//================= MACROS FOR ALL CLASSES =========================================

//  These are the standard methods that get included in declarations of
//  all persistent classes.
#define	PER_PRIVATE_MEMBER_PERSISTENT_STANDARD( className, VIRTUAL )   		\
    public:                                                         \
    	static const char* perClassName( void );    	            \
    	VIRTUAL const char* perMostDerivedClassName( void ) const;	\
    	VIRTUAL char* perPDerivedClass( void ) const;               \
    public:

#define	PER_PRIVATE_MEMBER_PERSISTENT( className, VIRTUAL )   		        \
    public:                                                         \
        static  className& perCreate()                              \
            { return *(_NEW( className( PERSISTENCE_CONSTRUCTOR ) ) ); }   \
        className( PerConstructor );                                \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT( className, VIRTUAL )   		\
    public:                                                         \
        static  className& perCreate()                              \
            { return *(_NEW( className() ) ); }                    \
    protected:                                                      \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_ABSTRACT( className, VIRTUAL )   		\
    protected:                                                      \
        className( PerConstructor );                                \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( className, VIRTUAL ) \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD( className, VIRTUAL )



#define PER_PRIVATE_CLASSNAME_DEFINITION( className )               \
        	{ return #className; }

#define PER_PRIVATE_VIRTUAL_WRITE_DEFINITION                        \
    	    { perWrite( ostr, *this ); }

#define PER_PRIVATE_VIRTUAL_CLASSNAME_DEFINITION                    \
            { return this->perClassName(); }

#define PER_PRIVATE_P_DERIVED_CLASS_DEFINITION                      \
        { return _CONST_CAST( char*, _REINTERPRET_CAST( const char*, this ) ); }



#define	PER_PRIVATE_MEMBER_PERSISTENT_STANDARD_INLINE( className, VIRTUAL )  \
    public:                                                         \
    	static const char* perClassName( void )     	            \
        	PER_PRIVATE_CLASSNAME_DEFINITION( className )		    \
    private:                                                        \
    	VIRTUAL const char* perMostDerivedClassName( void ) const	\
            PER_PRIVATE_VIRTUAL_CLASSNAME_DEFINITION                \
    	VIRTUAL char* perPDerivedClass( void ) const                \
            PER_PRIVATE_P_DERIVED_CLASS_DEFINITION                  \
	    friend PerOstream& operator <<( PerOstream& ostr, const className& ob )	\
    	PER_OBJECT_WRITE( className )									        \
	    friend PerIstream& operator >>( PerIstream& istr, className& ob )	    \
    	PER_OBJECT_READ( className )										    \
	    friend PerOstream& operator <<( PerOstream& ostr, const className* pOb )	\
    	PER_POINTER_WRITE( className )											\
	    friend PerIstream& operator >>( PerIstream& istr, className*& pOb )	    \
    	PER_POINTER_READ( className, className::perCreate() )                                          \
    public:



#define	PER_PRIVATE_MEMBER_PERSISTENT_INLINE( className, VIRTUAL )	        \
    public:                                                                 \
        static  className& perCreate()                                      \
            { return *(_NEW( className( PERSISTENCE_CONSTRUCTOR ) ) ); }    \
    protected:                                                              \
        className( PerConstructor );                                        \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD_INLINE( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_INLINE( className, VIRTUAL )   		\
    public:                                                                 \
        static  className& perCreate()                              \
            { return *(_NEW( className() ) ); }                    \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD_INLINE( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_ABSTRACT_INLINE( className, VIRTUAL )   		\
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD_INLINE( className, VIRTUAL )

#define	PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_ABSTRACT_INLINE( className, VIRTUAL )  \
    PER_PRIVATE_MEMBER_PERSISTENT_STANDARD_INLINE( className, VIRTUAL )


#define	PER_OBJECT_WRITE( className )		    \
{											    \
    ASSERT_INFO( "w" );                         \
	Persistence::instance().writeObjectPre( ostr, _STATIC_CAST( const void*, &ob ), #className ); \
    ASSERT_INFO( "x" );                         \
	perWrite( ostr, ob );						\
    ASSERT_INFO( "y" );                         \
	Persistence::instance().writeObjectPost( _STATIC_CAST( const void*, &ob ), #className ); \
    ASSERT_INFO( "z" );                         \
	return ostr;								\
}

#define	PER_OBJECT_READ( className )			\
{											    \
	Persistence::instance().readObjectPre( istr, _STATIC_CAST( const void*, &ob ), #className ); \
	perRead( istr, ob );						\
	Persistence::instance().readObjectPost( _STATIC_CAST( const void*, &ob ), #className ); \
	return istr;								\
}

#define	PER_POINTER_WRITE( className )							\
{											                    \
    void* pMostDerivedOb = pOb ? pOb->perPDerivedClass() : (void*)pOb;          \
    const char* mostDerivedClassName =  pOb ? pOb->perMostDerivedClassName() : NULL;    \
    if( Persistence::instance().writePointerPre(                                    \
      ostr, _STATIC_CAST( const void*, pOb ), #className, pMostDerivedOb, mostDerivedClassName ) == Persistence::WRITE_OBJECT ) \
    {                                                                           \
        ostr << *pOb;       \
    }                                                                           \
    Persistence::instance().writePointerPost( ostr, _STATIC_CAST( const void*, pOb ), #className, pMostDerivedOb );    \
    return ostr;                                                                \
}

#define	PER_POINTER_READ( className, createFunction )					\
{                       \
    if( Persistence::instance().readPointerPre( istr, _REINTERPRET_CAST( void**, &pOb ), #className ) == Persistence::READ_OBJECT )  \
    {                                           \
        istr >> createFunction;         \
    }                                           \
    Persistence::instance().readPointerPost( istr, _REINTERPRET_CAST( void**, &pOb ), #className );  \
    return istr;        \
}

#define	PER_POINTER_READ_ABSTRACT( className )					\
{                       \
    Persistence::instance().readPointerAbstract( istr, _REINTERPRET_CAST( void**, &pOb ), #className );  \
    return istr;        \
}


#define PER_PRIVATE_WRITE_RAW_POINTER( ostr, ptr ) Persistence::instance().writeRawPointer( ostr, ptr )

//#define PER_PRIVATE_READ_RAW_POINTER( istr, ptr )  Persistence::instance().readRawPointer( istr, _STATIC_CAST( void**, &ptr ) )
#define PER_PRIVATE_READ_RAW_POINTER( istr, ptr )  Persistence::instance().readRawPointer( istr, _REINTERPRET_CAST( void**, &ptr ) )

#define PER_PRIVATE_WRITE_RAW_OBJECT( ostr, ob )    \
    Persistence::instance().writeAsRaw( true ),     \
    perWrite( ostr, ob ),                           \
    Persistence::instance().writeAsRaw( false )

#define PER_PRIVATE_READ_RAW_OBJECT( istr, ob )     \
    Persistence::instance().readAsRaw( true ),      \
    perRead( istr, ob ),                            \
    Persistence::instance().readAsRaw( false )

#define PER_PRIVATE_WRITE_RAW_DATA( ostr, ptr, nBytes )     \
    Persistence::instance().writeRawData( ostr, _REINTERPRET_CAST( const char*, ptr ), nBytes )
    //Persistence::instance().writeRawData( ostr, _STATIC_CAST( const char*, ptr ), nBytes )

#define PER_PRIVATE_READ_RAW_DATA( istr, ptr, nBytes )      \
    Persistence::instance().readRawData( istr, _REINTERPRET_CAST( char*, ptr ), nBytes )
    //Persistence::instance().readRawData( istr, _STATIC_CAST( char*, ptr ), nBytes )

#define PER_PRIVATE_READ_CONST_OBJECT( ISTR, NON_CONST_TYPE, OBJECT )      \
    ISTR >> _CONST_CAST( NON_CONST_TYPE&, OBJECT )


#define PER_PRIVATE_WRITE_ENUM_DEFINITION( TYPE )                   \
    inline PerOstream& operator <<( PerOstream& ostr, const TYPE& val )    \
    {                                                               \
        PER_WRITE_RAW_DATA( ostr, &val, sizeof( TYPE ) );           \
        return ostr;                                                \
    }

#define PER_PRIVATE_READ_ENUM_DEFINITION( TYPE )                    \
    inline PerIstream& operator >>( PerIstream& istr, TYPE& val )   \
    {                                                               \
        PER_READ_RAW_DATA( istr, &val, sizeof( TYPE ) );            \
        return istr;                                                \
    }

#define PER_PRIVATE_ENUM_PERSISTENT( TYPE )                         \
    PER_PRIVATE_WRITE_ENUM_DEFINITION( TYPE )                       \
    PER_PRIVATE_READ_ENUM_DEFINITION( TYPE )

#define PER_PRIVATE_FRIEND_ENUM_PERSISTENT( TYPE )                  \
    friend                                                          \
    PER_PRIVATE_WRITE_ENUM_DEFINITION( TYPE )                       \
    friend                                                          \
    PER_PRIVATE_READ_ENUM_DEFINITION( TYPE )

//================= MACROS FOR NON-TEMPLATE CLASSES ================================

#define	PER_PRIVATE_DECLARE_PERSISTENT( className )					\
	PerOstream& operator <<( PerOstream& ostr, const className& ob );	\
	PerIstream& operator >>( PerIstream& istr, className& ob );	        \
	PerOstream& operator <<( PerOstream& ostr, const className* pOb );	\
	PerIstream& operator >>( PerIstream& istr, className*& pOb );        \
    void perRead ## className( PerIstream& );               \
    void perWrite ## className( PerOstream&, const void* pVoid );

#define	PER_PRIVATE_DEFINE_PERSISTENT( className )					    \
	char* className::perPDerivedClass( void ) const                \
        { return _CONST_CAST( char*, _REINTERPRET_CAST( const char*, this ) ); }                 \
	const char* className::perClassName( void )     	                \
    	{ return #className; }										    \
	const char* className::perMostDerivedClassName( void ) const   \
        { return this->perClassName(); }                       \
    void perRead ## className( PerIstream& istr )               \
    {                                                           \
        istr >> className::perCreate();                         \
    }                                                           \
    void perWrite ## className( PerOstream& ostr, const void* pVoid )   \
    {                                                           \
        const className* pObject = _STATIC_CAST( const className*, pVoid ); \
        ostr << *pObject;                                       \
    }                                                           \
	PerOstream& operator <<( PerOstream& ostr, const className& ob )	\
	PER_OBJECT_WRITE( className )									        \
	PerIstream& operator >>( PerIstream& istr, className& ob )	        \
	PER_OBJECT_READ( className )										        \
	PerOstream& operator <<( PerOstream& ostr, const className* pOb )	\
	PER_POINTER_WRITE( className )												\
	PerIstream& operator >>( PerIstream& istr, className*& pOb )	    \
	PER_POINTER_READ( className, className::perCreate() )

#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT( className )					    \
	char* className::perPDerivedClass( void ) const                \
        { return _CONST_CAST( char*, _REINTERPRET_CAST( const char*, this ) ); }                 \
	const char* className::perClassName( void )     	                \
    	{ return #className; }										    \
	const char* className::perMostDerivedClassName( void ) const   \
        { return this->perClassName(); }                       \
	PerOstream& operator <<( PerOstream& ostr, const className& ob )	\
	PER_OBJECT_WRITE( className )									        \
	PerIstream& operator >>( PerIstream& istr, className& ob )	        \
	PER_OBJECT_READ( className )										        \
	PerOstream& operator <<( PerOstream& ostr, const className* pOb )	\
	PER_POINTER_WRITE( className )												\
	PerIstream& operator >>( PerIstream& istr, className*& pOb )	    \
	PER_POINTER_READ_ABSTRACT( className )

#define	PER_PRIVATE_DEFINE_PERSISTENT_INLINE( className )					    \
	friend PerOstream& operator <<( PerOstream& ostr, const className& ob )	\
	PER_OBJECT_WRITE( className )									        \
	friend PerIstream& operator >>( PerIstream& istr, className& ob )	        \
	PER_OBJECT_READ( className )										        \
	friend PerOstream& operator <<( PerOstream& ostr, const className* pOb )	\
	PER_POINTER_WRITE( className )												\
	friend PerIstream& operator >>( PerIstream& istr, className*& pOb )	    \
	PER_POINTER_READ( className )

#define	PER_PRIVATE_REGISTER_DERIVED_CLASS( className )         \
    Persistence::instance().registerDerivedClass( className::perClassName(), perWrite ## className, perRead ## className )

#define PER_PRIVATE_READ_WRITE( className )                 \
    void perWrite( PerOstream&, const className& );         \
    void perRead( PerIstream&, className& );

#define PER_PRIVATE_FRIEND_READ_WRITE( className )          \
    friend void perWrite( PerOstream&, const className& );  \
    friend void perRead( PerIstream&, className& );

//================= MACROS FOR TEMPLATE CLASSES ================================

//  See the C FAQs for details of this hack. Basically it allows us to pass
//  arguments containing commas into macros
#define PER_COMMA   ,

#define CLASS_LIST_T1   class T1
#define CLASS_LIST_T2   class T1 PER_COMMA class T2
#define CLASS_LIST_T3   class T1 PER_COMMA class T2 PER_COMMA class T3
#define CLASS_LIST_T4   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4
#define CLASS_LIST_T5   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5
#define CLASS_LIST_T6   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5 PER_COMMA class T6
#define CLASS_LIST_T7   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5 PER_COMMA class T6 PER_COMMA class T7
#define CLASS_LIST_T8   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5 PER_COMMA class T6 PER_COMMA class T7 PER_COMMA class T8
#define CLASS_LIST_T9   class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5 PER_COMMA class T6 PER_COMMA class T7 PER_COMMA class T8 PER_COMMA class T9
#define CLASS_LIST_T10  class T1 PER_COMMA class T2 PER_COMMA class T3 PER_COMMA class T4 PER_COMMA class T5 PER_COMMA class T6 PER_COMMA class T7 PER_COMMA class T8 PER_COMMA class T9 PER_COMMA class T10

#define LIST_T1         T1
#define LIST_T2         T1 PER_COMMA T2
#define LIST_T3         T1 PER_COMMA T2 PER_COMMA T3
#define LIST_T4         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4
#define LIST_T5         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5
#define LIST_T6         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5 PER_COMMA T6
#define LIST_T7         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5 PER_COMMA T6 PER_COMMA T7
#define LIST_T8         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5 PER_COMMA T6 PER_COMMA T7 PER_COMMA T8
#define LIST_T9         T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5 PER_COMMA T6 PER_COMMA T7 PER_COMMA T8 PER_COMMA T9
#define LIST_T10        T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5 PER_COMMA T6 PER_COMMA T7 PER_COMMA T8 PER_COMMA T9 PER_COMMA T10

#define	PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T, LIST_T )		\
	template< CLASS_LIST_T >		                                            \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >& ob );	\
	template< CLASS_LIST_T >										            \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >& ob );	    \
	template< CLASS_LIST_T >										            \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >* pOb );	\
	template< CLASS_LIST_T >										            \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >*& pOb );

//	template < CLASS_LIST_T > const char* className< LIST_T >::perClassName( void ) const;

//{ return #className ## #STRING_T; }
#define	PER_PRIVATE_DEFINE_PERSISTENT_T( className, LIST_T, STRING_T )			\
    template < > char* className< LIST_T >::perPDerivedClass( void ) const                   \
        { return _CONST_CAST( char*, _REINTERPRET_CAST( const char*, this ) ); }                                \
    template < > const char* className< LIST_T >::perClassName( void )		            \
	    { return #className #STRING_T; }							            \
	template < > const char* className< LIST_T >::perMostDerivedClassName( void ) const      \
        { return this->perClassName(); }                                        \
	template < > PerOstream& operator << < LIST_T >( PerOstream& ostr, const className< LIST_T >& ob )	\
	PER_OBJECT_WRITE( className )											    \
	template < > PerIstream& operator >> < LIST_T >( PerIstream& istr, className< LIST_T >& ob )	    \
	PER_OBJECT_READ( className )												\
	template < > PerOstream& operator << < LIST_T >( PerOstream& ostr, const className< LIST_T >* pOb )	\
	PER_POINTER_WRITE( className ## STRING_T )	    				            \
	template < > PerIstream& operator >> < LIST_T >( PerIstream& istr, className< LIST_T >*& pOb )	    \
	PER_POINTER_READ( className ## STRING_T, (className< LIST_T >::perCreate())  )			\
    void perRead ## className ## STRING_T( PerIstream& istr )                   \
    {                                                                           \
        istr >> className< LIST_T >::perCreate();                               \
    }                                                                           \
    void perWrite ## className ## STRING_T( PerOstream& ostr, const void* pVoid )   \
    {                                                                           \
        const className< LIST_T >* pObject = (const className< LIST_T >*)pVoid; \
        ostr << *pObject;                                                       \
    }

#define	PER_READ_WRITE_PERSISTENT_T( className, LIST_T, STRING_T )			\
    void perWrite( PerOstream& ostr, const className< LIST_T > & ob ) \
    { perWrite< LIST_T >( ostr, ob ); } \
    void perRead( PerIstream& istr, className< LIST_T > & ob ) \
    { perRead< LIST_T >( istr, ob ); }

    //char* className< LIST_T >::perPDerivedClass( void ) const                   \
        { return _STATIC_CAST( char*, this ); }                                 \
	const char* className< LIST_T >::perClassName( void ) const		            \
	    { return className ## STRING_T; }							            \
	const char* className< LIST_T >::perMostDerivedClassName( void ) const      \
        { return this->perClassName(); }                                        \

#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, LIST_T, STRING_T )	\
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >& ob )	\
	PER_OBJECT_WRITE( className )											    \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >& ob )	        \
	PER_OBJECT_READ( className )												            \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >* pOb )	\
	PER_POINTER_WRITE( className ## STRING_T ) 					            \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >*& pOb )	    \
	PER_POINTER_READ_ABSTRACT( className< LIST_T > )						\
	char* className< LIST_T >::perPDerivedClass( void ) const           \
        { return _STATIC_CAST( char*, this ); }                                 \
	const char* className< LIST_T >::perClassName( void ) const		            \
	    { return #className ## #STRING_T; }							            \
	const char* className< LIST_T >::perMostDerivedClassName( void ) const  \
        { return this->perClassName(); }

#define	PER_PRIVATE_DEFINE_PERSISTENT_INLINE_T( className, CLASS_LIST_T, LIST_T )			\
    template< CLASS_LIST_T >                                                    \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >& ob )	\
	PER_OBJECT_WRITE( className )											    \
    template< CLASS_LIST_T >                                                    \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >& ob )	    \
	PER_OBJECT_READ( className )												\
    template< CLASS_LIST_T >                                                    \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >* pOb )	\
	PER_POINTER_WRITE( className )												\
    template< CLASS_LIST_T >                                                    \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >*& pOb )	    \
	PER_POINTER_READ( className, (className< LIST_T >::perCreate()) )			\
    template< CLASS_LIST_T >                                                    \
    inline                                                                      \
	char* className< LIST_T >::perPDerivedClass( void ) const           \
/*        { return _STATIC_CAST( char*, this ); }*/                                 \
        { return _CONST_CAST( char*, _REINTERPRET_CAST( const char*, this ) ); }                                 \
    template< CLASS_LIST_T >                                                    \
    inline                                                                      \
	const char* className< LIST_T >::perMostDerivedClassName( void ) const  \
        {  return this->perClassName(); }                                        \
    template< CLASS_LIST_T >                                                    \
    inline                                                                      \
	const char* className< LIST_T >::perClassName( void ) 		            \
	    { return #className; }

#define PER_PRIVATE_READ_WRITE_T( className, CLASS_LIST_T, LIST_T )     \
    template< CLASS_LIST_T >                                            \
    void perWrite( PerOstream&, const className< LIST_T >& );           \
    template< CLASS_LIST_T >                                            \
    void perRead( PerIstream&, className< LIST_T >& );

#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T( className, LIST_T, STRING_T )         \
    Persistence::instance().registerDerivedClass( className< LIST_T >::perClassName(), perWrite ## className ## STRING_T, perRead ## className ## STRING_T )


#define	PER_PRIVATE_DECLARE_PERSISTENT_T1( className )      PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T1, LIST_T1 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_T1( className, T1 )   PER_PRIVATE_DEFINE_PERSISTENT_T( className, T1, T1 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T1( className, T1 )   PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, T1, #T1 )
// #define PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T1( className ) PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T( className, CLASS_LIST_T1, LIST_T1 )
// #define PER_PRIVATE_STREAM_CREATE_FUNCTION_T1( className )  PER_PRIVATE_STREAM_CREATE_FUNCTION_T( className, CLASS_LIST_T1, LIST_T1 )
#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T1( className, T1 )         PER_PRIVATE_REGISTER_DERIVED_CLASS_T( className, LIST_T1, T1 )
#define PER_PRIVATE_READ_WRITE_T1( className )              PER_PRIVATE_READ_WRITE_T( className, CLASS_LIST_T1, LIST_T1 )

#define	PER_PRIVATE_DEFINE_PERSISTENT_INLINE_T1( className )	PER_PRIVATE_DEFINE_PERSISTENT_INLINE_T( className, CLASS_LIST_T1, LIST_T1 )


#define	PER_PRIVATE_DECLARE_PERSISTENT_T2( className )      PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T2, LIST_T2 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_T2( className, T1, T2 ) PER_PRIVATE_DEFINE_PERSISTENT_T( className, T1 PER_COMMA T2, T1 ## T2 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T2( className, T1, T2 ) PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, T1 PER_COMMA T2, #T1 ## #T2 )
#define	PER_PRIVATE_READ_WRITE_PERSISTENT_T2( className, T1, T2 ) PER_READ_WRITE_PERSISTENT_T( className, T1 PER_COMMA T2, T1 ## T2 )
// #define PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T2( className ) PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T( className, CLASS_LIST_T2, LIST_T2 )
// #define PER_PRIVATE_STREAM_CREATE_FUNCTION_T2( className )  PER_PRIVATE_STREAM_CREATE_FUNCTION_T( className, CLASS_LIST_T2, LIST_T2 )
#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T2( className, T1, T2 ) PER_PRIVATE_REGISTER_DERIVED_CLASS_T( className, T1 PER_COMMA T2, T1 ## T2 )
#define PER_PRIVATE_READ_WRITE_T2( className )              PER_PRIVATE_READ_WRITE_T( className, CLASS_LIST_T2, LIST_T2 )

#define	PER_PRIVATE_DECLARE_PERSISTENT_T3( className )      PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T3, LIST_T3 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_T3( className, T1, T2, T3 ) PER_PRIVATE_DEFINE_PERSISTENT_T( className, T1 PER_COMMA T2 PER_COMMA T3, #T1 ## #T2 ## #T3 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T3( className, T1, T2, T3 ) PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, T1 PER_COMMA T2 PER_COMMA T3, #T1 ## #T2 ## #T3 )
// #define PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T3( className ) PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T( className, CLASS_LIST_T3, LIST_T3 )
// #define PER_PRIVATE_STREAM_CREATE_FUNCTION_T3( className )  PER_PRIVATE_STREAM_CREATE_FUNCTION_T( className, CLASS_LIST_T3, LIST_T3 )
#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T3( className, T1, T2, T3 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3 >::perClassName(), NULL )

#define	PER_PRIVATE_DECLARE_PERSISTENT_T4( className )      PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T4, LIST_T4 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_T4( className, T1, T2, T3, T4 ) PER_PRIVATE_DEFINE_PERSISTENT_T( className, T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4, #T1 ## #T2 ## #T3 ## #T4 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T4( className, T1, T2, T3, T4 ) PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4, #T1 ## #T2 ## #T3 ## #T4 )
// #define PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T4( className ) PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T( className, CLASS_LIST_T4, LIST_T4 )
// #define PER_PRIVATE_STREAM_CREATE_FUNCTION_T4( className )  PER_PRIVATE_STREAM_CREATE_FUNCTION_T( className, CLASS_LIST_T4, LIST_T4 )
#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T4( className, T1, T2, T3, T4 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4 >::perClassName(), NULL )

#define	PER_PRIVATE_DECLARE_PERSISTENT_T5( className )      PER_PRIVATE_DECLARE_PERSISTENT_T( className, CLASS_LIST_T5, LIST_T5 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_T5( className, T1, T2, T3, T4, T5 ) PER_PRIVATE_DEFINE_PERSISTENT_T( className, T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5, #T1 ## #T2 ## #T3 ## #T4 ## #T5 )
#define	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T5( className, T1, T2, T3, T4, T5 ) PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T( className, T1 PER_COMMA T2 PER_COMMA T3 PER_COMMA T4 PER_COMMA T5, #T1 ## #T2 ## #T3 ## #T4 ## #T5 )
// #define PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T5( className ) PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T( className, CLASS_LIST_T5, LIST_T5 )
// #define PER_PRIVATE_STREAM_CREATE_FUNCTION_T5( className )  PER_PRIVATE_STREAM_CREATE_FUNCTION_T( className, CLASS_LIST_T5, LIST_T5 )
#define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T5( className, T1, T2, T3, T4, T5 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5 >::perClassName(), NULL )



// #define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T6( className, T1, T2, T3, T4, T5, T6 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5, T6 >::perClassName(), &className< T1, T2, T3, T4, T5, T6 >::perCreateFromFile )
// #define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T7( className, T1, T2, T3, T4, T5, T6, T7 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5, T6, T7 >::perClassName(), &className< T1, T2, T3, T4, T5, T6, T7 >::perCreateFromFile )
// #define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5, T6, T7, T8 >::perClassName(), &className< T1, T2, T3, T4, T5, T6, T7, T8 >::perCreateFromFile )
// #define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5, T6, T7, T8, T9 >::perClassName(), &className< T1, T2, T3, T4, T5, T6, T7, T8, T9 >::perCreateFromFile )
// #define	PER_PRIVATE_REGISTER_DERIVED_CLASS_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 ) Persistence::instance().registerDerivedClass( className< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >::perClassName(), &className< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >::perCreateFromFile )



//  I have been unable to get pointer read / write working correctly
//  so it is not implemented at present.

#define	PER_PRIVATE_DEFINE_NON_INTRUSIVE_PERSISTENT_T( className, CLASS_LIST_T, LIST_T )			\
    template< CLASS_LIST_T >                                                    \
	PerOstream& operator <<( PerOstream& ostr, const className< LIST_T >& ob )	\
	PER_OBJECT_WRITE( className )											    \
    template< CLASS_LIST_T >                                                    \
	PerIstream& operator >>( PerIstream& istr, className< LIST_T >& ob )	    \
	PER_OBJECT_READ( className )


#define PER_PRIVATE_DEFINE_NON_INTRUSIVE_PERSISTENT_T2( className ) \
        PER_PRIVATE_DEFINE_NON_INTRUSIVE_PERSISTENT_T( className, CLASS_LIST_T2, LIST_T2 )


PerOstream& operator <<( PerOstream& ostr, const               bool& val );
PerOstream& operator <<( PerOstream& ostr, const               char& val );
PerOstream& operator <<( PerOstream& ostr, const      signed   char& val );
PerOstream& operator <<( PerOstream& ostr, const    unsigned   char& val );
PerOstream& operator <<( PerOstream& ostr, const      signed  short& val );
PerOstream& operator <<( PerOstream& ostr, const    unsigned  short& val );
PerOstream& operator <<( PerOstream& ostr, const      signed    int& val );
PerOstream& operator <<( PerOstream& ostr, const    unsigned    int& val );
PerOstream& operator <<( PerOstream& ostr, const      signed   long& val );
PerOstream& operator <<( PerOstream& ostr, const    unsigned   long& val );
PerOstream& operator <<( PerOstream& ostr, const unsigned long long& val );
PerOstream& operator <<( PerOstream& ostr, const              float& val );
PerOstream& operator <<( PerOstream& ostr, const             double& val );
PerOstream& operator <<( PerOstream& ostr, const        long double& val );

PerOstream& operator <<( PerOstream& ostr, const               bool* pVal );
PerOstream& operator <<( PerOstream& ostr, const               char* pVal );
PerOstream& operator <<( PerOstream& ostr, const      signed   char* pVal );
PerOstream& operator <<( PerOstream& ostr, const    unsigned   char* pVal );
PerOstream& operator <<( PerOstream& ostr, const      signed  short* pVal );
PerOstream& operator <<( PerOstream& ostr, const    unsigned  short* pVal );
PerOstream& operator <<( PerOstream& ostr, const      signed    int* pVal );
PerOstream& operator <<( PerOstream& ostr, const    unsigned    int* pVal );
PerOstream& operator <<( PerOstream& ostr, const      signed   long* pVal );
PerOstream& operator <<( PerOstream& ostr, const    unsigned   long* pVal );
PerOstream& operator <<( PerOstream& ostr, const unsigned long long* pVal );
PerOstream& operator <<( PerOstream& ostr, const              float* pVal );
PerOstream& operator <<( PerOstream& ostr, const             double* pVal );
PerOstream& operator <<( PerOstream& ostr, const        long double* pVal );

PerIstream& operator >>( PerIstream& istr,               bool& val );
PerIstream& operator >>( PerIstream& istr,               char& val );
PerIstream& operator >>( PerIstream& istr,      signed   char& val );
PerIstream& operator >>( PerIstream& istr,    unsigned   char& val );
PerIstream& operator >>( PerIstream& istr,      signed  short& val );
PerIstream& operator >>( PerIstream& istr,    unsigned  short& val );
PerIstream& operator >>( PerIstream& istr,      signed    int& val );
PerIstream& operator >>( PerIstream& istr,    unsigned    int& val );
PerIstream& operator >>( PerIstream& istr,      signed   long& val );
PerIstream& operator >>( PerIstream& istr,    unsigned   long& val );
PerIstream& operator >>( PerIstream& istr, unsigned long long& val );
PerIstream& operator >>( PerIstream& istr,              float& val );
PerIstream& operator >>( PerIstream& istr,             double& val );
PerIstream& operator >>( PerIstream& istr,        long double& val );

PerIstream& operator >>( PerIstream& istr,               bool*& pVal );
PerIstream& operator >>( PerIstream& istr,               char*& pVal );
PerIstream& operator >>( PerIstream& istr,      signed   char*& pVal );
PerIstream& operator >>( PerIstream& istr,    unsigned   char*& pVal );
PerIstream& operator >>( PerIstream& istr,      signed  short*& pVal );
PerIstream& operator >>( PerIstream& istr,    unsigned  short*& pVal );
PerIstream& operator >>( PerIstream& istr,      signed    int*& pVal );
PerIstream& operator >>( PerIstream& istr,    unsigned    int*& pVal );
PerIstream& operator >>( PerIstream& istr,      signed   long*& pVal );
PerIstream& operator >>( PerIstream& istr,    unsigned   long*& pVal );
PerIstream& operator >>( PerIstream& istr, unsigned long long*& pVal );
PerIstream& operator >>( PerIstream& istr,              float*& pVal );
PerIstream& operator >>( PerIstream& istr,             double*& pVal );
PerIstream& operator >>( PerIstream& istr,        long double*& pVal );

void perWrite( PerOstream& ostr,   const             bool& val );
void perRead( PerIstream& ostr,                bool& val );
void perWrite( PerOstream& ostr,   const             char& val );
void perRead( PerIstream& ostr,                char& val );
void perWrite( PerOstream& ostr,   const    signed   char& val );
void perRead( PerIstream& ostr,       signed   char& val );
void perWrite( PerOstream& ostr,   const  unsigned   char& val );
void perRead( PerIstream& ostr,     unsigned   char& val );
void perWrite( PerOstream& ostr,   const    signed  short& val );
void perRead( PerIstream& ostr,       signed  short& val );
void perWrite( PerOstream& ostr,   const  unsigned  short& val );
void perRead( PerIstream& ostr,     unsigned  short& val );
void perWrite( PerOstream& ostr,   const    signed    int& val );
void perRead( PerIstream& ostr,       signed    int& val );
void perWrite( PerOstream& ostr,   const  unsigned    int& val );
void perRead( PerIstream& ostr,     unsigned    int& val );
void perWrite( PerOstream& ostr,   const    signed   long& val );
void perRead( PerIstream& ostr,       signed   long& val );
void perWrite( PerOstream& ostr,   const  unsigned   long& val );
void perRead( PerIstream& ostr,     unsigned   long& val );
void perWrite( PerOstream& ostr, const unsigned long long& val );
void perRead( PerIstream& ostr,  unsigned long long& val );
void perWrite( PerOstream& ostr,   const            float& val );
void perRead( PerIstream& ostr,               float& val );
void perWrite( PerOstream& ostr,   const           double& val );
void perRead( PerIstream& ostr,              double& val );
void perWrite( PerOstream& ostr,   const      long double& val );
void perRead( PerIstream& ostr,         long double& val );


#endif

/* End PERSIST.HPP ******************************************************/

