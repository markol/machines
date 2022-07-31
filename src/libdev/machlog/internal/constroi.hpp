/*
 * C O N S T R O I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogConstructionImpl

    construction base class implementation class.
*/

#ifndef _MACHLOG_CONSTROI_HPP
#define _MACHLOG_CONSTROI_HPP

#include "base/base.hpp"
#include "machlog/constron.hpp"
#include "machlog/buildpnt.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"


class MachLogConstructionImpl
// Canonical form revoked
{
public:
    MachLogConstructionImpl();
    ~MachLogConstructionImpl();
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogConstructionImpl );
	PER_FRIEND_READ_WRITE( MachLogConstructionImpl );

    void CLASS_INVARIANT;

private:
	friend class MachLogConstruction;
    friend ostream& operator <<( ostream& o, const MachLogConstructionImpl& t );

    MachLogConstructionImpl( const MachLogConstructionImpl& );
    MachLogConstructionImpl& operator =( const MachLogConstructionImpl& );

    MachLogConstruction::PolygonId 		obstaclePolygonId_;//Registered in planet config space
    MachPhys::BuildingMaterialUnits 	constructedUnits_; //Defines state of construction/deconstruction
    PhysAbsoluteTime 					timeConstructedUnitsUpdated_;//Time construction state last changed
    MachPhys::BuildingMaterialUnits 	visualisedUnitsOnUpdate_; //Number of units visualised when
                                                    //constructedUnits last changed
    MachPhys::BuildingMaterialUnits 	visualisedUnits_; //Number of units last visualised

    PhysConfigSpace2d* pConfigSpace_;
    
    MachLogConstruction::Entrances   		entrances_;
	MachPhysMarker* 						pMarker_; //Pointer to selection/highlight marker entity
	MachLogConstruction::Constructors		constructors_;
	MachLogConstruction::Machines			machines_;
	MachLogConstruction::BuildPoints		buildPoints_;
	bool									needRebuild_;
	
	// time after which it is deemed that a specified pad has not been "recently used" by a resource carrier
	MachLogConstruction::Times				notRecentlyUsedPadTimes_;
	
	int										nReservations_;	// used by supercommands to maintain wireframes even
															// when no constructors have been added.
															
	int										constructionId_; //  used by constructions which have "alternative sites"
															 //  to ensure that no more than one allternative is actually built
															 //  at any one time.
															 
	int										originalRaceConstructionId_; //  a captured construction automatically has its
																		 //  constructionId_ set to a non-clashing dummy value,
																		 //  but if the original race recaptures it, it should have its
																		 //  constructionId_ restored. This will hold it in the interrim.															 
																		 
	bool 									isComplete_;
	
	// used to determine interval during which subsequent hits from gunfire will not cause more verbal e-mails 
   	PhysAbsoluteTime                		lastHitTime_;
	
	// used in conjunction with mechanism which removes the buildpoint which would otherwise block the entrance
	bool									removedDoorPoint_;

};

PER_DECLARE_PERSISTENT( MachLogConstructionImpl );

#endif

/* End CONSTROI.HPP *************************************************/
