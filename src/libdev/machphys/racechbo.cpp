/*
 * R A C E C H B O . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "ctl/map.hpp"
#include "system/pathname.hpp"

#include "render/matvec.hpp"
#include "render/matmap.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

#include "world4d/mveccple.hpp"
#include "world4d/fwmateri.hpp"
#include "world4d/mvecccha.hpp"
#include "machphys/mccospec.hpp"
#include "machphys/internal/racechbo.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/machdata.hpp"
#include "machphys/consdata.hpp"
#include "machphys/weapon.hpp"

#include "utility/linetok.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

MachPhysRaceChangerBody::MachPhysRaceChangerBody()
:   pMachineChangerMap_( _NEW( MachineChangerMap() ) ),
    pConstructionChangerMap_( _NEW( ConstructionChangerMap() ) ),
    pWeaponChangerMap_( _NEW( WeaponChangerMap() ) ),
    pChangerRaces_( _NEW( Races() ) ),
    pMaterialVecPtrSet_( _NEW( W4dMaterialVecPtrSet ) )
{
    //Set up the list of races in use. red is needed for traitor animations
    pChangerRaces_->reserve( 4 );
    pChangerRaces_->push_back( MachPhys::RED );
    pChangerRaces_->push_back( MachPhys::BLUE );
    pChangerRaces_->push_back( MachPhys::GREEN );
    pChangerRaces_->push_back( MachPhys::YELLOW );

    TEST_INVARIANT;
}

MachPhysRaceChangerBody::~MachPhysRaceChangerBody()
{
    TEST_INVARIANT;

    //Iterate through the maps deleting entries
    for( MachineChangerMap::iterator it = pMachineChangerMap_->begin();
         it != pMachineChangerMap_->end(); ++it )
    {
        _DELETE( (*it).second );
    }

    for( ConstructionChangerMap::iterator it = pConstructionChangerMap_->begin();
         it != pConstructionChangerMap_->end(); ++it )
    {
        _DELETE( (*it).second );
    }

    for( WeaponChangerMap::iterator it = pWeaponChangerMap_->begin();
         it != pWeaponChangerMap_->end(); ++it )
    {
        _DELETE( (*it).second );
    }

    _DELETE( pMachineChangerMap_ );
    _DELETE( pConstructionChangerMap_ );
    _DELETE( pWeaponChangerMap_ );
    _DELETE( pChangerRaces_ );
    _DELETE( pMaterialVecPtrSet_ );
}

void MachPhysRaceChangerBody::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysRaceChangerBody& t )
{

    o << "MachPhysRaceChangerBody " << (void*)&t << " start" << std::endl;
    o << "MachPhysRaceChangerBody " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysRaceChangerBody::add
(
    const MachPhysMachine& machine, const MachPhysMachineData& data
)
{
    //Need to create new map entries for this machine for each change race.
    //Set up a vector of material maps to store the conversion maps for each race for
    //this composite. Also a vector of the hues for each race.
    const Races& changerRaces = *pChangerRaces_;
    uint nRaces = changerRaces.size();

    ctl_pvector< RenMaterialMap > materialMaps;
    materialMaps.reserve( nRaces );

    ctl_vector< double > changerHues;
    changerHues.reserve( nRaces );

    for( size_t i = 0; i != nRaces; ++i )
    {
        materialMaps.push_back( _NEW( RenMaterialMap ) );
        changerHues.push_back( hue( changerRaces[i] ) );
    }

    //Set up the material maps
    W4dCompositeMaterialVecChanger::fillMaterialMaps
    (
        machine, hue( MachPhys::RED ), changerHues, keyTextures(), texturesVec(), materialMaps
    );

    //Set up a machine spec using the machine attribute data
    MachPhysMachineSpec spec = machineSpec( data );

    //Create the composite changers and add to the main map
    for( size_t i = 0; i != nRaces; ++i )
    {
        //Create the composite changer
        RenMaterialMap* pRaceMaterialMap = materialMaps[i];
        W4dCompositeMaterialVecChanger* pChanger =
            _NEW( W4dCompositeMaterialVecChanger( machine, *pRaceMaterialMap,
                                                  pMaterialVecPtrSet_ ) );

        //Insert into map
        spec.race( changerRaces[i] );
        pMachineChangerMap_->insert( spec, pChanger );

        //Delete the material map
        _DELETE( pRaceMaterialMap );
    }
}

void MachPhysRaceChangerBody::add
(
    const MachPhysConstruction& construction, const MachPhysConstructionData& data
)
{
    //Need to create new map entries for this construction for each change race.
    //Set up a vector of material maps to store the conversion maps for each race for
    //this composite. Also a vector of the hues for each race.
    const Races& changerRaces = *pChangerRaces_;
    uint nRaces = changerRaces.size();

    ctl_pvector< RenMaterialMap > materialMaps;
    materialMaps.reserve( nRaces );

    ctl_vector< double > changerHues;
    changerHues.reserve( nRaces );

    for( size_t i = 0; i != nRaces; ++i )
    {
        materialMaps.push_back( _NEW( RenMaterialMap ) );
        changerHues.push_back( hue( changerRaces[i] ) );
    }

    //Set up the material maps
    W4dCompositeMaterialVecChanger::fillMaterialMaps
    (
        construction, hue( MachPhys::RED ), changerHues, keyTextures(), texturesVec(), materialMaps
    );

    //Set up a construction spec using the machine attribute data
    MachPhysConstructionSpec spec = constructionSpec( data );

    //Create the composite changers and add to the main map
    for( size_t i = 0; i != nRaces; ++i )
    {
        //Create the composite changer
        RenMaterialMap* pRaceMaterialMap = materialMaps[i];
        W4dCompositeMaterialVecChanger* pChanger =
            _NEW( W4dCompositeMaterialVecChanger( construction, *pRaceMaterialMap,
                                                  pMaterialVecPtrSet_ ) );

        //Insert into map
        spec.race( changerRaces[i] );
        pConstructionChangerMap_->insert( spec, pChanger );

        //Delete the material map
        _DELETE( pRaceMaterialMap );
    }
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChangerBody::changer
(
    const MachPhysMachine& machine, const MachPhysMachineData& data, MachPhys::Race toRace
) const
{
    //Set up a machine spec from the machine attribute data
    MachPhysMachineSpec spec = machineSpec( data );
    spec.race( toRace );

    //Look it up in the map
    MachineChangerMap::const_iterator it = pMachineChangerMap_->find( spec );
    if( it == pMachineChangerMap_->end() )
    {
        //Not registered yet, so register it
        _CONST_CAST( MachPhysRaceChangerBody*, this )->add( machine, data );

        //Now try again to find the changer we want
        it = pMachineChangerMap_->find( spec );
    }

    ASSERT( it != pMachineChangerMap_->end(), "Machine not found in race changer" );
    return *((*it).second);
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChangerBody::changer
(
    const MachPhysConstruction& construction, const MachPhysConstructionData& data,
    MachPhys::Race toRace
) const
{
    //Set up a construction spec from the attribute data
    MachPhysConstructionSpec spec = constructionSpec( data );
    spec.race( toRace );

    //Look it up in the map
    ConstructionChangerMap::const_iterator it = pConstructionChangerMap_->find( spec );
    if( it == pConstructionChangerMap_->end() )
    {
        //Not registered yet, so register it
        _CONST_CAST( MachPhysRaceChangerBody*, this )->add( construction, data );

        //Now try again to find the changer we want
        it = pConstructionChangerMap_->find( spec );
    }

    ASSERT( it != pConstructionChangerMap_->end(), "Construction not found in race changer" );
    return *((*it).second);
}

//static
MachPhysMachineSpec MachPhysRaceChangerBody::machineSpec( const MachPhysMachineData& data )
{
    //Get all the info needed to construct the spec.
    //Models are not differentiated by software level, so use sw level 1.
    const uint swLevel = 1;
    return MachPhysMachineSpec( data.machineType(), data.subType(), data.hwLevel(),
                                swLevel, MachPhys::RED );
}

//static
MachPhysConstructionSpec MachPhysRaceChangerBody::constructionSpec
(
    const MachPhysConstructionData& data
)
{
    //Get all the info needed to construct the spec.
    return MachPhysConstructionSpec( data.constructionType(), data.subType(), data.hwLevel(),
                                     MachPhys::RED );
}

double MachPhysRaceChangerBody::hue( MachPhys::Race race ) const
{
    double result = 0;

    switch( race )
    {
        case MachPhys::RED:
            result = 0.0;
            break;
        case MachPhys::BLUE:
            result = 222.0;
            break;

        case MachPhys::GREEN:
            result = 71.0;
            break;
        case MachPhys::YELLOW:
            result = 50.0;
            break;
    }

    return result;
}

void MachPhysRaceChangerBody::add( const MachPhysWeapon& weapon )
{
    //Need to create new map entries for this weapon for each change race.
    //Set up a vector of material maps to store the conversion maps for each race for
    //this composite. Also a vector of the hues for each race.
    const Races& changerRaces = *pChangerRaces_;
    uint nRaces = changerRaces.size();

    ctl_pvector< RenMaterialMap > materialMaps;
    materialMaps.reserve( nRaces );

    ctl_vector< double > changerHues;
    changerHues.reserve( nRaces );

    for( size_t i = 0; i != nRaces; ++i )
    {
        materialMaps.push_back( _NEW( RenMaterialMap ) );
        changerHues.push_back( hue( changerRaces[i] ) );
    }

    //Set up the material maps
    W4dCompositeMaterialVecChanger::fillMaterialMaps
    (
        weapon, hue( MachPhys::RED ), changerHues, keyTextures(), texturesVec(), materialMaps
    );

    //Set up a weapon spec using the weapon type
    MachPhysWeaponSpec spec = weaponSpec( weapon.type() );

    //Create the composite changers and add to the main map
    for( size_t i = 0; i != nRaces; ++i )
    {
        //Create the composite changer
        RenMaterialMap* pRaceMaterialMap = materialMaps[i];
        W4dCompositeMaterialVecChanger* pChanger =
            _NEW( W4dCompositeMaterialVecChanger( weapon, *pRaceMaterialMap,
                                                  pMaterialVecPtrSet_ ) );

        //Insert into map
        spec.race( changerRaces[i] );
        pWeaponChangerMap_->insert( spec, pChanger );

        //Delete the material map
        _DELETE( pRaceMaterialMap );
    }
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChangerBody::changer
(
    const MachPhysWeapon& weapon, MachPhys::Race toRace
) const
{
    //Set up a weapon spec from the weapon attribute data
    MachPhysWeaponSpec spec = weaponSpec( weapon.type() );
    spec.race( toRace );

    //Look it up in the map
    WeaponChangerMap::const_iterator it = pWeaponChangerMap_->find( spec );
    if( it == pWeaponChangerMap_->end() )
    {
        //Not registered yet, so register it
        _CONST_CAST( MachPhysRaceChangerBody*, this )->add( weapon );

        //Now try again to find the changer we want
        it = pWeaponChangerMap_->find( spec );
    }

    ASSERT( it != pWeaponChangerMap_->end(), "Weapon not found in race changer" );
    return *((*it).second);
}

//static
MachPhysWeaponSpec MachPhysRaceChangerBody::weaponSpec( MachPhys::WeaponType type )
{
    return MachPhysWeaponSpec( type, MachPhys::RED );
}

//static
const MachPhysRaceChangerBody::TexturesVec& MachPhysRaceChangerBody::texturesVec()
{
	static MachPhysRaceChangerBody::TexturesVec texturesVec;
	static bool first = true;

	if( first )
	{
		first = false;

		texturesVec.reserve( 10 );
	    static const SysPathName logoFilename = "data/logos.dat";

        SysMetaFile metaFile( "mach1.met" );

        std::auto_ptr< istream > pIstream;

        if( SysMetaFile::useMetaFile() )
        {
            //pIstream = _NEW( SysMetaFileIstream( metaFile, logoFilename, std::ios::text ) );
            pIstream = std::auto_ptr< istream > (
                _NEW( SysMetaFileIstream( metaFile, logoFilename, std::ios::in ) ));
        }
        else
        {
            ASSERT_FILE_EXISTS( logoFilename.c_str() );
            //pIstream = _NEW( ifstream( logoFilename.c_str(), ios::text | ios::in ) );
            pIstream = std::auto_ptr< istream > (
                _NEW( std::ifstream( logoFilename.c_str(), std::ios::in ) ) );
        }

	    UtlLineTokeniser   parser( *pIstream, logoFilename );

	    ASSERT( not parser.finished(), "Logo file empty" );


	    while( not parser.finished() )
	    {
	    	string logoName = parser.tokens()[ 0 ];

			size_t nameLength = logoName.length();

			ASSERT( nameLength > 6, " shortest name is X1.bmt" );

			string nameRoot;
			string extension;

			if( logoName.substr(nameLength-6, 6) == "_b.bmp" )
			{
				nameRoot = logoName.substr(0, nameLength-7);
				extension = "_b.bmp";
			}
			else if( logoName.substr(nameLength-7, 7) == "_bt.bmp" )
			{
				nameRoot = logoName.substr(0, nameLength-8);
				extension = "_bt.bmp";
			}
			else if( logoName.substr(nameLength-4, 4) == ".bmp" )
			{
				nameRoot = logoName.substr(0, nameLength-5);
				extension = ".bmp";
			}
			else
			{
				ASSERT_INFO( logoName );
				ASSERT( false, " logName does not end with _b(t).bmp/.bmp " );
			}

			const uint nTextures = 4;

			MachPhysRaceChangerBody::Textures textures;
			textures.reserve(nTextures);

		    for( int i = 1; i <= nTextures; ++i )
			{
				string textureName = nameRoot;

				char textN[4];
//				itoa(i, textN, 10 );
                sprintf(textN, "%d", i);
				textureName += string(textN);

				textureName += extension;

		        textures.push_back( RenTexManager::instance().createTexture( textureName ) );

			}

			texturesVec.push_back( textures );
		    parser.parseNextLine();
		}
	}

	return texturesVec;
}

const MachPhysRaceChangerBody::Textures& MachPhysRaceChangerBody::keyTextures()
{
	static MachPhysRaceChangerBody::Textures textures;
	static bool first = true;

	if( first )
	{
		first = false;

		const size_t size = MachPhysRaceChangerBody::texturesVec().size();
		textures.reserve( size );

		for( uint i=0; i<size; ++i)
		{
			const MachPhysRaceChangerBody::Textures& iTextures = MachPhysRaceChangerBody::texturesVec()[i];
			textures.push_back(iTextures[0]);
		}
	}

	return textures;
}

/* End RACECHBO.CPP *************************************************/
