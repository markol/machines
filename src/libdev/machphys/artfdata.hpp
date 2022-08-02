/*
 * A R T F D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysArtefactData

    Holds the artefact specific data for a type of MachPhysArtefact.
*/

#ifndef _MACHPHYS_ARTFDATA_HPP
#define _MACHPHYS_ARTFDATA_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "machphys/objdata.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MexConvexPolygon2d;

class MachPhysArtefactData : public MachPhysObjectData
// Canonical form revoked
{
public:
    //ctor. height and pPolygon define the obstacle attributes that should be used for
    //the associated artefact.
    MachPhysArtefactData( MATHEX_SCALAR height, 
    					  MexConvexPolygon2d* pPolygon,
						  const MachPhys::ArmourUnits&	 armour,
						  const MachPhys::HitPointUnits& hitPoints,
						  const MachPhys::BuildingMaterialUnits& cost,
						  int subType,
						  uint resourceStringId );

    virtual ~MachPhysArtefactData();

    //Properties
    MATHEX_SCALAR obstacleHeight() const;
    int subType() const;
    uint resourceStringId() const;

    //The local coordinate obstacle polygon
    const MexConvexPolygon2d& obstaclePolygon() const;

    
    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysArtefactData& t );

    MachPhysArtefactData( const MachPhysArtefactData& );
    MachPhysArtefactData& operator =( const MachPhysArtefactData& );

    MATHEX_SCALAR height_; //obstacle height
    int subType_; //The subtype code for this artefact
    uint resourceStringId_; //Id of the resource string to use in prompt text etc
    MexConvexPolygon2d* pPolygon_; //Obstacle polygon in local coordinates
};

#ifdef _INLINE
    #include "machphys/artfdata.ipp"
#endif


#endif

/* End ARTFDATA.HPP *************************************************/
