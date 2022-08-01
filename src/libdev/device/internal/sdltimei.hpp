/*
    DevSdlTimeInternal

    An internal class to handle the three different ways we might
    get the time using SDL2.0
*/

#ifndef _DEVICE_SDLTIMEI_HPP
#define _DEVICE_SDLTIMEI_HPP

#include "base/base.hpp"

class DevSdlTimeInternal
// Canonical form revoked
{
public:
    //  Singleton class
    static DevSdlTimeInternal& instance(){ static DevSdlTimeInternal instance_; return instance_; };
    ~DevSdlTimeInternal();

    double  resolution() const { return resolution_; };

    enum TimeMethod
    { RDTSC, PERFORMANCE_COUNTER, TIME_GET_TIME };

    TimeMethod method() const { return method_; };

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const DevSdlTimeInternal& t );

    DevSdlTimeInternal( const DevSdlTimeInternal& );
    DevSdlTimeInternal& operator =( const DevSdlTimeInternal& );

    DevSdlTimeInternal();


    double resolution_;
    TimeMethod method_;

};


#endif

/* End W95TIMEI.HPP *************************************************/
