/*
 * S M O K P U F F . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <iostream>
#include "stdlib/string.hpp"

#include "device/time.hpp"
#include "mathex/degrees.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "machphys/smokpuff.hpp"
#include "machphys/random.hpp"

//#include "machphys/private/datastr.hpp"
//#include "system/pathname.hpp"
//#include "ctl/vector.hpp"

PER_DEFINE_PERSISTENT( MachPhysSmokePuff );

MachPhysSmokePuff::MachPhysSmokePuff(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysPuffType puffType,
    MATHEX_SCALAR size,
    MATHEX_SCALAR dOffset )
: W4dSprite3d( pParent, localTransform, size * 2.0, size * 2.0, material( puffType ) )
{
    depthOffset( dOffset );
    rotate( MexDegrees( 90 ) * ( rand() % 4 ) );

    TEST_INVARIANT;
}

MachPhysSmokePuff::~MachPhysSmokePuff()
{
    TEST_INVARIANT;

}

// static
void MachPhysSmokePuff::preload( void )
{
    material( PUFF_1 );
    material( PUFF_2 );
    material( PUFF_3 );
    material( PUFF_4 );
    material( PUFF_5 );
    material( PUFF_6 );

	material( POD_GREEN );
	material( POD_PURPLE );

	material( C_FACTORY_GREEN );
	material( C_FACTORY_WHITE );

	material( M_FACTORY_GREEN );
	material( M_FACTORY_BROWN );

	material( F_FACTORY_GREEN );
	material( F_FACTORY_RED );

	material( C_LAB_BLUE );
	material( C_LAB_WHITE );

	material( M_LAB_BLUE );
	material( M_LAB_BROWN );

	material( SMELTER_PURPLE );
	material( SMELTER_RED );

	material( MINE_YELLOW );
	material( MINE_BLACK );

	material( GARRISON_GREEN );
	material( GARRISON_YELLOW );

	material( EMPLACEMENT_RED );
	material( EMPLACEMENT_YELLOW );

	material( BEACON_RED );
	material( BEACON_AQUA );
}
/*
static ctl_vector< MATHEX_SCALAR > colours()
{
	static ctl_vector< MATHEX_SCALAR > theColours = MachPhysDataStream::data("smokecol.dat" );

	static bool first = true;
	if( first )
	{
		first = false;
		size_t nValues = theColours.size();

		ASSERT_INFO( nValues );
		ASSERT( nValues%3 == 0, "not enough values ");

		for( size_t i=0; i<nValues; ++i)
		{
			theColours[i] /= 255.0;
		}
	}

	return theColours;
}
*/

// static
RenMaterial MachPhysSmokePuff::material( MachPhysPuffType puffType )
{
    RenMaterial    result;

    switch( puffType )
    {
        case PUFF_1:
        {
            static RenMaterial puff = puff1();
            result = puff;
        } break;

        case PUFF_2:
        {
            static RenMaterial puff = puff2();
            result = puff;
        } break;

         case PUFF_3:
        {
            static RenMaterial puff = puff3();
            result = puff;
        } break;

        case PUFF_4:
        {
            static RenMaterial puff = puff4();
            result = puff;
        } break;

        case PUFF_5:
        {
            static RenMaterial puff = puff5();
            result = puff;
        } break;

        case PUFF_6:
        {
            static RenMaterial puff = puff6();
            result = puff;
        } break;

		case POD_GREEN:
        {
            static RenMaterial puffMat = puff( RenColour(50.0/255.0, 0, 0) );
            result = puffMat;
        } break;

		case POD_PURPLE:
        {
            static RenMaterial puffMat = puff( RenColour(255.0/255.0, 0, 0) );
            result = puffMat;
        } break;

		case C_FACTORY_GREEN:
        {
            static RenMaterial puffMat = puff( RenColour(100.0/255.0, 100.0/255.0, 255.0/255.0) );
            result = puffMat;
        } break;

		case C_FACTORY_WHITE:
        {
            static RenMaterial puffMat = puff( RenColour(200.0/255.0, 200.0/255.0, 255.0/255.0) );
            result = puffMat;
        } break;


		case M_FACTORY_GREEN:
        {
            static RenMaterial puffMat = puff( RenColour(118.0/255.0, 78.0/255.0, 0) );
            result = puffMat;
        } break;

		case M_FACTORY_BROWN:
        {
            static RenMaterial puffMat = puff( RenColour(255.0/255.0, 150.0/255.0, 0) );
            result = puffMat;
        } break;

		case F_FACTORY_GREEN:
        {
            static RenMaterial puffMat = puff( RenColour(128.0/255.0, 227.0/255.0, 0) );
            result = puffMat;
        } break;

		case F_FACTORY_RED:
        {
            static RenMaterial puffMat = puff( RenColour(118.0/255.0, 78.0/255.0, 0) );
            result = puffMat;
        } break;

		case C_LAB_BLUE:
        {
            static RenMaterial puffMat = puff( RenColour(209.0/255.0, 222.0/255.0, 91) );
            result = puffMat;
        } break;

		case C_LAB_WHITE:
        {
            static RenMaterial puffMat = puff( RenColour(128.0/255.0, 227.0/255.0, 0) );
            result = puffMat;
        } break;

		case M_LAB_BLUE:
        {
            static RenMaterial puffMat = puff( RenColour(70.0/255.0, 196.0/255.0, 254.0/255.0) );
            result = puffMat;
        } break;

		case M_LAB_BROWN:
        {
            static RenMaterial puffMat = puff( RenColour(118.0/255.0, 78.0/255.0, 0) );
            result = puffMat;
        } break;

		case SMELTER_PURPLE:
        {
            static RenMaterial puffMat = puff( RenColour(118.0/255.0, 78.0/255.0, 0) );
            result = puffMat;
        } break;

		case SMELTER_RED:
        {
            static RenMaterial puffMat = puff( RenColour(255.0/255.0, 0, 0) );
            result = puffMat;
        } break;

		case MINE_YELLOW:
        {
            static RenMaterial puffMat = puff( RenColour(255.0/255.0, 150.0/255.0, 0) );
            result = puffMat;
        } break;

		case MINE_BLACK:
        {
            static RenMaterial puffMat = puff( RenColour(0, 0, 0) );
            result = puffMat;
        } break;

		case GARRISON_GREEN:
        {
            static RenMaterial puffMat = puff( RenColour(0, 255.0/255.0, 0) );
            result = puffMat;
        } break;

		case GARRISON_YELLOW:
        {
            static RenMaterial puffMat = puff( RenColour(128.0/255.0, 227.0/255.0, 0) );
            result = puffMat;
        } break;

		case EMPLACEMENT_RED:
        {
            static RenMaterial puffMat = puff( RenColour(255.0/255.0, 62.0/255.0, 0) );
            result = puffMat;
        } break;

		case EMPLACEMENT_YELLOW:
        {
            static RenMaterial puffMat = puff( RenColour(200.0/255.0, 200.0/255.0, 0) );
            result = puffMat;
        } break;

		case BEACON_RED:
        {
            static RenMaterial puffMat = puff( RenColour(0, 0, 255.0/255.0) );
            result = puffMat;
        } break;

		case BEACON_AQUA:
        {
            static RenMaterial puffMat = puff( RenColour(39.0/255.0, 255.0/255.0, 238.0/255.0) );
            result = puffMat;
        } break;

        default:
        {
            ASSERT_BAD_CASE_INFO( puffType );
        } break;
    }

    return result;
}

