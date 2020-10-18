/*
 * D E B R I S U N. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/debrisun.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/scale.hpp"
#include "ctl/vector.hpp"

#include "machphys/random.hpp"
#include "machphys/flames.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/soundman.hpp"
#include "world4d/matfrmpl.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysDebrisUnit );

static 	const ctl_vector<RenTexture>& textures();

MachPhysDebrisUnit::MachPhysDebrisUnit
(
	W4dEntity* pParent,
	const W4dTransform3d& localTransform,
	const MexAlignedBox2d& boundary,
    const PhysRelativeTime& overallDuration
)
: W4dEntity( factory(), pParent, localTransform ),
pFlame1_(NULL),
pFlame2_(NULL),
pFlame3_(NULL),
pPlume1_(NULL),
pPlume2_(NULL),
pPlume3_(NULL),
overallDuration_(overallDuration),
boundary_(boundary)
{
	HAL_STREAM("MPDebrisUnit::CTOR in\n" );
    TEST_INVARIANT;
    visible(true);

	//cycle the textures
	static W4dMaterialPlanPtr planPtr[4];
	static bool first = true;
	if ( first )
	{
		first = false;
		for( int i =0; i<4; ++i)
		{
			planPtr[i] =
			W4dMaterialFramePlan::createPlan(this, textures(), 10.0*overallDuration, 1, i, 2 );
		}
	}

	const size_t frameOffset = MachPhysRandom::randomInt(4);
	entityPlanForEdit().materialPlan( planPtr[frameOffset], SimManager::instance().currentTime() );
	HAL_STREAM("MPDebrisUnit::CTOR out\n" );
}

MachPhysDebrisUnit::MachPhysDebrisUnit( PerConstructor con )
: 	W4dEntity( con ),
	pFlame1_(NULL),
	pFlame2_(NULL),
	pFlame3_(NULL),
	pPlume1_(NULL),
	pPlume2_(NULL),
	pPlume3_(NULL)
{
}

MachPhysDebrisUnit::~MachPhysDebrisUnit()
{
	HAL_STREAM("MPDebrisUnit::DTOR\n" );
	visible(false);
    TEST_INVARIANT;
}

const MachPhysDebrisUnit& MachPhysDebrisUnit::factory()
{
    //static W4dRoot root( 1 );
    //static MachPhysDebrisUnit debris( &root, MexTransform3d::MexTransform3d() );

    //return debris;
    return MachPhysOtherPersistence::instance().debrisExemplar();
}

MachPhysDebrisUnit::MachPhysDebrisUnit
(
	W4dEntity* pParent,
	const MexTransform3d& transform
)
: W4dEntity(  pParent, transform, SOLID )
{
    TEST_INVARIANT;
    SysPathName lodFile( "models/debris/debris.lod" );
    ASSERT_FILE_EXISTS( lodFile.pathname().c_str() );
    readLODFile( lodFile );
}

void MachPhysDebrisUnit::startBurning()
{
    PhysAbsoluteTime currentTime = SimManager::instance().currentTime();

	MATHEX_SCALAR boundXMin = boundary_.minCorner().x();
	MATHEX_SCALAR boundYMin = boundary_.minCorner().y();
	MATHEX_SCALAR boundXMax = boundary_.maxCorner().x();
	MATHEX_SCALAR boundYMax = boundary_.maxCorner().y();

	ASSERT(boundXMin < boundXMax, "Bad boundary");
	ASSERT(boundYMin < boundYMax, "Bad boundary");

	MATHEX_SCALAR xLength = (boundXMax - boundXMin);
	MATHEX_SCALAR yLength = (boundYMax - boundYMin);

	//scale the debris unit model to xlength X yLength
	const RenNonUniformScale scale( xLength/boundingVolume().xLength(), yLength/boundingVolume().yLength(), 1.0);
	temporaryScale( scale, W4dEntity::PROPOGATE );

    MATHEX_SCALAR maxBigFlame = 1.5;
    MATHEX_SCALAR minBigFlame = 0.75;
    MATHEX_SCALAR maxSmallFlame = 0.7;
    MATHEX_SCALAR minSmallFlame = 0.25;
	boundXMin += xLength/4.0;
	boundXMax -= xLength/4.0;
	boundYMin += yLength/4.0;
	boundYMax -= yLength/4.0;

   	double xVal = MachPhysRandom::randomDouble(boundXMin, boundXMax);
   	double yVal = MachPhysRandom::randomDouble(boundYMin, boundYMax);
	double bigFlameSize = MachPhysRandom::randomDouble(minBigFlame, maxBigFlame);
	double weeFlameSize = MachPhysRandom::randomDouble(minSmallFlame, maxSmallFlame);

    MexTransform3d tx;
    MexPoint3d position( xVal, yVal, bigFlameSize/2.0 );
    tx = W4dEntity::localTransform();
    tx.translate(position);
	pFlame1_ = _NEW(MachPhysFlame(
    	this,
    	tx,
    	FLAME_1,
    	bigFlameSize,
    	bigFlameSize,
    	0,
		currentTime + 0.1,
    	overallDuration_ ));

    position.x(xVal);
    position.y(yVal);
    position.z(0);
    tx = W4dEntity::localTransform();
    tx.translate(position);
	pPlume1_ =  _NEW(MachPhysSmokePlume(
    	this,
		tx,
        8,
        0,
        3,
        1.5,
        PUFF_1,
        currentTime,
        30 ));
/*
   	xVal = MachPhysRandom::randomDouble(boundXMin, boundXMax);
   	yVal = MachPhysRandom::randomDouble(boundYMin, boundYMax);
	bigFlameSize = MachPhysRandom::randomDouble(minBigFlame, maxBigFlame);

    position.x(xVal);
    position.y(yVal);
    position.z(bigFlameSize/2);
    tx = W4dEntity::localTransform();
    tx.translate(position);
	pFlame2_ = _NEW(MachPhysFlame(
    	this,
    	tx,
    	FLAME_1,
    	bigFlameSize,
    	0,
    	currentTime,
    	overallDuration_ ));

    position.x(xVal);
    position.y(yVal);
    position.z(0);
    tx = W4dEntity::localTransform();
    tx.translate(position);
	pPlume2_ =  _NEW(MachPhysSmokePlume(
    	this,
		tx,
        12,
        0,
        7,
        0.5,
        MachPhysSmokePuff::randomPuff(),
        currentTime,
        30 ));*/
}

