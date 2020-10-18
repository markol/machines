/*
 * S T A T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/stats.hpp"
#include "machlog/internal/statimpl.hpp"

MachLogStats::MachLogStats()
:	pStatsImpl_( _NEW( MachLogStatsImpl ) )
{
    TEST_INVARIANT;
}

MachLogStats::~MachLogStats()
{
	_DELETE( pStatsImpl_ );
    TEST_INVARIANT;

}

void MachLogStats::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

int MachLogStats::pcPriority() const
{
	return pStatsImpl_->pcPriority_;
}

int MachLogStats::aiPriority() const
{
	return pStatsImpl_->aiPriority_;
}

PhysRelativeTime MachLogStats::maxMoveTime() const
{
	return pStatsImpl_->maxMoveTime_;
}

MATHEX_SCALAR MachLogStats::potentialKillRadiusMultiplier() const
{
	return pStatsImpl_->potentialKillRadiusMultiplier_;
}

float MachLogStats::cancelledRefundablePercentage() const
{
	return pStatsImpl_->cancelledRefundablePercentage_;
}

float MachLogStats::secondhandRefundablePercentage() const
{
	return pStatsImpl_->secondhandRefundablePercentage_;
}

double MachLogStats::friendlyDeconstructMultiplier() const
{
 	return pStatsImpl_->friendlyDeconstructMultiplier_;
}

double MachLogStats::enemyDeconstructMultiplier() const
{
	return pStatsImpl_->enemyDeconstructMultiplier_;
}

double MachLogStats::enemyCaptureMultiplier() const
{
	return pStatsImpl_->enemyCaptureMultiplier_;
}

PhysAbsoluteTime MachLogStats::connectionTime() const
{
	return pStatsImpl_->connectionTime_;
}

double MachLogStats::scannerRangeToWeaponRangeRatio() const
{
	return pStatsImpl_->scannerRangeToWeaponRangeRatio_;
}

ostream& operator <<( ostream& o, const MachLogStats& t )
{

    o << "MachLogStats " << (void*)&t << " start" << std::endl;
	o << *t.pStatsImpl_ << std::endl;
    o << "MachLogStats " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachLogStats::resyncThreshold() const
{
    return pStatsImpl_->resyncThreshold_;
}

void MachLogStats::resyncThreshold( PhysRelativeTime threshold )
{
    PRE( threshold >= 0.0 );
    pStatsImpl_->resyncThreshold_ = threshold;
}

double MachLogStats::nuclearMissileBlastRange() const
{
	return pStatsImpl_->nuclearMissileBlastRange_;
}
/* End STATS.CPP ****************************************************/
