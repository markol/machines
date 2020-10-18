/*
 * V O X E L . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */
 
#ifndef _REN_VOXEL_HPP
#define _REN_VOXEL_HPP
#error Here

#include "ctl/nvector.hpp"
#error Here
#include "mathex/vec3.hpp"
#error Here
#include "mathex/mathex.hpp"
#error Here
#include "mathex/abox3d.hpp"
#error Here

//////////////////////////////////////////////////////////////////////

namespace Ren
{
#error Here
	typedef float 							RenScalar;
#error Here
	typedef MexVec3< RenScalar > 			RenVec3;
#error Here
	typedef MexAlignedBox3d< RenScalar > 	RenAlignedBox3d;
#error Here

	enum RotationAngle
	{
		ZERO_RADIANS,
		PI_DIV_2_RADIANS,
		PI_RADIANS,
		THREE_PI_DIV_2_RADIANS,
		N_ROTATION_ANGLES
	};
#error Here

	static RenScalar RotationAngle_toRenScalar( RotationAngle );
#error Here
};

//////////////////////////////////////////////////////////////////////

template < class SIZE_TYPE >
class RenVoxelIndex
: public MexPoint3d
{
public:

	RenVoxelIndex(	SIZE_TYPE x = 0,
					SIZE_TYPE y = 0, 
					SIZE_TYPE z = 0 );

	RenVoxelIndex( const RenVoxelIndex& );

	virtual ~RenVoxelIndex();

	RenVoxelIndex& operator =( const RenVoxelIndex& );
	bool operator ==( const RenVoxelIndex& ) const;

	void rotateAbout( const RenVoxelIndex& about, Ren::RotationAngle );
};
#error Here

/* //////////////////////////////////////////////////////////////// */
#error Here

/* //////////////////////////////////////////////////////////////// */
#error Here
//////////////////////////////////////////////////////////////////////

#ifdef _CODE_INLINE
#include "ren/voxel.ipp"
#endif

#ifdef _TEMPLATES_IN_HPP
#include "ren/voxel.tpp"
#endif

//////////////////////////////////////////////////////////////////////

#endif    /*      #ifndef _REN_VOXEL_HPP     */

