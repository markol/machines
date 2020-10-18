/*
 * M E M C D A T A . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#include "base/base.hpp"
#include "base/memcdata.hpp"

DbgMemChkData::DbgMemChkData()
{
}

DbgMemChkData::~DbgMemChkData()
{
}

size_t  DbgMemChkData::nNewCalls( void ) const
{
    return nNewCalls_;
}

void    DbgMemChkData::nNewCalls( size_t newNNewCalls )
{
    nNewCalls_ = newNNewCalls;
}

size_t  DbgMemChkData::nDeleteCalls( void ) const
{
    return nDeleteCalls_;
}

void    DbgMemChkData::nDeleteCalls( size_t newNDeleteCalls )
{
    nDeleteCalls_ = newNDeleteCalls;
}

size_t  DbgMemChkData::nNewArrayCalls( void ) const
{
    return nNewArrayCalls_;
}

void    DbgMemChkData::nNewArrayCalls( size_t newNNewArrayCalls )
{
    nNewArrayCalls_ = newNNewArrayCalls;
}

size_t  DbgMemChkData::nDeleteArrayCalls( void ) const
{
    return nDeleteArrayCalls_;
}

void    DbgMemChkData::nDeleteArrayCalls( size_t newNDeleteArrayCalls )
{
    nDeleteArrayCalls_ = newNDeleteArrayCalls;
}

size_t  DbgMemChkData::totalMemoryNewed( void ) const
{
    return totalMemoryNewed_;
}

void    DbgMemChkData::totalMemoryNewed( size_t newTotalMemoryNewed )
{
    totalMemoryNewed_ = newTotalMemoryNewed;
}

size_t  DbgMemChkData::totalMemoryDeleted( void ) const
{
    return totalMemoryDeleted_;
}

void    DbgMemChkData::totalMemoryDeleted( size_t newTotalMemoryDeleted )
{
    totalMemoryDeleted_ = newTotalMemoryDeleted;
}

size_t  DbgMemChkData::maxMemoryNewed( void ) const
{
    return maxMemoryNewed_;
}

void    DbgMemChkData::maxMemoryNewed( size_t newMaxMemoryNewed )
{
    maxMemoryNewed_ = newMaxMemoryNewed;
}

int32  DbgMemChkData::nOutstandingNewCalls( void ) const
{
    return  (int32)nNewCalls_ - (int32)nDeleteCalls_;
}

int32  DbgMemChkData::nOutstandingNewArrayCalls( void ) const
{
    return  (int32)nNewArrayCalls_ - (int32)nDeleteArrayCalls_;
}

int32   DbgMemChkData::currentMemoryNewed( void ) const
{
    return  (int32)totalMemoryNewed_ - (int32)totalMemoryDeleted_;
}

ostream& operator <<( ostream& o, const DbgMemChkData& t )
{
    o << "...................................................................." << std::endl;
    o << t.nNewCalls() << " NEWs, " << t.nDeleteCalls() << " DELETEs ( " << (int32)t.nNewCalls() - (int32)t.nDeleteCalls() << " outstanding NEWs )" << std::endl;
    o << t.nNewArrayCalls() << " NEW_ARRAYs, " << t.nDeleteArrayCalls() << " DELETE_ARRAYs ( " << (int32)t.nNewArrayCalls() - (int32)t.nDeleteArrayCalls() << " outstanding NEW_ARRAYs )" << std::endl;
    o << t.totalMemoryNewed() << " bytes newed in total, " << (int32)t.totalMemoryDeleted() << " bytes deleted in total " << std::endl;
    o << t.totalMemoryNewed() - t.totalMemoryDeleted() << " bytes currently newed, " << t.maxMemoryNewed() << " bytes maximum newed" << std::endl;
    o << "...................................................................." << std::endl;

    return o;
}

/* End MEMCDATA.CPP *************************************************/
