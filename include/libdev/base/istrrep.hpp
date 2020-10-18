/*
 * I S T R R E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PerIstreamReporter

    Reports the progress that an istream is making in reading its input file.
*/

#ifndef _BASE_ISTRREP_HPP
#define _BASE_ISTRREP_HPP

#include "base/base.hpp"

class PerIstreamReporter
// Canonical form revoked
{
public:
    PerIstreamReporter( void );
    ~PerIstreamReporter( void );

    //  Override this functino to provide whatever monitoring you want.
    //  The function must return the number of bytes it wants read
    //  before it is called again.
    virtual size_t report( size_t bytesRead, size_t fileSize ) = 0;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PerIstreamReporter& t );

    PerIstreamReporter( const PerIstreamReporter& );
    PerIstreamReporter& operator =( const PerIstreamReporter& );

};

// This class is useful in situations other than just reading from a PerIstream, therefore
// a more general name has been typedefed.
typedef PerIstreamReporter BaseProgressReporter;

#endif

/* End ISTRREP.HPP **************************************************/
