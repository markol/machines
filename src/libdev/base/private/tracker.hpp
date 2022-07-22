/*
 * T R A C K E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	Private implementation file for the tracking macros. Used just
	to keep the public file tidy.
*/

#ifndef _BASE_PRIVATE_TRACKER_HPP
#define _BASE_PRIVATE_TRACKER_HPP

#include "base/base.hpp"
#include "base/private/diagint.hpp"
#include <cstring>

#define PRIVATE_OBJECT_TRACKER( CLASS )                                         \
    static DiagPointerTracker& diagTracker()                            \
    {                                                                   \
        static DiagPointerTracker tracker_;                             \
        return tracker_;                                                \
    }                                                                   \
    static bool doTrackingActive()                                      \
    {                                                                   \
        bool    result = false;                                         \
        const char* pStr = Diag::instance().getenv( "cb_" #CLASS "_tracking" );                \
        if( pStr )                                                      \
        {                                                               \
            /*if( stricmp( pStr, "active" ) == 0 )*/                        \
            if( strcasecmp( pStr, "active" ) == 0 )                        \
            {                                                           \
                result = true;                                          \
            }                                                           \
        }                                                               \
        return result;                                                  \
    }                                                                   \
    static bool diagTrackingActive()                                    \
    {                                                                   \
        static bool trackingActive_ = doTrackingActive() || DiagInternal::trackAllObjects_;               \
        return trackingActive_;                                         \
    }                                                                   \
    friend bool exists( const CLASS* )


#define PRIVATE_LOG_CONSTRUCTION                                                \
    if( diagTrackingActive() )             \
    {                                                                   \
        ASSERT_INFO( _STATIC_CAST( void*, this ) );                     \
        ASSERT( not diagTracker().pointerPresent( _STATIC_CAST( void*, this ) ), "Attempt to construct object on top of existing object" );\
        diagTracker().addPointer( _STATIC_CAST( void*, this ) );    \
    }

#define PRIVATE_LOG_DESTRUCTION                                                 \
    if( diagTrackingActive() )             \
    {                                                                   \
        ASSERT_INFO( _STATIC_CAST( void*, this ) );                     \
        ASSERT( diagTracker().pointerPresent( _STATIC_CAST( void*, this ) ), "Object destroyed twice" );    \
        diagTracker().removePointer( _STATIC_CAST( void*, this ) );     \
    }

#define PRIVATE_EXISTS( CLASS )                                                 \
    bool exists( const CLASS* ptr )                              \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _CONST_CAST( void*, _REINTERPRET_CAST( const void*, ptr ) ) );                      \
        return CLASS::diagTracker().pointerPresent(_CONST_CAST( void*, _REINTERPRET_CAST( const void*, ptr ) ) );   \
    }                                                                   \
    bool exists( const CLASS& ref )                              \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T1( CLASS )                                            \
    template< class X1 >                                                 \
    static bool exists( const CLASS< X1 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1 >                                                 \
    static bool exists( const CLASS< X1 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T2( CLASS )                                            \
    template< class X1, class X2 >                                                 \
    static bool exists( const CLASS< X1, X2 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2 >                                                 \
    static bool exists( const CLASS< X1, X2 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T3( CLASS )                                            \
    template< class X1, class X2, class X3 >                                                 \
    static bool exists( const CLASS< X1, X2, X3 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3 >                                                 \
    static bool exists( const CLASS< X1, X2, X3 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T4( CLASS )                                            \
    template< class X1, class X2, class X3, class X4 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T5( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T6( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5, class X6 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5, X6 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5, class X6 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T7( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5, X6, X7 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T8( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5, X6, X7, X8 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T9( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8, class X9 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8, class X9 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_EXISTS_T10( CLASS )                                            \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8, class X9, class X10 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9, X10 >* ptr )                         \
    {                                                                   \
        if( not ptr->diagTrackingActive() )                             \
            return true;                                                \
        ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      \
        return CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9, X10 >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   \
    }                                                                   \
    template< class X1, class X2, class X3, class X4, class X5, class X6, class X7, class X8, class X9, class X10 >                                                 \
    static bool exists( const CLASS< X1, X2, X3, X4, X5, X6, X7, X8, X9, X10 >& ref )                         \
    {                                                                   \
        return ::exists( &ref );                                        \
    }

#define PRIVATE_ASSERT_EXISTS( VAL )                                            \
    ASSERT( ::exists( VAL ), "Object should exist and doesn't" );   \

#define PRIVATE_ASSERT_THIS_EXISTS                                              \
    if( diagTrackingActive() )             \
    {                                                                   \
        ASSERT_EXISTS( this );                                          \
    }

#define PRIVATE_INVARIANT_THIS_EXISTS                                              \
    if( diagTrackingActive() )             \
    {                                                                   \
        INVARIANT( ::exists( this ) );   \
    }

#endif

/* End TRACKER.HPP **************************************************/

