#include "mathex/mathex.hpp"
#include "mathex/transf3d.hpp"

MATHEX_SCALAR det(MATHEX_SCALAR	a1,
						  MATHEX_SCALAR a2,
						  MATHEX_SCALAR a3,
						  MATHEX_SCALAR a4,
						  MATHEX_SCALAR a5,
						  MATHEX_SCALAR a6,
						  MATHEX_SCALAR a7,
						  MATHEX_SCALAR a8,
						  MATHEX_SCALAR a9)
{

	return a1*a5*a9 + a4*a8*a3 + a7*a2*a6 -
		   a1*a8*a6 - a4*a9*a2 - a7*a5*a3;
}

// TODO is this in use?
MexTransform3d MexTransform3d::inverse() const

{
    //*this = Inv(*this)

	//the cofactor matrix of this:
    MATHEX_SCALAR   inv[4][4];
	MATHEX_SCALAR detForward = det(forward_[0][0], forward_[0][1], forward_[0][2],
				                   forward_[1][0], forward_[1][1], forward_[1][2],
	                               forward_[2][0], forward_[2][1], forward_[2][2]);

	PRE(detForward != 0);

	inv[0][0] = (forward_[1][1] * forward_[2][2] - forward_[1][2] * forward_[2][1])/detForward;
	inv[1][0] = (forward_[1][2] * forward_[2][0] - forward_[1][0] * forward_[2][2])/detForward;
	inv[2][0] = (forward_[1][0] * forward_[2][1] - forward_[1][1] * forward_[2][0])/detForward;

	inv[3][0] = -det(forward_[1][0], forward_[1][1], forward_[1][2],
	                forward_[2][0], forward_[2][1], forward_[2][2],
				    forward_[3][0], forward_[3][1], forward_[3][2])/detForward;

	inv[0][1] = (forward_[2][1] * forward_[0][2] - forward_[0][1] * forward_[2][2])/detForward;
	inv[1][1] = (forward_[0][0] * forward_[2][2] - forward_[0][2] * forward_[2][0])/detForward;
	inv[2][1] = (forward_[0][1] * forward_[2][0] - forward_[0][0] * forward_[2][1])/detForward;

	inv[3][1] =  det(forward_[0][0], forward_[0][1], forward_[0][2],
	                forward_[2][0], forward_[2][1],	forward_[2][2],
				    forward_[3][0], forward_[3][1], forward_[3][2])/detForward;

	inv[0][2] = (forward_[1][1] * forward_[0][2] - forward_[0][1] * forward_[1][2])/detForward;
	inv[1][2] = (forward_[1][0] * forward_[0][2] - forward_[0][0] * forward_[1][2])/detForward;
	inv[2][2] = (forward_[0][0] * forward_[1][1] - forward_[0][1] * forward_[1][0])/detForward;

	inv[3][2] = -det(forward_[0][0], forward_[0][1], forward_[0][2],
	                forward_[1][0], forward_[1][1],	forward_[1][2],
				    forward_[3][0], forward_[3][1], forward_[3][2])/detForward;


	inv[0][3] = 0;
	inv[1][3] = 0;
	inv[2][3] = 0;
	inv[3][3] = 1.0;

	return MexTransform3d(inv);
}

