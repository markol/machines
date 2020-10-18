/*
 * P R E P O S T . H P P
 * (c) Charybdis Limited, 1995,1996. All Rights Reserved.
 */

#ifndef _PREPOST_HPP
#define _PREPOST_HPP

#include <sstream>
#include "stdlib/exceptio.hpp"

//  Invariants are turned on by default in the debug version

#ifndef _NO_TEST_INVARIANTS
    #ifndef NDEBUG
        #define _TEST_INVARIANTS
    #endif
#endif

#ifdef  NDEBUG
    #define PRE( exp )
    #define POST( exp )
    #define PRE_DATA( exp )
    #define POST_DATA( exp )
    #define PRE_INFO( exp )
    #define POST_INFO( exp )
    #define ASSERT_INFO( exp )

    #define ASSERT( exp, xmsg )
    #define ASSERT_DATA( exp )
    #define ASSERT_FAIL( xmsg )
    #define ASSERT_BAD_CASE
    #define ASSERT_BAD_CASE_INFO( exp )
    #define DEFAULT_ASSERT_BAD_CASE( exp )

    #define INVARIANT_INFO( exp )
    #define INVARIANT( exp )
    #define TEST_INVARIANT
    #define CLASS_INVARIANT    invariant( void ) const
    #define ASSERT_FILE_EXISTS( f )
