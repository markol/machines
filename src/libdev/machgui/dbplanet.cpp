/*
 * D B P L A N E T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/internal/dbplanei.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbPlanet );

MachGuiDbPlanet::MachGuiDbPlanet( const string& planetName, uint menuStringId )
:   MachGuiDbElement( menuStringId )
{
    pData_ = _NEW( MachGuiDbIPlanet );
    name( planetName );
    isCustom(false);

    TEST_INVARIANT;
}

MachGuiDbPlanet::~MachGuiDbPlanet()
{
    TEST_INVARIANT;

    //Delete all the dependent scenarios
    for( size_t i = pData_->scenarios_.size(); i--; )
    {
        _DELETE( pData_->scenarios_[i] );
    }

    _DELETE( pData_ );
}

void MachGuiDbPlanet::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbPlanet& t )
{

    o << "MachGuiDbPlanet " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbPlanet " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiDbPlanet::campaignPicture( const string& filename )
{
    pData_->campaignPicture_ = filename;
}

const string& MachGuiDbPlanet::campaignPicture() const
{
    return pData_->campaignPicture_;
}

uint MachGuiDbPlanet::nScenarios() const
{
    return pData_->scenarios_.size();
}

MachGuiDbScenario& MachGuiDbPlanet::scenario( uint index )
{
    PRE( index < nScenarios() );

    return *(pData_->scenarios_[ index ]);
}

void MachGuiDbPlanet::addScenario( MachGuiDbScenario* pScenario )
{
    //Add to own list
    pData_->scenarios_.push_back( pScenario );

    //Set the back pointer in the scenario
    pScenario->planet( this );
}

void MachGuiDbPlanet::system( MachGuiDbSystem* pSystem )
{
    pData_->pSystem_ = pSystem;
}

MachGuiDbSystem& MachGuiDbPlanet::system() const
{
    PRE( pData_->pSystem_ != NULL );
    return *(pData_->pSystem_);
}

void perWrite( PerOstream& ostr, const MachGuiDbPlanet& ob )
{
    const MachGuiDbElement& base = ob;

    ostr << base;

    ostr << ob.pData_;
}

void perRead( PerIstream& istr, MachGuiDbPlanet& ob )
{
    MachGuiDbElement& base = ob;

    istr >> base;

    istr >> ob.pData_;
}

MachGuiDbPlanet::MachGuiDbPlanet( PerConstructor con )
:   MachGuiDbElement( con ),
    pData_( NULL )
{
}
/* End DBPLANET.CPP *************************************************/
