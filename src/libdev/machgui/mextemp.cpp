/*
 * M E X T E M P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/mextemp.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/point2d.hpp"

MexTemp::MexTemp()
{
}

MexTemp::~MexTemp()
{
}

//static
void MexTemp::quarterRotate( MexAlignedBox2d* pBox, int n )
{
    PRE( pBox != NULL );

    //Convert n to a value in the range -3 thru 3
    n %= 4;
    if( n != 0 )
    {
        //Get the existing limits
        MATHEX_SCALAR ax = pBox->minCorner().x();
        MATHEX_SCALAR ay = pBox->minCorner().y();
        MATHEX_SCALAR bx = pBox->maxCorner().x();
        MATHEX_SCALAR by = pBox->maxCorner().y();

        MATHEX_SCALAR xMin = 0, xMax = 0, yMin = 0, yMax = 0;

        //Take case specific action
        switch( n )
        {
            case 1:
            case -3:
            {
                xMin = -by;
                xMax = -ay;
                yMin = ax;
                yMax = bx;
                break;
            }

            case 2:
            case -2:
            {
                xMin = -bx;
                xMax = -ax;
                yMin = -by;
                yMax = -ay;
                break;
            }

            case 3:
            case -1:
            {
                xMin = ay;
                xMax = by;
                yMin = -bx;
                yMax = -ax;
                break;
            }
        }

        //Reset the boundary
        pBox->corners( MexPoint2d( xMin, yMin ), MexPoint2d( xMax, yMax ) );
    }
}
/* End MEXTEMP.CPP **************************************************/
