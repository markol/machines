/*
 * M A T B O D Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/internal/matbody.hpp"
#include "render/internal/matmgr.hpp"
#include "render/texture.hpp"
#include "render/capable.hpp"
#include "render/internal/devicei.hpp"

#ifndef _INLINE
    #include "render/internal/matbody.ipp"
#endif

PER_DEFINE_PERSISTENT( RenIMatBody );

RenIMatBody::RenIMatBody():
	diffuse_ (RenColour::white()),
	emissive_(RenColour::black()),
	depth_(0),
	alphaPriority_(0),
	refCount_(0),
	meshId_(0),
	fogMultiplier_(1.0),
	dirty_(true),
	alphaTransparent_(false),
	transparent_(false),
	interMeshCoplanar_(false),
	sharable_(true),
	absolutePriority_(false),
	intraMeshPriority_(false),
	usesBilinear_(false),
	hasFogMultiplier_(false)
{
	PRE(Ren::initialised());
    TEST_INVARIANT;
}

RenIMatBody::RenIMatBody(const RenColour& dif):
	diffuse_ (dif),
	emissive_(RenColour::black()),
	depth_(0),
	alphaPriority_(0),
	refCount_(0),
	meshId_(0),
	fogMultiplier_(1.0),
	dirty_(true),
	alphaTransparent_(false),
	transparent_(false),
	interMeshCoplanar_(false),
	sharable_(true),
	absolutePriority_(false),
	intraMeshPriority_(false),
	usesBilinear_(false),
	hasFogMultiplier_(false)
{
	PRE(Ren::initialised());
    TEST_INVARIANT;
}

RenIMatBody::RenIMatBody( const RenIMatBody& copyMe ):
	texture_( copyMe.texture_ ),
	diffuse_( copyMe.diffuse_ ),
	emissive_( copyMe.emissive_ ),
	depth_( copyMe.depth_ ),
	alphaPriority_(copyMe.alphaPriority_),
	refCount_(0),
	fogMultiplier_(copyMe.fogMultiplier_),
	dirty_(true),
	alphaTransparent_( copyMe.alphaTransparent_ ),
	transparent_( copyMe.transparent_),
	interMeshCoplanar_( copyMe.interMeshCoplanar_ ),
	sharable_(copyMe.sharable_),
	absolutePriority_(copyMe.absolutePriority_),
	intraMeshPriority_( copyMe.intraMeshPriority_ ),
	usesBilinear_(copyMe.usesBilinear_),
	hasFogMultiplier_(copyMe.hasFogMultiplier_)
{
}

RenIMatBody::~RenIMatBody()
{
    TEST_INVARIANT;
}

void RenIMatBody::update()
{
	updateTransparent();
	dirty_ = false;
}

void RenIMatBody::updateTransparent()
{
	const bool matTransparent = diffuse_.isTransparent();

	// The transparent flag is not set if the current device doesn't support it.
	// This reflection of the device capabilities strictly doesn't belong here.
	// It is done anyway as an optimisation: the transparency capabilities are
	// evaluated here so they don't have to be evaluated every time we apply
	// a material for rendering purposes.
	// TBD: this may fail in the presence of multiple devices.
	const RenCapabilities& caps = RenIDeviceImpl::currentPimpl()->capabilities();
	const bool stipple = caps.supportsStippledAlpha();
	const bool alpha   = caps.supportsFlatAlpha();
	alphaTransparent_ = matTransparent && alpha;
	transparent_      = matTransparent && (alpha || stipple);
}

bool RenIMatBody::operator==(const RenIMatBody& rhs)
{
	// This fn. compares the values of the attributes of a material for
	// equivalence.  It is not appropriate to compare these attributes:
	//		reference counts	-- implementation detail
	//		dirty_				-- 		 "			 "
	//		transparent_		-- cached value, derived from alpha values
	//		meshId_				-- implementation detail of rendering
	// Note the order: the values which are most likely to differ are compared
	// first, increasing the chance of an early false result.
	return	diffuse_		   == rhs.diffuse_   &&
			texture_		   == rhs.texture_   &&
			depth_			   == rhs.depth_     &&
			alphaPriority_	   == rhs.alphaPriority_     &&
			interMeshCoplanar_ == rhs.interMeshCoplanar_ &&
			absolutePriority_  == rhs.absolutePriority_  &&
			intraMeshPriority_ == rhs.intraMeshPriority_ &&
			emissive_		   == rhs.emissive_	&&
			hasFogMultiplier_  == rhs.hasFogMultiplier_ &&
			fogMultiplier_	   == rhs.fogMultiplier_;
}

void RenIMatBody::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenIMatBody& t )
{
    o << "material: " << t.texture() << ", " << t.diffuse() << ", count=" << t.refCount_;
	o << ", trans=" << ((t.transparent_)? "t": "f");
	o << ", tex alpha=" << ((t.texture_.hasAlphaTransparency())? "t": "f");
	o << ", inter=" << ((t.interMeshCoplanar_)? "t": "f");
	o << ", intra=" << ((t.intraMeshPriority_)? "t": "f");
	o << ", abs=" <<   ((t.absolutePriority_ )? "t": "f");
	o << ", cdepth=" << t.coplanarDepth();
	o << ", fog modif=" << ((t.hasFogMultiplier_)? "t": "f");
	if(t.hasFogMultiplier_)
		o << ", fog mult=" << t.fogMultiplier_;

    return o;
}

inline int testAndShift(bool b, uint shiftBy)
{
	return _STATIC_CAST(int, ((b)? 1: 0) << shiftBy);
}

static uchar pack6Bools
(
	bool b1,
	bool b2,
	bool b3,
	bool b4,
	bool b5,
	bool b6
)
{
	const int result =  testAndShift(b1, 0) |
						testAndShift(b2, 1) |
						testAndShift(b3, 2) |
						testAndShift(b4, 3) |
						testAndShift(b5, 4) |
						testAndShift(b6, 5);

	return _STATIC_CAST(uchar, result);
}

static void unpack6Bools
(
	uchar packed,
	bool* b1,
	bool* b2,
	bool* b3,
	bool* b4,
	bool* b5,
	bool* b6
)
{
	*b1 = packed & 0x01;
	*b2 = packed & 0x02;
	*b3 = packed & 0x04;
	*b4 = packed & 0x08;
	*b5 = packed & 0x10;
	*b6 = packed & 0x20;
}

void perWrite( PerOstream& ostr, const RenIMatBody& mat )
{
	// A note on flags: The transparent flags are not persisted because they
	// depend upon the hardware caps and will vary from platform to platform.
	// Rather than persist them, we force an update call after reading and thus
	// the flags will be recomputed before first use.  Hence dirty_ isn't
	// persisted either.
	const uchar packedBools = pack6Bools(mat.interMeshCoplanar_,
										 mat.sharable_,
										 mat.absolutePriority_,
										 mat.usesBilinear_,
										 mat.intraMeshPriority_,
										 mat.hasFogMultiplier_);

	// A note on reference counts: We cannot rightfully persist the reference
	// count, since the number of references to the object when it is
	// streamed back in is unrelated to the number of references that existed
	// when it was originally streamed out.
	// We don't persist meshId_ because it isn't part of a material's rep;
	// it is only used for alpha sorting during rendering.
    PER_WRITE_RAW_OBJECT( ostr, mat.texture_ );
    PER_WRITE_RAW_OBJECT( ostr, mat.diffuse_ );
    PER_WRITE_RAW_OBJECT( ostr, mat.emissive_ );
    PER_WRITE_RAW_OBJECT( ostr, mat.depth_ );
    PER_WRITE_RAW_OBJECT( ostr, mat.alphaPriority_ );
    PER_WRITE_RAW_OBJECT( ostr, mat.fogMultiplier_ );
    PER_WRITE_RAW_OBJECT( ostr, packedBools );
}

void perRead( PerIstream& istr, RenIMatBody& mat )
{
	uchar packedBools;
    PER_READ_RAW_OBJECT( istr, mat.texture_ );
    PER_READ_RAW_OBJECT( istr, mat.diffuse_ );
    PER_READ_RAW_OBJECT( istr, mat.emissive_ );
    PER_READ_RAW_OBJECT( istr, mat.depth_ );
    PER_READ_RAW_OBJECT( istr, mat.alphaPriority_ );
    PER_READ_RAW_OBJECT( istr, mat.fogMultiplier_ );
    PER_READ_RAW_OBJECT( istr, packedBools );

	bool tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
	unpack6Bools(packedBools, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
	mat.interMeshCoplanar_	= (tmp1? 1: 0);
	mat.sharable_			= (tmp2? 1: 0);
	mat.absolutePriority_	= (tmp3? 1: 0);
	mat.usesBilinear_		= (tmp4? 1: 0);
	mat.intraMeshPriority_	= (tmp5? 1: 0);
	mat.hasFogMultiplier_	= (tmp6? 1: 0);

	// Force an update of transparency stuff.  See note on persisting flags.
	mat.update();
}

/* End MATBODY.CPP *************************************************/
