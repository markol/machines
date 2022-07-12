/*
 * D I A G S T R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DiagStream

    A diagnostic stream. This owns the actual output stream and
    deals with prepending appropriate information (date, time
    etc.) to each line of output.
*/

#ifndef _BASE_DIAGSTR_HPP
#define _BASE_DIAGSTR_HPP

#include "base/base.hpp"

#include "base/internal/append.hpp"
#include "base/internal/prepend.hpp"
#include "base/internal/windstr.hpp"

class BaseWindowStream;

class DiagStream
{
public:
//    enum StreamType { INITIAL, REPLACEMENT };

    //  Construct the basic DiagStream that acts as a sink
    DiagStream();

    //  Setup the DiagStream from the given envuironment variable. If
    //  the environment variable does not exist use the supplied
    //  default setting.
    void setup(
        const char* environmentVariable,
        const char* defaultSetting );

    ~DiagStream();

    //  Return the ostream associated with this DiagStream
    ostream&    ostr();

    //  True iff this stream is going to a file
    bool    hasDestination() const;

    //  Close any file associated with this stream
    void    close();

    void    indent( int nSpaces );

private:
    // Operation deliberately revoked
    DiagStream( const DiagStream& );

    // Operation deliberately revoked
    DiagStream& operator =( const DiagStream& );

    // Operation deliberately revoked
    bool operator ==( const DiagStream& );

    void interpretEnvironmentVariable(
        const char* environmentVariable,
        const char* defaultSetting );
    void setupStream();
    const char* name() const;

    DiagStream* pNextStream();
    void pNextStream( DiagStream* pNext );

    bool append() const;
    void forceAppendFile();

    static  DiagStream*& pFirstStream();

    ostream&    nonPrependOstr();

    std::ofstream    ostr_;
    BaseAppendOstream    appendOstr_;

    bool        exists_;

    DiagStream* pDependantStream_;
    DiagStream* pNextStream_;
    bool        append_;
    bool        cout_;
    BaseWindowStream*   pWindowStream_;

    char*       name_;

    DiagOstreamPrepend  masterOstr_;
};


#endif

/* End DIAGSTR.HPP **************************************************/
