/*
 * S T A T I M P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "utility/linetok.hpp"
#include "machlog/internal/statimpl.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

MachLogStatsImpl::MachLogStatsImpl()
:	aiPriority_( 1 ),
	pcPriority_( 1 ),
	maxMoveTime_( 0.010 ),
	potentialKillRadiusMultiplier_ ( 2.0 ),
	cancelledRefundablePercentage_ ( 0.75 ),
	secondhandRefundablePercentage_ ( 0.5 ),
	friendlyDeconstructMultiplier_ ( 3.0 ),
	enemyDeconstructMultiplier_ ( 1.0 ),
	enemyCaptureMultiplier_ ( 2.0 ),
	connectionTime_( 120 ),
	scannerRangeToWeaponRangeRatio_( 1.4 ),
    resyncThreshold_( 0.6 ),
	nuclearMissileBlastRange_( 50 )
{
	readStatsInitialisationFile();

    TEST_INVARIANT;
}

MachLogStatsImpl::~MachLogStatsImpl()
{
    TEST_INVARIANT;

}

void MachLogStatsImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachLogStatsImpl::readStatsInitialisationFile()
{
	HAL_STREAM("MLStatsImpl::readStatsIni\n" );

	const SysPathName pathName( "data/stats.ini" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, pathName, ios::text ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, pathName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( pathName.c_str() );
        //pIstream = _NEW( ifstream( pathName.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( pathName.c_str(),std::ios::in ) ));
    }

	UtlLineTokeniser parser( *pIstream, pathName );
	while( not parser.finished() )
	{
		HAL_STREAM("parser.tokens.size " << parser.tokens().size() << std::endl );
		HAL_STREAM(" " << parser.tokens()[0] << std::endl );
		if( parser.tokens()[0] == "AI_PRIORITY" )
			aiPriority_ = atol( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "PC_PRIORITY" )
			pcPriority_ = atol( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "MAX_MOVE_TIME" )
			maxMoveTime_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "POTENTIAL_KILL_RADIUS" )
			potentialKillRadiusMultiplier_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "CANCEL_REFUND" )
			cancelledRefundablePercentage_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "RECYCLE_REFUND" )
			secondhandRefundablePercentage_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "FRIstd::endlY_DECONSTRUCT_RATE_MULT" )
			friendlyDeconstructMultiplier_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "ENEMY_DECONSTRUCT_RATE_MULT" )
			enemyDeconstructMultiplier_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "ENEMY_CAPTURE_RATE_MULT" )
			enemyCaptureMultiplier_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "CONNECTION_TIME" )
			connectionTime_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "TURRET_SCANNER_RANGE_MULT" )
			scannerRangeToWeaponRangeRatio_ = atof( parser.tokens()[1].c_str() );
		else if( parser.tokens()[0] == "NUCLEAR_MISSILE_BLAST_RANGE" )
			nuclearMissileBlastRange_ = atof( parser.tokens()[1].c_str() );
		parser.parseNextLine();
	}
}

ostream& operator <<( ostream& o, const MachLogStatsImpl& t )
{

    o << "MachLogStatsImpl " << (void*)&t << " start" << std::endl;
	o << " aiPriority " << t.aiPriority_ << std::endl;
	o << " pcPriority " << t.pcPriority_ << std::endl;
	o << " maxMoveTime " << t.maxMoveTime_ << std::endl;
	o << " potentialKillRadiusMultiplier_ " << t.potentialKillRadiusMultiplier_ << std::endl;
	o << " cancelledRefundablePercentage_ " << t.cancelledRefundablePercentage_ << std::endl;
	o << " secondhandRefundablePercentage_ " << t.secondhandRefundablePercentage_ << std::endl;
	o << " friendlyDeconstructMultiplier_ " << t.friendlyDeconstructMultiplier_ << std::endl;
	o << " enemyDeconstructMultiplier_ " << t.enemyDeconstructMultiplier_ << std::endl;
	o << " enemyCaptureMultiplier_ " << t.enemyCaptureMultiplier_ << std::endl;
	o << " connectionTime_ " << t.connectionTime_ << std::endl;
	o << " scannerRangeToWeaponRangeRatio_ " << t.scannerRangeToWeaponRangeRatio_ << std::endl;
    o << "MachLogStatsImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End STATIMPL.CPP *************************************************/
