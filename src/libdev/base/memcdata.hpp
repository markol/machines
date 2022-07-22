/*
 * M E M C D A T A . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#ifndef _MEMCDATA_HPP
#define _MEMCDATA_HPP

#include <fstream>

#include "base/base.hpp"

class DbgMemChkData
{
    //  THIS CLASS IS BITWISE CANONICAL

public:
    DbgMemChkData();
    ~DbgMemChkData();

    //  All figures for memory sizes are in bytes

    size_t  nNewCalls() const;
    void    nNewCalls( size_t newNNewCalls );

    size_t  nDeleteCalls() const;
    void    nDeleteCalls( size_t newNDeleteCalls );

    size_t  nNewArrayCalls() const;
    void    nNewArrayCalls( size_t newNNewArrayCalls );

    size_t  nDeleteArrayCalls() const;
    void    nDeleteArrayCalls( size_t newNDeleteArrayCalls );

    size_t  totalMemoryNewed() const;
    void    totalMemoryNewed( size_t newTotalMemoryNewed );

    size_t  totalMemoryDeleted() const;
    void    totalMemoryDeleted( size_t newTotalMemoryDeleted );

    size_t  maxMemoryNewed() const;
    void    maxMemoryNewed( size_t newMaxMemoryNewed );

    int32   nOutstandingNewCalls() const;
    int32   nOutstandingNewArrayCalls() const;

    int32   currentMemoryNewed() const;

    friend std::ostream& operator <<( std::ostream& o, const DbgMemChkData& t );

private:
    // Operation deliberately revoked
    bool operator ==( const DbgMemChkData& ) const;

    size_t  nNewCalls_;
    size_t  nDeleteCalls_;
    size_t  nNewArrayCalls_;
    size_t  nDeleteArrayCalls_;
    size_t  totalMemoryNewed_;
    size_t  totalMemoryDeleted_;
    size_t  maxMemoryNewed_;

};

#endif

/* End MEMCDATA.HPP *************************************************/
