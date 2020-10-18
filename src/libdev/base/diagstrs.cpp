/*
 * D I A G S T R S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/diagstrs.hpp"
#include "base/internal/diagstr.hpp"

// static
DiagStreams& DiagStreams::instance()
{
    static DiagStreams instance_;
    return instance_;
}

DiagStreams::DiagStreams()
{

    for( size_t i = 0; i < nStreams(); ++i )
        diagStreamEnabled_[ i ] = false;
}

DiagStreams::~DiagStreams()
{

}

size_t	DiagStreams::nStreams()
{
	return DIAG_N_DIAG_STREAMS;
}

void DiagStreams::close()
{
    for( size_t i = 0; i < nStreams(); ++i )
    {
        diagStreams_[ i ].close();
    }
}

ostream& operator <<( ostream& o, const DiagStreams& t )
{

    o << "DiagStreams " << (void*)&t << " start" << std::endl;
    o << "DiagStreams " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DIAGSTRS.CPP *************************************************/
