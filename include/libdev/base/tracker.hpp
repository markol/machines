/*
 * T R A C K E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DiagPointerTracker

    Allow pointers to be tracked. Useful for checking that
    objects are only deleted once.
    
    Supply a set of macros to allow class construction / destruction
    to be easily tracked.
*/

#ifndef _BASE_TRACKER_HPP
#define _BASE_TRACKER_HPP

#include "base/base.hpp"

class   DiagPointerTrackerImpl;

class DiagPointerTracker
{
public:
    DiagPointerTracker();
    ~DiagPointerTracker();

    void    addPointer( void* ptr );
    bool    pointerPresent( void* ptr ) const;
    void    removePointer( void* ptr );
    //  PRE( pointerPresent( ptr ) );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const DiagPointerTracker& t );

private:
    // Operation deliberately revoked
    DiagPointerTracker( const DiagPointerTracker& );

    // Operation deliberately revoked
    DiagPointerTracker& operator =( const DiagPointerTracker& );

    // Operation deliberately revoked
    bool operator ==( const DiagPointerTracker& );

    DiagPointerTrackerImpl*    pImplementation_;
};

//================= MACROS FOR TRACKING CLASSES ================================

#ifndef NDEBUG

    #include "base/private/tracker.hpp"
    
    //  Put this in the private section of the tracked class declaration in
    //  the .hpp file
    #define OBJECT_TRACKER( CLASS ) PRIVATE_OBJECT_TRACKER( CLASS )

    //  Put this at the start of every constructor for the class being tracked.
    #define LOG_CONSTRUCTION        PRIVATE_LOG_CONSTRUCTION

    //  Put this at the end of the destructor for the class being tracked.
    #define LOG_DESTRUCTION         PRIVATE_LOG_DESTRUCTION

    //  Put this in the .hpp file but outside the class declaration. Use
    //  the appropriate macro for the number of template arguments.
    #define EXISTS( CLASS )         PRIVATE_EXISTS( CLASS )
    #define EXISTS_T1( CLASS )      PRIVATE_EXISTS_T1( CLASS )
    #define EXISTS_T2( CLASS )      PRIVATE_EXISTS_T2( CLASS )
    #define EXISTS_T3( CLASS )      PRIVATE_EXISTS_T3( CLASS )
    #define EXISTS_T4( CLASS )      PRIVATE_EXISTS_T4( CLASS )
    #define EXISTS_T5( CLASS )      PRIVATE_EXISTS_T5( CLASS )
    #define EXISTS_T6( CLASS )      PRIVATE_EXISTS_T6( CLASS )
    #define EXISTS_T7( CLASS )      PRIVATE_EXISTS_T7( CLASS )
    #define EXISTS_T8( CLASS )      PRIVATE_EXISTS_T8( CLASS )
    #define EXISTS_T9( CLASS )      PRIVATE_EXISTS_T9( CLASS )
    #define EXISTS_T10( CLASS )     PRIVATE_EXISTS_T10( CLASS )

    //  Use this to check if a pointer or reference to an object of a
    //  tracked class is valid (i.e. the object still exists)
    #define ASSERT_EXISTS( VAL )    PRIVATE_ASSERT_EXISTS( VAL )
    
    //  Put this into member functions of a tracked class
    #define ASSERT_THIS_EXISTS      PRIVATE_ASSERT_THIS_EXISTS

    //  Put this into the invariant of the tracked class
    #define INVARIANT_THIS_EXISTS   PRIVATE_INVARIANT_THIS_EXISTS

#else

    #define OBJECT_TRACKER( CLASS )
    #define LOG_CONSTRUCTION
    #define LOG_DESTRUCTION
    #define EXISTS( CLASS )
    #define EXISTS_T1( CLASS )
    #define EXISTS_T2( CLASS )
    #define EXISTS_T3( CLASS )
    #define EXISTS_T4( CLASS )
    #define EXISTS_T5( CLASS )
    #define EXISTS_T6( CLASS )
    #define EXISTS_T7( CLASS )
    #define EXISTS_T8( CLASS )
    #define EXISTS_T9( CLASS )
    #define EXISTS_T10( CLASS )
    #define ASSERT_EXISTS( VAL )
    #define ASSERT_THIS_EXISTS
    #define INVARIANT_THIS_EXISTS

#endif

#endif

/* End TRACKER.HPP **************************************************/

