/*
 * S P I R A L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/spiral.hpp"

MachLogSquareSpiral::MachLogSquareSpiral()
: x_( 0 ),
  y_( 0 ),
  edgeLength_( 1 ),
  gridIndex_( 0 )
{

    TEST_INVARIANT;
}

MachLogSquareSpiral::~MachLogSquareSpiral()
{
    TEST_INVARIANT;

}

int MachLogSquareSpiral::x() const
{
    return x_;
}

int MachLogSquareSpiral::y() const
{
    return y_;
}

void MachLogSquareSpiral::operator ++( PREFIX )
{
    (*this)++;
}

void MachLogSquareSpiral::operator ++( POSTFIX )
{
    PRE( edgeLength_ % 2 == 1 );

    ++gridIndex_;
    
    if( gridIndex_ >= ( edgeLength_ - 1 ) * 4 )
    {
        //  We have been all the way around this square, move on
        //  to the next one

        gridIndex_ = 0;
        edgeLength_ += 2;
    }    
    
    
    if( edgeLength_ == 1 )
    {
        x_ = 0;
        y_ = 0;
    }
    else
    {
        //  The grid is arranged in a series of squares - the middle square
        //  has edge length 1, the next square has edge length 3, then 5, 7 etc.
        //  Grid points are generated around the edge of each square.

        size_t edge = gridIndex_ / ( edgeLength_ - 1 );
        size_t edgeIndex =  gridIndex_ % ( edgeLength_ - 1 );
        
        switch( edge )
        {
            case 0:
                x_ = ( edgeLength_ - 1 ) / 2 - edgeIndex;
                y_ = ( edgeLength_ - 1 ) / 2;
                break;
            case 1:
                x_ = -( edgeLength_ - 1 ) / 2;
                y_ = ( edgeLength_ - 1 ) / 2 - edgeIndex;
                break;
            case 2:
                x_ = -( edgeLength_ - 1 ) / 2 + edgeIndex;
                y_ = -( edgeLength_ - 1 ) / 2;
                break;
            case 3:
                x_ = ( edgeLength_ - 1 ) / 2;
                y_ = -( edgeLength_ - 1 ) / 2 + edgeIndex;
                break;
            DEFAULT_ASSERT_BAD_CASE( edge );
        }
    }
}

void MachLogSquareSpiral::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogSquareSpiral& t )
{

    o << "MachLogSquareSpiral " << (void*)&t << " start" << std::endl;
    o << "MachLogSquareSpiral " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SPIRAL.CPP ***************************************************/


