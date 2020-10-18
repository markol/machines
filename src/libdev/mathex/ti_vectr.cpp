/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "base/base.hpp"
#include <vector>
#include "ctl/vector.hpp"

#include "mathex/outcode.hpp"
#include "mathex/mathex.hpp"
#include "mathex/grid2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/transf3d.hpp"

void MathexDummyFunction1()
{
	static std::vector<OutCode> outcodeVec;
    static ctl_vector< MATHEX_SCALAR > d3;
    static ctl_vector< MexPoint2d > d4;
    static ctl_vector< MexPoint2d* > d5;
    static ctl_vector< MexVec2 > d6;
    static ctl_vector< uint > d7;
    static std::auto_ptr< MexConvexPolygon2d::Points > d8;

	static	MexXCoord< double >					dummyX(0);
	static	MexYCoord< double >					dummyY(0);

    static  ctl_vector< MexGrid2d::CellIndex > d9;
    static ctl_vector< int > d10;
    static ctl_vector< MexPoint3d > d11;
    static ctl_vector< MexTransform3d > d12;

}

   	PER_DEFINE_PERSISTENT_T1( MexXCoord, double );
   	PER_DEFINE_PERSISTENT_T1( MexYCoord, double );

/* End TI.CPP *****************************************************/
