/*
 * T R A C K E R I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DiagPointerTrackerImpl

    A brief description of the class should go in here
*/

#ifndef _BASE_TRACKERI_HPP
#define _BASE_TRACKERI_HPP

#include "base/base.hpp"
#include "base/lessptr.hpp"

#include "ctl/set.hpp"

class DiagPointerTrackerImpl
{
public:
    DiagPointerTrackerImpl( void );
    ~DiagPointerTrackerImpl( void );

    void    addPointer( void* ptr );
    bool    pointerPresent( void* ptr ) const;
    void    removePointer( void* ptr );
    //  PRE( pointerPresent( ptr ) );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const DiagPointerTrackerImpl& t );

private:
    // Operation deliberately revoked
    DiagPointerTrackerImpl( const DiagPointerTrackerImpl& );

    // Operation deliberately revoked
    DiagPointerTrackerImpl& operator =( const DiagPointerTrackerImpl& );

    // Operation deliberately revoked
    bool operator ==( const DiagPointerTrackerImpl& );

    ctl_set< void*, less_ptr< void > >    pointers_;
};


#endif

/* End TRACKERI.HPP *************************************************/
