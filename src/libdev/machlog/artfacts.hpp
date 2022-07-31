/*
 * A R T F A C T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogArtefacts

    Handles creation of the artefacts in a scenario
*/

#ifndef _MACHLOG_ARTFACTS_HPP
#define _MACHLOG_ARTFACTS_HPP

#include "base/base.hpp"
#include "stdlib/strfwd.hpp"
#include "utility/id.hpp"
#include "mathex/mathex.hpp"
#include "machphys/machphys.hpp"


//forward refs
template< class T > class ctl_vector;
template< class T > class CtlCountedPtr;
class SysPathName;
class MexPoint2d;
class MexPoint3d;
class MexRadians;
class W4dGenericRepository;
class W4dEntity;
class MachLogArtefactsData;
class UtlLineTokeniser;
class MachLogArtefactsImpl;
class MachPhysArtefactData;
class MachLogArtefacts
// Canonical form revoked
{
public:
    MachLogArtefacts();
    ~MachLogArtefacts();

	enum CreateArtefactInstances { CREATE_ARTEFACT_INSTANCES, DO_NOT_CREATE_ARTEFACT_INSTANCES };
    //Parse the ARTEFACTS section of a scenario file
    void parseArtefactsSection( UtlLineTokeniser* pParser, CreateArtefactInstances );

    //The ids used to mark artefacts in the scenario file
    typedef uint TagId;

    //Load the artefact models defined in pathname. This must be a file suitable
    //for loading into a W4dGenericRepository
    void load( const SysPathName& pathname );

    //the number of registered artefact sub-types
    int nSubTypes() const;

    //Add a new artefact subType associated with the physical model in the repository which has key name.
    //The resource string id is stringId.
    //Its associated physical data is defined by cost, armour, hitPoints, and it has
    //a 2.5d boundary defined by localBoundary and height
    void addSubType( int subType, const string& name, MachPhys::BuildingMaterialUnits cost,
                     MachPhys::ArmourUnits armour, MachPhys::HitPointUnits hitPoints,
                     uint stringId,
                     MATHEX_SCALAR height, const ctl_vector< MexPoint2d >& localBoundary );
    //PRE( nSubTypes() == subType );

    //Add a new dumb artefact subType associated with the physical model in the repository which has key name.
    void addSubType( int subType, const string& name );
    //PRE( nSubTypes() == subType );

    //Add a damage link such that when the artefact actor with id dyingArtefactId is destroyed,
    //the artefact with actor id damageArtefactId is damaged by hpDamge and armourDamage.
    //If hpDamage is negative, the amount is subtracted from existing hp value. Otherwise the hp value
    //is reduced to hpDamage if greate than that. Similar rule for armourDamage.
    void addDamageRelation( UtlId dyingArtefactId, UtlId damageArtefactId, int hpDamage, int armourDamage );

    //Construct and return a new physical copy of artefact of given subType,
    //at given locatin and angle about the z axis
    W4dEntity* newPhysArtefact( int subType, const MexPoint3d& location, const MexRadians& angle ) const;
    //PRE( subType < nSubTypes() );

    //The data object for given subType.
    const MachPhysArtefactData& artefactData( int subType ) const;
    //PRE( subType < nSubTypes() );

    //Unloads scenario specific data
    void unload();

    void CLASS_INVARIANT;

	//the file name that was parsed for the artefacts section
	const SysPathName& artefactPathName() const;
	//whether or not there were any artefacts parsed from the scenario
	bool artefactsParsed() const;

    //Finish processing the loading of artefacts.
    //This clears out all the sub-type data. This should normally be called internally but the persistence 
    //mechanism needs to call it as well hence the public access rights.
    void finish();

	PER_MEMBER_PERSISTENT_DEFAULT( MachLogArtefacts );
	PER_FRIEND_READ_WRITE( MachLogArtefacts );

private:
	friend class MachLogArtefactsImpl;
    friend ostream& operator <<( ostream& o, const MachLogArtefacts& t );

    MachLogArtefacts( const MachLogArtefacts& );
    MachLogArtefacts& operator =( const MachLogArtefacts& );

    //data members
	MachLogArtefactsImpl*	pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogArtefacts );

#endif

/* End ARTFACTS.HPP *************************************************/
