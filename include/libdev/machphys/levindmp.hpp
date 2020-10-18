/*
 * L E V I N D M P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysLevelIndexMap

    Perform the mapping both ways between levels and indices. Levels are
    in the range 1 - 5 and are not necessarily contiguous (a machine might
    have hardware levels 2, 3 and 5), the indices run from zero and are
    contiguous (so the aforementioned machine would have hardware indices
    0, 1 and 2)
*/

#ifndef _MACHPHYS_LEVINDMP_HPP
#define _MACHPHYS_LEVINDMP_HPP

#include "base/base.hpp"

class MachPhysLevelIndexMap
{
public:
    MachPhysLevelIndexMap& operator =( const MachPhysLevelIndexMap& );
    MachPhysLevelIndexMap( const MachPhysLevelIndexMap& );
    ~MachPhysLevelIndexMap( void );

    size_t  nLevels() const;
    
    //  Return the level corresponding to a given index
    size_t  level( size_t index ) const;
    
    //  Return the index corresponding to a given level
    size_t  index( size_t level ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLevelIndexMap& t );

private:

    // Operation deliberately revoked
    bool operator ==( const MachPhysLevelIndexMap& );

    friend class MachPhysLevels;
    MachPhysLevelIndexMap( const size_t* pLevelArray, size_t nLevels );

    const size_t*   pLevelArray_;
    size_t          nLevels_;
};


#endif

/* End LEVINDMP.HPP **************************************************/