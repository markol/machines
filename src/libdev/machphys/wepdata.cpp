/*
 * W E P D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/wepdata.hpp"

#ifndef _INLINE
    #include "machphys/wepdata.ipp"
#endif

#include "system/pathname.hpp"
#include "render/texmgr.hpp"
#include "machphys/effects.hpp"

MachPhysWeaponData::MachPhysWeaponData()
:   damagePoints_( 1 ), //Use sensible defaults
    range_( 30.0 ),
    projectileSpeed_( 50.0 ),
    nRoundsPerBurst_( 1 ),
	fixedBurst_( true ),
    burstDuration_( 0.2 ),
    reloadTime_( 1.0 ),
    recoilDistance_( 0.0 ),
    recoilBackTime_( 0.05 ),
    recoilForeTime_( 0.15 ),
    minTiltAngle_( 0.0 ),
    maxTiltAngle_( 0.0 ),
    verticalDeviationAngle_( MexDegrees( 10.0 ) ),
    horizontalDeviationAngle_( MexDegrees( 10.0 ) ),
	flameRotate_( true ),
	flameScale_( false ),
	flameScaleFrom_( 1 ),
	flameScaleTo_( 1 ),
	trailOn_( true ),
	trailColour_( RenColour::white() ),
	trailTextureKey_(""),
	trailStartTextureIndex_(0),
	trailEndTextureIndex_(0)

{
    launchOffsets_.reserve( 1 );
    launchOffsets_.push_back( MexPoint3d( 0, 0, 0 ) );

    TEST_INVARIANT;
}

MachPhysWeaponData::~MachPhysWeaponData()
{
    TEST_INVARIANT;

}

void MachPhysWeaponData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWeaponData& t )
{

    o << "MachPhysWeaponData " << (void*)&t << " start" << std::endl;
    o << "  Damage " << t.damagePoints_ << "  range " << t.range_
      << "  speed " << t.projectileSpeed_ << std::endl;
    o << "  Rounds per burst " << t.nRoundsPerBurst_ << " duration " << t.burstDuration_
      << "  reload time " << t.reloadTime_ << std::endl;
    o << "MachPhysWeaponData " << (void*)&t << " end" << std::endl;

    return o;
}

double MachPhysWeaponData::sinVerticalDeviationAngle() const
{
    //TBD: should be cached
    return sin( verticalDeviationAngle_ );
}

double MachPhysWeaponData::sinHorizontalDeviationAngle() const
{
    //TBD: should be cached
    return sin( horizontalDeviationAngle_ );
}

double MachPhysWeaponData::tanVerticalDeviationAngle() const
{
    //TBD: should be cached
    return tan( verticalDeviationAngle_ );
}

double MachPhysWeaponData::tanHorizontalDeviationAngle() const
{
    //TBD: should be cached
    return tan( horizontalDeviationAngle_ );
}

bool MachPhysWeaponData::trailOn() const
{
	return trailOn_;
}

const RenColour& MachPhysWeaponData::trailColour() const
{
	return trailColour_;
}

ctl_vector< RenTexture > MachPhysWeaponData::trailTextures() const
{
    ctl_vector< RenTexture > textures;

	if( trailOn() and trailTextureKey_ != "")
	{
		string nameRoot;
		string extension;
		size_t nameLength = trailTextureKey_.length();

		if( trailTextureKey_.substr(nameLength-2, 2) == "_b" )
		{
			nameRoot = trailTextureKey_.substr(0, nameLength-2);
			extension = "_b.bmp";
		}
		else if( trailTextureKey_.substr(nameLength-3, 3) == "_bt" )
		{
			nameRoot = trailTextureKey_.substr(0, nameLength-3);
			extension = "_bt.bmp";
		}
		else
		{
			nameRoot = trailTextureKey_;
			extension = ".bmp";
		}


		if( trailEndTextureIndex_ == 0 and trailStartTextureIndex_ == 0 )
		{
			ctl_vector<RenTexture> trailTextures;
			trailTextures.reserve(1);


			string textureName = nameRoot + extension;
			trailTextures.push_back( RenTexManager::instance().createTexture( textureName ) );

			textures = trailTextures;
		}
		else
		{
			ASSERT(trailEndTextureIndex_ >= trailStartTextureIndex_, "" );

			size_t nTextures = trailEndTextureIndex_ - trailStartTextureIndex_ + 1;
			ASSERT( nTextures < 100, "" );

			ctl_vector<RenTexture> trailTextures;
			trailTextures.reserve(nTextures);


		    for( int i = trailStartTextureIndex_; i < trailEndTextureIndex_+1; ++i )
			{
				string textureName = nameRoot;

				if(i < 10 )
				{
					textureName += "0";
				}

				char textN[4];
//				itoa(i, textN, 10 );
                sprintf(textN, "%d", i);
				textureName += string(textN);

				textureName += extension;
		        trailTextures.push_back( RenTexManager::instance().createTexture( textureName ) );

			}

			textures = trailTextures;
		}
	}
	else
	{
		textures = MachPhysEffects::smokeTextures();
	}

	return textures;
}

bool MachPhysWeaponData::flameRotate() const
{
	return flameRotate_;
}

bool MachPhysWeaponData::flameScale() const
{
	return flameScale_;
}

MATHEX_SCALAR MachPhysWeaponData::flameScaleFrom() const
{
	return flameScaleFrom_;
}

MATHEX_SCALAR MachPhysWeaponData::flameScaleTo() const
{
	return flameScaleTo_;
}

void MachPhysWeaponData::trailOn( bool on )
{
	trailOn_ = on;
}

void MachPhysWeaponData::trailColour(const RenColour&  col )
{
	trailColour_ = col;
}

void MachPhysWeaponData::trailTextureKey( const string& key )
{
	trailTextureKey_ = key;
}

void MachPhysWeaponData::trailStartTextureIndex( size_t startIndex )
{
	trailStartTextureIndex_ = startIndex;
}

void MachPhysWeaponData::trailEndTextureIndex( size_t endIndex)
{
	trailEndTextureIndex_ = endIndex;
}


void MachPhysWeaponData::flameRotate( bool trueFalse)
{
	flameRotate_ = trueFalse;
}

void MachPhysWeaponData::flameScale( bool trueFalse)
{
	flameScale_ = trueFalse;
}

void MachPhysWeaponData::flameScaleFrom( MATHEX_SCALAR startScale )
{
	flameScaleFrom_ = startScale;
}

void MachPhysWeaponData::flameScaleTo( MATHEX_SCALAR endScale)
{
	flameScaleTo_ = endScale;
}

bool MachPhysWeaponData::fixedBurst() const
{
	return fixedBurst_;
}

void MachPhysWeaponData::fixedBurst( bool status )
{
	fixedBurst_ = status;
}


/* End WEPDATA.CPP **************************************************/
