
#include "base/base.hpp"
#include "base/tracker.hpp"

class T1
{
public:
    T1()
    {
        LOG_CONSTRUCTION;
    }
    ~T1()
    {
        LOG_DESTRUCTION;
    }

private:
    OBJECT_TRACKER( T1 );
};

EXISTS( T1 );

template< class X >
class T2
{
public:
    T2()
    {
        LOG_CONSTRUCTION;
    }
    ~T2()
    {
        LOG_DESTRUCTION;
    }

private:
    OBJECT_TRACKER( T2 );
};

EXISTS_T1( T2 );

//         template< class X >                                                 
//         static bool exists( const T2< X >* ptr )                         
//         {                                                                   
//             ASSERT_INFO( _STATIC_CAST( void*, ptr ) );                      
//             return T2< X >::diagTracker().pointerPresent( _STATIC_CAST( void*, ptr ) );   
//         }                                                                   
//         template< class X >                                                 
//         static bool exists( const T2< X >& ref )                         
//         {                                                                   
//             return ::exists( &ref );                                        
//         }
