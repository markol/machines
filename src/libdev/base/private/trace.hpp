/*
 * T R A C E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    BaseTrace

    A brief description of the class should go in here
*/

#ifndef _BASE_TRACE_HPP
#define _BASE_TRACE_HPP

class BaseTrace
{
public:
    //  Singleton class
    static BaseTrace& instance();

    ~BaseTrace();

    void    trace( bool );
    bool    trace() const { return trace_; }
    
    void    log( const char* fileName, size_t line );
    
private:
    // Operation deliberately revoked
    BaseTrace( const BaseTrace& );

    // Operation deliberately revoked
    BaseTrace& operator =( const BaseTrace& );

    // Operation deliberately revoked
    bool operator ==( const BaseTrace& );

    BaseTrace();
    
    bool    trace_;
};


#endif

/* End TRACE.HPP ****************************************************/