#else

    ///////////////////////////////

    class BaseAssertion
    {
    public:

	    static	void preconditionFail( const char* exprStr, const char* file, const char* line );
    	static	void postconditionFail( const char* exprStr, const char* file, const char* line );
		static	void assertFileExists( const char* fileName, const char* file, const char* line );
		static	void assertFail( const char *exp, const xmsg& );
        static	void invariantFail( const char* exprStr, const char* file, const char* line,
            const char* calledFromFile, const char* calledFromLine );

        static  bool    validFileName( const char* fileName );

        //  True iff we are currently in an ASSERT, PRE or POST
        static  bool inAssertion();
        //  True iff we are currently in an ASSERT_INFO, PRE_INFO or POST_INFO
        static  bool inAssertionInfo();
        //  True iff we are currently in an ASSERT_DATA, PRE_DATA or POST_DATA
        //  (Not yet propely implemented - always returns false)
        static  bool inAssertionData();

		enum	InfoType { PRE_TYPE, POST_TYPE, ASSERT_TYPE, INVARIANT_TYPE };

		static	size_t	nextIndex( void );

	    static	void    logFilePositionInfo( size_t index, const char* file, size_t line, InfoType type );
		static	std::ostringstream&	str( size_t index );

		static	void	writeLoggedInfo( std::ostream& o );

        ////////////////////////////////

        class AssertionLock
        {
        public:

        	static AssertionLock& instance();

        	void lock();
        	void unlock();

        	bool isLocked() const;

            void lockInvariant();
            void unlockInvariant();

            bool invariantLocked() const;

        private:

        	bool isLocked_;
            bool invariantLocked_;

            AssertionLock();

        	// revoked...
            AssertionLock( const AssertionLock& );
            AssertionLock& operator =( const AssertionLock& );
        };

        ////////////////////////////////

        class AssertionInfo
        {
        public:

            AssertionInfo();

        private:

        	// revoked...
            AssertionInfo( const AssertionInfo& );
            AssertionInfo& operator =( const AssertionInfo& );
        };

        enum AssertionAction { ASSERT_FAIL, ASSERT_IGNORE };

        typedef AssertionAction (*AssertionAction_fn_AssertionInfo)( const AssertionInfo& );

        static  void set_assertion_handler( AssertionAction_fn_AssertionInfo );

    private:

		class InfoEntry
		{
		public:
			InfoEntry( void );

			std::ostringstream& str( void );
		    void    logFilePositionInfo( const char* file, size_t line, InfoType type );

			bool	containsData( void ) const;

			friend std::ostream& operator <<( std::ostream&, const BaseAssertion::InfoEntry& );

		private:
			enum { MAX_DATA_LENGTH = 200 };
			enum { MAX_FILENAME_LENGTH = 200 };

			std::ostringstream	stream_;

			char	dataBuffer_[ MAX_DATA_LENGTH ];
			char	fileNameBuffer_[ MAX_FILENAME_LENGTH ];
			size_t	lineNumber_;
			InfoType	type_;
			bool	containsData_;

    		// revoked...
        	InfoEntry( const InfoEntry& );
	        InfoEntry& operator =( const InfoEntry& );
		};

		enum { N_SAVED_ENTRIES = 50 };

		static	size_t	index_;
		static	InfoEntry	info_[ N_SAVED_ENTRIES ];

        static  AssertionAction_fn_AssertionInfo    assertionHandler_;

    	// revoked...
		BaseAssertion( void );
        BaseAssertion( const BaseAssertion& );
        BaseAssertion& operator =( const BaseAssertion& );

        friend std::ostream& operator <<( std::ostream&, const BaseAssertion::InfoEntry& );

    };

    #include "base/private/trace.hpp"

    ///////////////////////////////

    #define LOG_INFO( exp, type )                         					\
    {                                                   					\
		size_t	nextIndex = BaseAssertion::nextIndex();						\
        BaseAssertion::str( nextIndex ) << #exp << " :\t" << exp << '\0';  \
        BaseAssertion::logFilePositionInfo( nextIndex, __FILE__, __LINE__, type );\
    }

    #define PRE_INFO( exp )		LOG_INFO( exp, BaseAssertion::PRE_TYPE )
    #define POST_INFO( exp )	LOG_INFO( exp, BaseAssertion::POST_TYPE )
    #define ASSERT_INFO( exp )	LOG_INFO( exp, BaseAssertion::ASSERT_TYPE )
    #define INVARIANT_INFO( exp )	LOG_INFO( exp, BaseAssertion::INVARIANT_TYPE )


	#define	ASSERT_EXPRESSION( exp, fn ) {					                \
            if( BaseTrace::instance().trace() )                             \
                BaseTrace::instance().log( __FILE__, __LINE__ );            \
			if( not BaseAssertion::AssertionLock::instance().isLocked() )	\
			{	BaseAssertion::AssertionLock::instance().lock();			\
				(exp) ? BaseAssertion::AssertionLock::instance().unlock()	\
						: fn; } }

    #define PRE( exp )	ASSERT_EXPRESSION( exp, BaseAssertion::preconditionFail( #exp, __FILE__, _STR( __LINE__ ) ) )
    #define POST( exp )	ASSERT_EXPRESSION( exp, BaseAssertion::postconditionFail( #exp, __FILE__, _STR( __LINE__ ) ) )

    #define PRE_DATA( exp )     exp
    #define POST_DATA( exp )    exp

	////////////////////////////////////////////////////////////

    #define ASSERT_FILE_EXISTS( f ) \
        BaseAssertion::AssertionLock::instance().isLocked() ? (void)0 : BaseAssertion::assertFileExists( f, __FILE__, _STR( __LINE__ ) )

	////////////////////////////////////////////////////////////

    #define ASSERT( exp, xmsg )		\
    	ASSERT_EXPRESSION( exp, BaseAssertion::assertFail( __FILE__ ":" _STR( __LINE__ ) " " #exp, xmsg ) )

    #define ASSERT_DATA( exp )      exp

    #define ASSERT_FAIL( xmsg )		ASSERT( false, xmsg )

    #define ASSERT_BAD_CASE         ASSERT( false, logic_error( "bad case" ) )

    #define ASSERT_BAD_CASE_INFO( exp )         { ASSERT_INFO( exp ); ASSERT( false, logic_error( "bad case" ) ) }

    #define DEFAULT_ASSERT_BAD_CASE( exp )  default: ASSERT_INFO( exp ); ASSERT( false, logic_error( "bad case" ) ); break;

	////////////////////////////////////////////////////////////

    #ifdef _TEST_INVARIANTS

    	#define INVARIANT( exp )	\
	    	ASSERT_EXPRESSION( exp, BaseAssertion::invariantFail( #exp, __FILE__, _STR( __LINE__ ), file, line ) )

        #define TEST_INVARIANT {     						                            \
            if( DiagInternal::checkInvariants_ )                                        \
            {                                                                           \
                if( BaseTrace::instance().trace() )                                     \
                    BaseTrace::instance().log( __FILE__, __LINE__ );                    \
    			if( not BaseAssertion::AssertionLock::instance().invariantLocked() )	\
    			{	BaseAssertion::AssertionLock::instance().lockInvariant();			\
    				invariant( __FILE__, _STR( __LINE__ ) );	                        \
    				BaseAssertion::AssertionLock::instance().unlockInvariant();			\
    			}                                                                       \
                }                                                                       \
            }

        #define CLASS_INVARIANT    invariant( const char *file, const char* line ) const

    #else

        #define INVARIANT( exp )

        #define TEST_INVARIANT {                                            \
            if( BaseTrace::instance().trace() )                             \
                BaseTrace::instance().log( __FILE__, __LINE__ );            \
            }

        #define CLASS_INVARIANT    invariant( void ) const

    #endif

#endif      //  NDEBUG

//  Because this is a compile time check it is included in both
//  debug and release versions of the code. It will only work
//  on expressions that are evaluated at compile time ( usually
//  integer constants with some simple maths +, * etc. )
//  Note that the error message given is:
//      Error! E034: (col 58) dimension cannot be zero

#define ASSERT_COMPILE_TIME( EXP ) ( (void)sizeof( char [ ((EXP) != 0) ] ) )


#endif // _PREPOST_HPP

/* End PREPOST.HPP ***************************************************/
