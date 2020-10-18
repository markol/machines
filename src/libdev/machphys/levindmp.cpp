/*
 * L E V I N D M P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/levindmp.hpp"

MachPhysLevelIndexMap::MachPhysLevelIndexMap( const size_t* pLevelArray, size_t nLevels )
: pLevelArray_( pLevelArray ),
  nLevels_( nLevels )
{

    TEST_INVARIANT;
}

MachPhysLevelIndexMap& MachPhysLevelIndexMap::operator =( const MachPhysLevelIndexMap& copyMe )
{
    pLevelArray_ = copyMe.pLevelArray_;
    nLevels_ = copyMe.nLevels_;

    return *this;
}

MachPhysLevelIndexMap::MachPhysLevelIndexMap( const MachPhysLevelIndexMap& copyMe )
: pLevelArray_( copyMe.pLevelArray_ ),
  nLevels_( copyMe.nLevels_ )
{
}

MachPhysLevelIndexMap::~MachPhysLevelIndexMap()
{
    TEST_INVARIANT;

}

size_t  MachPhysLevelIndexMap::nLevels() const
{
    return nLevels_;
}

size_t  MachPhysLevelIndexMap::level( size_t index ) const
{
    PRE( index < nLevels_ );

    return pLevelArray_[ index ];
}

size_t  MachPhysLevelIndexMap::index( size_t level ) const
{
    size_t  result = 0;

    bool    foundLevel = false;

    for( size_t i = 0; i < nLevels_ and not foundLevel; ++i )
    {
        if( level == pLevelArray_[ i ] )
        {
            result = i;
            foundLevel = true;
        }
    }

    ASSERT( foundLevel, "Level not found" );

    return result;
}

void MachPhysLevelIndexMap::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLevelIndexMap& t )
{

    o << "MachPhysLevelIndexMap " << (void*)&t << " start" << std::endl;
    o << "MachPhysLevelIndexMap " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LEVINDMP.CPP *************************************************/
