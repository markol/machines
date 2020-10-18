/*
 * R E S C A R R I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/rescarri.hpp"

#include "mathex/point3d.hpp"

#include "machlog/smelter.hpp"
#include "machlog/mine.hpp"


PER_DEFINE_PERSISTENT( MachLogResourceCarrierImpl );

MachLogResourceCarrierImpl::MachLogResourceCarrierImpl()
: amountCarried_( 0 ),
  isTransporting_( false ),
  isPickingUp_( false ),
  isPuttingDown_( false ),
  smeltingBuildingExplicitlyAssigned_( false ),
  iterations_( MachLogResourceCarrier::FOREVER ),
  currentElement_( 0 ),
  protectNodesFromClearance_( false ),	
  pDestinationSmeltingBuilding_( NULL )
{
	suppliers_.reserve( 10 );
    TEST_INVARIANT;
}

MachLogResourceCarrierImpl::~MachLogResourceCarrierImpl()
{
    TEST_INVARIANT;
}

void MachLogResourceCarrierImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogResourceCarrierImpl& t )
{

    o << "MachLogResourceCarrierImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogResourceCarrierImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogResourceCarrierImpl& actorImpl )
{
	ostr << actorImpl.amountCarried_;
	ostr << actorImpl.isTransporting_;
	ostr << actorImpl.isPickingUp_;						
	ostr << actorImpl.isPuttingDown_;	
	ostr << actorImpl.suppliers_;
	ostr << actorImpl.pDestinationSmeltingBuilding_;												
	ostr << actorImpl.smeltingBuildingExplicitlyAssigned_;						
	ostr << actorImpl.iterations_;	
	ostr << actorImpl.protectNodesFromClearance_;							
	ostr << actorImpl.currentElement_;								
}

void perRead( PerIstream& istr, MachLogResourceCarrierImpl& actorImpl )
{
	istr >> actorImpl.amountCarried_;
	istr >> actorImpl.isTransporting_;	
	istr >> actorImpl.isPickingUp_;						
	istr >> actorImpl.isPuttingDown_;
	istr >> actorImpl.suppliers_;
	istr >> actorImpl.pDestinationSmeltingBuilding_;						
	istr >> actorImpl.smeltingBuildingExplicitlyAssigned_;
	istr >> actorImpl.iterations_;
	istr >> actorImpl.protectNodesFromClearance_;
	istr >> actorImpl.currentElement_;									
}

// static
MachLogResourceCarrierImpl::Suppliers::iterator
MachLogResourceCarrierImpl::iNearestSupplier( Suppliers& listOfCandidateSuppliers,
										  	  const MexPoint2d& position )
{
	PRE( not listOfCandidateSuppliers.empty() );

	MATHEX_SCALAR lowestDistanceFound = 100000000;

	Suppliers::iterator iNearestSupplierFound = listOfCandidateSuppliers.end();		// default assignment

	for( Suppliers::iterator iSup = listOfCandidateSuppliers.begin(); iSup != listOfCandidateSuppliers.end(); ++iSup )
	{
		MATHEX_SCALAR sqrDistanceFromposition = 
			position.sqrEuclidianDistance( (*iSup)->position() );

		if( sqrDistanceFromposition < lowestDistanceFound )
		{
			iNearestSupplierFound = iSup;
			lowestDistanceFound = sqrDistanceFromposition;
		}
	}	

	POST( iNearestSupplierFound != listOfCandidateSuppliers.end() );

	return iNearestSupplierFound;
}	

/* End RESCARRI.CPP *************************************************/