bool MachPhysDebrisUnit::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy ) const
{
    return W4dEntity::defaultIntersectsLine( line, pDistance, accuracy);
}

void MachPhysDebrisUnit::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const MachPhysDebrisUnit& debrisUnit )
{
    const W4dEntity& base = debrisUnit;

    ostr << base;

    //ostr << debrisUnit.pPlume1_;
    //ostr << debrisUnit.pPlume2_;
    //ostr << debrisUnit.pPlume3_;
    //ostr << debrisUnit.pFlame1_;
    //ostr << debrisUnit.pFlame2_;
    //ostr << debrisUnit.pFlame3_;
    ostr << debrisUnit.overallDuration_;
    ostr << debrisUnit.boundary_;
}

void perRead( PerIstream& istr, MachPhysDebrisUnit& debrisUnit )
{
    W4dEntity& base = debrisUnit;

    istr >> base;

    //istr >> debrisUnit.pPlume1_;
    //istr >> debrisUnit.pPlume2_;
    //istr >> debrisUnit.pPlume3_;
    //istr >> debrisUnit.pFlame1_;
    //istr >> debrisUnit.pFlame2_;
    //istr >> debrisUnit.pFlame3_;
    istr >> debrisUnit.overallDuration_;
    istr >> debrisUnit.boundary_;
}

//local
static 	const ctl_vector<RenTexture>& textures()
{
	static ctl_vector<RenTexture> textures;
	static bool first = true;

	if( first )
	{
		first = false;

		textures.reserve(4);

		textures.push_back( RenTexManager::instance().createTexture("elec1_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("elec2_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("elec3_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("elec4_bt.bmp") );
	}
	return textures;
}

//static
void MachPhysDebrisUnit::preloadTextures()
{
	textures();
}

/* End DEBRISUN.CPP ***************************************************/
