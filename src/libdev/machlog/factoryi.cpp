/*
 * F A C T O R Y I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/factoryi.hpp"
#include "sim/manager.hpp"
#include "machlog/produnit.hpp"

PER_DEFINE_PERSISTENT( MachLogFactoryImpl );

MachLogFactoryImpl::MachLogFactoryImpl( MachPhys::FactorySubType subType )
:	amountBuilt_( 0 ),
	subType_( subType ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	deployPointIsSet_( false ),
	specifiedDeployPoint_( 0,0 )
{

    TEST_INVARIANT;
}

MachLogFactoryImpl::~MachLogFactoryImpl()
{
    TEST_INVARIANT;

}

void MachLogFactoryImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogFactoryImpl& t )
{

    o << "MachLogFactoryImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogFactoryImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogFactoryImpl& actorImpl )
{
	ostr << actorImpl.productionLine_;
	ostr << actorImpl.amountBuilt_;
	ostr << actorImpl.lastUpdateTime_;
	ostr << actorImpl.subType_;
	ostr << actorImpl.specifiedDeployPoint_;
	ostr << actorImpl.deployPointIsSet_;
}

void perRead( PerIstream& istr, MachLogFactoryImpl& actorImpl )
{
	istr >> actorImpl.productionLine_;
	istr >> actorImpl.amountBuilt_;
	istr >> actorImpl.lastUpdateTime_;
	istr >> actorImpl.subType_;
	istr >> actorImpl.specifiedDeployPoint_;
	istr >> actorImpl.deployPointIsSet_;
}

MachLogFactoryImpl::MachLogFactoryImpl( PerConstructor )
{
}
/* End FACTORYI.CPP *************************************************/
