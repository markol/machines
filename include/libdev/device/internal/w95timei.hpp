/*
 * W 9 5 T I M E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    DevW95TimeInternal

    An internal class to handle the three different ways we might
    get the time using Windows 95
*/

#ifndef _DEVICE_W95TIMEI_HPP
#define _DEVICE_W95TIMEI_HPP

#include "base/base.hpp"

class DevW95TimeInternal
// Canonical form revoked
{
public:
    //  Singleton class
    static DevW95TimeInternal& instance();
    ~DevW95TimeInternal();

    double  resolution() const;

    enum TimeMethod
    { RDTSC, PERFORMANCE_COUNTER, TIME_GET_TIME };
    
    TimeMethod method() const;

    double pentiumTicks() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const DevW95TimeInternal& t );

    DevW95TimeInternal( const DevW95TimeInternal& );
    DevW95TimeInternal& operator =( const DevW95TimeInternal& );

    DevW95TimeInternal();

    bool rdtscAvailable() const;
    void calibrateRdtsc();

    double resolution_;
    TimeMethod method_;
    
};

#ifdef _INLINE
    #include "internal/w95timei.ipp"
#endif


#endif

/* End W95TIMEI.HPP *************************************************/
