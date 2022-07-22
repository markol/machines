/*
 * A B S P 2 D . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#include "absp2d.hpp"

#include <iostream>
#include "mathex/point2d.hpp"
#include "mathex/epsilon.hpp"

#ifndef _INLINE
    #include "phys/absp2d.itp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
PhysAlignedBSPTree2d< P_ITEM >::PhysAlignedBSPTree2d
(
	const MexPoint2d& minCorner, const MexPoint2d& maxCorner,
    MATHEX_SCALAR minResolution, size_t nMaxItems,
    MATHEX_SCALAR lipSize
)
:	boundary_( minCorner, maxCorner ),
	minResolution_( minResolution ),
	nMaxItems_( nMaxItems ),
	pTopPartition_( NULL ),
    lipSize_( lipSize ),
	nItems_( 0 )
{
    PRE( minCorner.x() < maxCorner.x() )
    PRE( minCorner.y() < maxCorner.y() )
    PRE( minResolution > MexEpsilon::instance() )

    //Resolution must be greater than overlap
    if( minResolution < lipSize * 2 ) minResolution_ = lipSize * 2;

	//Allocate the top partition and set storage size for its contents
	pTopPartition_ = _NEW( Partition( nMaxItems_ ) );

    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
PhysAlignedBSPTree2d< P_ITEM >::~PhysAlignedBSPTree2d()
{
    TEST_INVARIANT;

	//Delete the tree of partitions recursively
	if( pTopPartition_ ) _DELETE( pTopPartition_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::add( P_ITEM pItem )
{
    PRE( pItem != NULL );

	//Get the boundary for the item
    MexAlignedBox2d itemBoundary;
	pItem->boundary( &itemBoundary );

    //Add to the appropriate partition
    POST_DATA( size_t oldCount = nItems_ );
    add( pItem, itemBoundary, pTopPartition_, boundary_ );
    POST( nItems_ == oldCount + 1 );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::add
(
    P_ITEM pItem,
    const MexAlignedBox2d& itemBoundary,
    Partition* pPartition,
    const MexAlignedBox2d& partitionBoundary
)
{
    PRE( pPartition != NULL );
    PRE( pItem != NULL );

    //If the current partition is full and not split, split it
    bool isSplit = pPartition->pLeft_ != NULL;
    if( not isSplit  and  pPartition->items_.size() == nMaxItems_ )
        isSplit = split( pPartition, partitionBoundary );

    //If subdivided check to see if the new item belongs in either
    //the left or right partition
    bool done = isSplit;
    if( isSplit )
    {
        //Generate the left and right boundaries
        MexAlignedBox2d leftBoundary;
        MexAlignedBox2d rightBoundary;
        split( partitionBoundary, &leftBoundary, &rightBoundary );

        //Test for containment
        if( leftBoundary.contains( itemBoundary ) )
            add( pItem, itemBoundary, pPartition->pLeft_, leftBoundary );
        else if( rightBoundary.contains( itemBoundary ) )
            add( pItem, itemBoundary, pPartition->pRight_, rightBoundary );
        else
            done = false;
    }

    //Add the item to this partition, if not added to a subdivision
    if( not done )
    {
        pPartition->items_.push_back( pItem );
        ++nItems_;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
bool PhysAlignedBSPTree2d< P_ITEM >::split
(
    Partition* pPartition,
    const MexAlignedBox2d& partitionBoundary
)
{
	bool wasSplit;

    //Generate the left and right boundaries
    MexAlignedBox2d leftBoundary;
    MexAlignedBox2d rightBoundary;
    wasSplit = split( partitionBoundary, &leftBoundary, &rightBoundary );

    if( wasSplit )
    {
        //Add the 2 new partitions
        pPartition->pLeft_ = _NEW( Partition( nMaxItems_ ) );
        pPartition->pRight_ = _NEW( Partition( nMaxItems_ ) );

        //Extract the item pointers from the current list and clear it
        PItems& theItems = pPartition->items_;
        PItems oldItems( theItems );
        theItems.erase( theItems.begin(), theItems.end() );

        //Add each of the old items back to the original partition or one of its
        //subdivisions
        size_t nOldItems = oldItems.size();
        nItems_ -= nOldItems;
        while( oldItems.size() != 0 )
        {
            P_ITEM pItem = oldItems.back();
            oldItems.pop_back();
            MexAlignedBox2d itemBoundary;
            pItem->boundary( &itemBoundary );
            add( pItem, itemBoundary, pPartition, partitionBoundary );
        }
    }

    return wasSplit;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::remove( P_ITEM pItem )
{
    PRE( pItem != NULL );

	//Get the boundary for the item
    MexAlignedBox2d itemBoundary;
	pItem->boundary( &itemBoundary );

    //Remove from the appropriate partition
    POST_DATA( size_t oldCount = nItems_ );
    remove( pItem, itemBoundary, pTopPartition_, boundary_ );
    POST( nItems_ == oldCount - 1 );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::remove
(
    P_ITEM pItem,
    const MexAlignedBox2d& itemBoundary,
    Partition* pPartition,
    const MexAlignedBox2d& partitionBoundary
)
{
    PRE( pPartition != NULL );
    PRE( pItem != NULL );

    //If subdivided check to see if the new item belongs in either
    //the left or right partition
    bool done = pPartition->pLeft_ != NULL;
    if( done )
    {
        //Generate the left and right boundaries
        MexAlignedBox2d leftBoundary;
        MexAlignedBox2d rightBoundary;
        split( partitionBoundary, &leftBoundary, &rightBoundary );

        //Test for containment
        if( leftBoundary.contains( itemBoundary ) )
            remove( pItem, itemBoundary, pPartition->pLeft_, leftBoundary );
        else if( rightBoundary.contains( itemBoundary ) )
            remove( pItem, itemBoundary, pPartition->pRight_, rightBoundary );
        else
            done = false;
    }

    //If the item is not in a subdivision, it must be in this partition, so remove it
    if( not done )
    {
        PItems& items = pPartition->items_;
        for( typename PItems::iterator i = items.begin(); i != items.end(); ++i )
        {
            if( (*i) == pItem )
            {
                items.erase( i );
                --nItems_;
                done = true;
                break;
            }
        }
    }

    POST( done );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
size_t PhysAlignedBSPTree2d< P_ITEM >::overlapping
(
    const MexAlignedBox2d& boundary, PItems* pItems
) const
{
    PRE( pItems != NULL );

    //Add overlapping items from the top partition and any subdivisions
    return overlapping( pItems, boundary, *pTopPartition_, boundary_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
size_t PhysAlignedBSPTree2d< P_ITEM >::overlapping
(
    PItems* pItems,
    const MexAlignedBox2d& boundary,
    const Partition& partition,
    const MexAlignedBox2d& partitionBoundary
) const
{
    PRE( pItems != NULL );

    //Check for overlap with items in this partition
    const PItems& items = partition.items_;
    size_t n = items.size();
    size_t count = 0;

    for( size_t i = 0; i != n; ++i )
    {
        //Get this item and its boundary
        P_ITEM pItem = items[i];
        MexAlignedBox2d itemBoundary;
        pItem->boundary( &itemBoundary );

        //If the boundary overlaps, add the item to the output list
        if( itemBoundary.intersects( boundary ) )
        {
            ++count;
            pItems->push_back( pItem );
        }
    }

    //If subdivided, check to see if overlaps left/right partitions
    if( partition.pLeft_ != NULL )
    {
        //Generate the left and right boundaries
        MexAlignedBox2d leftBoundary;
        MexAlignedBox2d rightBoundary;
        split( partitionBoundary, &leftBoundary, &rightBoundary );

        //Test for intersection with either side, and test recursively if so
        if( leftBoundary.intersects( boundary ) )
            count += overlapping( pItems, boundary, *partition.pLeft_, leftBoundary );

        if( rightBoundary.intersects( boundary ) )
            count += overlapping( pItems, boundary, *partition.pRight_, rightBoundary );
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
bool PhysAlignedBSPTree2d< P_ITEM >::split
(
	const MexAlignedBox2d boundary,
	MexAlignedBox2d* pLeftBoundary,
	MexAlignedBox2d* pRightBoundary
) const
{
	bool wasSplit;
	const MexPoint2d& minCorner = boundary.minCorner();
	const MexPoint2d& maxCorner = boundary.maxCorner();
	MATHEX_SCALAR width = (maxCorner.x() - minCorner.x()) * 0.5;
	MATHEX_SCALAR height = (maxCorner.y() - minCorner.y()) * 0.5;

	//Check for too small
    bool widthBigger = width > height;
	wasSplit = (widthBigger ? width > minResolution_ : height > minResolution_);

	if( wasSplit )
	{
		//Initialise the new boundarys
		*pLeftBoundary = *pRightBoundary = boundary;

		//Decide which way to split
		if( widthBigger )
		{
			MATHEX_SCALAR x = minCorner.x() + width;
			pLeftBoundary->xMax( x + lipSize_ );
			pRightBoundary->xMin( x - lipSize_ );
		}
		else
		{
			MATHEX_SCALAR y = minCorner.y() + height;
			pLeftBoundary->yMax( y + lipSize_ );
			pRightBoundary->yMin( y - lipSize_ );
		}
	}

	return wasSplit;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
ostream& operator <<( ostream& o, const PhysAlignedBSPTree2d< P_ITEM >& t )
{
    o << "AlignedBSPTree2d " << (void *)&t << std::endl;
    o << "Boundary " << t.boundary_ << std::endl;
    o << "Resolution " << t.minResolution_  << "  MaxItems " << t.nMaxItems_ <<
         "  Current count " << t.nItems_ << std::endl;
    t.trace( *(t.pTopPartition_), t.boundary_, o );
    o << "End AlignedBSPTree2d " << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class P_ITEM >
void PhysAlignedBSPTree2d< P_ITEM >::trace
(
    const Partition& partition, const MexAlignedBox2d& partitionBoundary, ostream& o
) const
{
    o << "  Partition( " << partition.items_.size() << " items )"
      << partitionBoundary << std::endl;

    if( partition.pLeft_ != NULL )
    {
        MexAlignedBox2d leftBoundary;
        MexAlignedBox2d rightBoundary;
        split( partitionBoundary, &leftBoundary, &rightBoundary );

        trace( *(partition.pLeft_), leftBoundary, o );
        trace( *(partition.pRight_), rightBoundary, o );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End ABSP2D.CTP ***************************************************/
