/*
 * M I N E S I T I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/minesiti.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/oreholo.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogMineralSiteImpl );

MachLogMineralSiteImpl::MachLogMineralSiteImpl( const MachPhys::MineralGrade& grade ,
												const MachPhys::BuildingMaterialUnits& amount,
												const MexPoint3d& p)
: state_( MachLogMineralSite::UNDISCOVERED ),
  position_( p ),
  amountOfOre_( amount ),
  grade_( grade ),
  pOreHolo_( NULL ),
  id_( -1 ),
  registeredAsExhausted_( false )
{
	position_ = MachLogSpacialManipulation::heightAdjustedLocation( p );
}

MachLogMineralSiteImpl::~MachLogMineralSiteImpl()
{
    TEST_INVARIANT;

}

void MachLogMineralSiteImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMineralSiteImpl& t )
{

    o << "MachLogMineralSiteImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogMineralSiteImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMineralSiteImpl& siteImpl )
{
	ostr << siteImpl.state_;
	ostr << siteImpl.position_;
	ostr << siteImpl.amountOfOre_;
	ostr << siteImpl.grade_;
	ostr << siteImpl.discoveredBy_;
	ostr << siteImpl.registeredAsExhausted_;
	ostr << siteImpl.id_;

    //Stream out the id to use for the ore holograph.
    UtlId holoId = (siteImpl.pOreHolo_ == NULL ? 0 : siteImpl.pOreHolo_->id());
    PER_WRITE_RAW_OBJECT( ostr, holoId );
}

void perRead( PerIstream& istr, MachLogMineralSiteImpl& siteImpl )
{
 	istr >> siteImpl.state_;
	istr >> siteImpl.position_;
	istr >> siteImpl.amountOfOre_;
	istr >> siteImpl.grade_;
	istr >> siteImpl.discoveredBy_;
	istr >> siteImpl.registeredAsExhausted_;
	istr >> siteImpl.id_;

    //Read in the ore holo id. If non-zero, construct a new ore holograph
    UtlId holoId;
    PER_READ_RAW_OBJECT( istr, holoId );
    if( holoId != 0 )
    {
        siteImpl.pOreHolo_ =
            MachLogActorMaker::newLogOreHolograph( siteImpl.discoveredBy_, 0, 0, siteImpl.position_, holoId );
    }
}

MachLogMineralSiteImpl::MachLogMineralSiteImpl( PerConstructor )
: pOreHolo_( NULL )
{
}
/* End MINESITI.CPP ***************************************************/
