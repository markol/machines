//	Internal specialist memory functions

#ifndef _BASE_INTERNAL_MEMCHK_HPP
#define _BASE_INTERNAL_MEMCHK_HPP

#ifndef NDEBUG

    void DbgMemChkSaveDebugInfo( bool saveDebug );

    //	These macros bypass the usual logging on memory. They are useful when
    //  the act of logging causes problems in itself - particularly for classes
    //  that are used to handle the logging streams.

    #define _NEW_WITHOUT_LOG(t)   			\
    	(DbgMemChkSaveDebugInfo( false ),	\
    	new t)

    #define	_DELETE_WITHOUT_LOG(t) 			\
    	(DbgMemChkSaveDebugInfo( false ),	\
    	delete t)

#else

	#define	_NEW_WITHOUT_LOG(t)	            new t
    
	#define	_DELETE_WITHOUT_LOG(t)          delete t

#endif

#endif