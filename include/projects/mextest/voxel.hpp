/*
 * V O X E L . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */
 
#ifndef _REN_VOXEL_HPP
#define _REN_VOXEL_HPP

// #include "ctl/nvector.hpp"
#include "mathex/vec3.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox3d.hpp"

//////////////////////////////////////////////////////////////////////


// namespace Ren
//{
	typedef float 							RenScalar;
//	typedef MexVec3< RenScalar > 			RenVec3;
//	typedef MexAlignedBox3d< RenScalar > 	RenAlignedBox3d;
/*
	enum RotationAngle
	{
		ZERO_RADIANS,
		PI_DIV_2_RADIANS,
		PI_RADIANS,
		THREE_PI_DIV_2_RADIANS,
		N_ROTATION_ANGLES
	};
*/
//	static RenScalar RotationAngle_toRenScalar( RotationAngle );
//};

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

/* //////////////////////////////////////////////////////////////// */

template < class OCCUPANT >
class RenVoxel
{
public:

	RenVoxel( const RenAlignedBox3d& = RenAlignedBox3d(), 
				const OCCUPANT& = OCCUPANT() );

	RenVoxel( const RenVoxel& );

	~RenVoxel();

	RenVoxel& operator =( const RenVoxel& );
	bool operator ==( const RenVoxel& ) const;

	///////////////////////////////

	OCCUPANT& occupant();
	const OCCUPANT& occupant() const;

//	RenVec3 position() const;
	MexVec3< float > position() const;
	// POST( result == boundary().centre() );

//	const RenAlignedBox3d& boundary() const;
	const MexAlignedBox3d< float >& boundary() const;

	///////////////////////////////

protected:
	
//	RenAlignedBox3d& boundary();
	MexAlignedBox3d< float >& boundary();

	OCCUPANT		occupant_;
//	RenAlignedBox3d	boundary_;
	MexAlignedBox3d< float >	boundary_;

	friend class RenVoxels< OCCUPANT >;
};

/* //////////////////////////////////////////////////////////////// */


//////////////////////////////////////////////////////////////////////

#ifdef _CODE_INLINE
#include "ren/voxel.ipp"
#endif

#ifdef _TEMPLATES_IN_HPP
#include "ren/voxel.tpp"
#endif

//////////////////////////////////////////////////////////////////////

#endif    /*      #ifndef _REN_VOXEL_HPP     */

