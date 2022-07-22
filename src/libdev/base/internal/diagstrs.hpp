/*
 * D I A G S T R S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DiagStreams

    A brief description of the class should go in here
*/

#ifndef _BASE_DIAGSTRS_HPP
#define _BASE_DIAGSTRS_HPP

#include "base/base.hpp"

#include "base/internal/diagstr.hpp"

class DiagStreams
{
public:
    //  Singleton class
    static DiagStreams& instance();
    ~DiagStreams();

    //  Close all open diagnostic streams
    void    close();

    friend ostream& operator <<( ostream& o, const DiagStreams& t );

private:
    // Operation deliberately revoked
    DiagStreams( const DiagStreams& );

    // Operation deliberately revoked
    DiagStreams& operator =( const DiagStreams& );

    // Operation deliberately revoked
    bool operator ==( const DiagStreams& );

    DiagStreams();

    friend class Diag;
    friend class DiagInternal;

    bool        diagStreamEnabled_[ DIAG_N_DIAG_STREAMS ];

    DiagStream	diagStreams_[ DIAG_N_DIAG_STREAMS ];

    size_t	nStreams();
};


#endif

/* End DIAGSTRS.HPP *************************************************/