// static
MachPhysPuffType MachPhysSmokePuff::randomPuff( void )
{
    MachPhysPuffType    result = PUFF_1;

    size_t puff = MachPhysRandom::randomInt( PUFF_3 );

    switch( puff )
    {
        case 0:
            result = PUFF_1;
            break;

        case 1:
            result = PUFF_2;
            break;

        default:
            ASSERT_BAD_CASE_INFO( puff );
            break;
    }

    return result;
}

// static
RenMaterial MachPhysSmokePuff::puff1( void )
{
    return material( "smk03_bt.bmp" );
}

// static
RenMaterial MachPhysSmokePuff::puff2( void )
{
    return material( "smk02_bt.bmp" );
}
// static
RenMaterial MachPhysSmokePuff::puff3( void )
{
	RenColour yellow(0.882353, 0.888235, 0.003922);
    RenMaterial mat=material( "smk03_bt.bmp" );
				mat.emissive(yellow);
				mat.diffuse(RenColour::black());
    return 	mat;
}

// static
RenMaterial MachPhysSmokePuff::puff4( void )
{
	RenColour rust( 0.470588, 0.002922, 0.009608 );
    RenMaterial mat=material( "smk02_bt.bmp" );
				mat.emissive(rust);
				mat.diffuse(RenColour::black());
    return 	mat;
}

// static
RenMaterial MachPhysSmokePuff::puff5( void )
{
	RenColour civilianWhite( 0.862745, 0.870588, 0.8784314 );
    RenMaterial mat=material( "smk03_bt.bmp" );
				mat.emissive(civilianWhite);
				mat.diffuse(RenColour::black());
    return 	mat;
}

// static
RenMaterial MachPhysSmokePuff::puff6( void )
{
	RenColour militaryRed( 66.0/255.0, 0, 0);
    RenMaterial mat=material( "smk02_bt.bmp" );
				mat.emissive(militaryRed);
				mat.diffuse(RenColour::black());
    return 	mat;
}

// static
RenMaterial MachPhysSmokePuff::puff( const RenColour& colour )
{
    RenMaterial mat=material( "smk02_bt.bmp" );
				mat.emissive(colour);
				mat.diffuse(RenColour::black());
    return 	mat;
}

// static
RenMaterial MachPhysSmokePuff::material( const string& textureFileName )
{
    RenTexture texture = RenTexManager::instance().createTexture( textureFileName );

    RenMaterial material( RenColour::black() );
    material.texture( texture );

    return material;
}

void MachPhysSmokePuff::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSmokePuff& t )
{

    o << "MachPhysSmokePuff " << (void*)&t << " start" << std::endl;
    o << "MachPhysSmokePuff " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysSmokePuff::MachPhysSmokePuff( PerConstructor con )
: W4dSprite3d( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysSmokePuff& puff )
{
    const W4dSprite3d& base = puff;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysSmokePuff& puff )
{
    W4dSprite3d& base = puff;

    istr >> base;
}

/* End SMOKPUFF.CPP *************************************************/

