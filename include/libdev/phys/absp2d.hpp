/*
 * A B S P 2 D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysAlignedBSPTree2d

    Binary tree partitioned over 2d space for holding pointers to objects
    as defined by parameter class.
*/

#ifndef _PHYS_ABSP2D_HPP
#define _PHYS_ABSP2D_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox2d.hpp"

//Forward declarations
class mexPoint2d;
// forward declarations
template< class P_ITEM > class PhysAlignedBSPTree2d;
template< class P_ITEM > ostream& operator <<
    ( ostream& o, const PhysAlignedBSPTree2d< P_ITEM >& t );


//The tree stores pointers to objects of class 'ITEM'
template< class P_ITEM >
class PhysAlignedBSPTree2d
{
public:
    //Useful types
    typedef ctl_vector< P_ITEM > PItems;

    //ctor. Limits of partitioned space defined by minCorner and maxCorner.
    //No partition will be created with dimension < minResolution.
    //The maximum number of items that will be stored in a partition before
    //it is subdivided is nMaxItems.
    //When a partition is subdivided, each sub-partition will extend across
    //the dividing line by lipSize, so that objects which lie across the dividing
    //line are likely to fall into one of the sub-partitions.
    PhysAlignedBSPTree2d( const MexPoint2d& minCorner, const MexPoint2d& maxCorner,
                          MATHEX_SCALAR minResolution, size_t nMaxItems,
                          MATHEX_SCALAR lipSize );
    //PRE( minCorner.x() < maxCorner.x() )
    //PRE( minCorner.y() < maxCorner.y() )
    //PRE( lipSize >= 0 )
    //PRE( minResolution > MexEpsilon::instance() )

    //dtor.
    ~PhysAlignedBSPTree2d( void );

    //Defines constraints on template for documentation - never called
    void constraints( P_ITEM pItem )
    {
        MexAlignedBox2d abox;
        pItem->boundary( &abox ); //Method to extract aligned bounding box
    };

    //Adds pItem to the tree
    void add( P_ITEM pItem );

    //Removes pItem from the tree
    void remove( P_ITEM pItem );

    //Count of items stored in the tree
    size_t nItems( void ) const;

    //Finds all the items referenced in the tree which have bounding boxes overlapping
    //boundary, and adds them to pItems.
    size_t overlapping( const MexAlignedBox2d& boundary, PItems* pItems ) const;

    void CLASS_INVARIANT;

    friend ostream& operator << <>( ostream& o, const PhysAlignedBSPTree2d< P_ITEM >& t );

private:
    // Operations deliberately revoked
    PhysAlignedBSPTree2d( const PhysAlignedBSPTree2d< P_ITEM >& );
    PhysAlignedBSPTree2d< P_ITEM >& operator =( const PhysAlignedBSPTree2d< P_ITEM >& );
    bool operator ==( const PhysAlignedBSPTree2d< P_ITEM >& );

    //Nested class representing a single partition
    class Partition
    {
    public:
        //ctor/dtor. The reserve size for the item list is set to reserveSize.
        Partition( size_t reserveSize )
        : pLeft_( NULL ), pRight_( NULL )
        {
            items_.reserve( reserveSize );
        };

        ~Partition()
        {
            if( pLeft_) _DELETE( pLeft_ );
            if( pRight_) _DELETE( pRight_ );
        };

        //Data members
        PItems items_; //The P_ITEMs stored in this partition
        Partition* pLeft_; //The 2 sub-partitions - always split along longer axis
        Partition* pRight_;
    };

    //Add pItem (which has boundary itemBoundary) to pPartition (which has
    //boundary partitionBoundary), or one of its subdivisions.
    void add( P_ITEM pItem, const MexAlignedBox2d& itemBoundary,
              Partition* pPartition, const MexAlignedBox2d& partitionBoundary );

    //Sub-divides pPartition unless it is too small, returning true if done it.
    //The partition has boundary partitionBoundary.
    //The existing items stored in pPartition are redistributed to its children
    //if they fit in either subdivision.
    bool split( Partition* pPartition, const MexAlignedBox2d& partitionBoundary );

    //Removes pItem (which has boundary itemBoundary) from pPartition (which has
    //boundary partitionBoundary), or one of its subdivisions.
    void remove( P_ITEM pItem, const MexAlignedBox2d& itemBoundary,
                 Partition* pPartition, const MexAlignedBox2d& partitionBoundary );

    //Add any items in partition (which has boundary partitionBoundary) whose
    //boundaries overlap boundary to pItems.
    size_t overlapping( PItems* pItems, const MexAlignedBox2d& boundary,
             const Partition& partition, const MexAlignedBox2d& partitionBoundary ) const;

    //True iff boundary is greater than the minimum resolution, and if so
    //returns the new left and right boundaries in pLeftBoundary and
    //pRightBoundary.
    bool split ( const MexAlignedBox2d boundary, MexAlignedBox2d* pLeftBoundary,
                 MexAlignedBox2d* pRightBoundary ) const;

    //writes partition data
    void trace( const Partition& partition, const MexAlignedBox2d& partitionBoundary,
                ostream& o ) const;

    //Data members
    MexAlignedBox2d boundary_; //The outer boundary of the space
    Partition* pTopPartition_; //Partition for whole space
    MATHEX_SCALAR minResolution_; //The minimum resolution for partitioning
    MATHEX_SCALAR lipSize_; //The amount by which left/right partitions are to be extended
                            //so that objects on the boundary tend to fall in at least one
                            //of the sub-partitions.
    size_t nMaxItems_; //The maximum number of items that would ideally
                       // be stored in a partition
    size_t nItems_; //Count of items stored in the tree
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "phys/absp2d.ctp"
//#endif

#ifdef _INLINE
    #include "phys/absp2d.itp"
#endif


#endif

/* End ABSP2D.HPP ***************************************************/
