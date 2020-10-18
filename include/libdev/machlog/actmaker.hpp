/*
 * A C T M A K E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogActorMaker

    Constructs new physical and logical instances of machines, constructions, etc
    based on major actor type, subtype, hardware and software level.
*/

#ifndef _MACHLOG_ACTMAKER_HPP
#define _MACHLOG_ACTMAKER_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"
#include "utility/id.hpp"

//Forward refs
class MachPhysConstruction;
class W4dEntity;
class MexTransform3d;
class MexPoint3d;
class MexRadians;
class MachLogMachine;
class MachLogConstruction;
class MachActor;
class MachLogOreHolograph;
class MexAlignedBox2d;
class MachLogDebris;
class MachLogLandMine;
class MachLogArtefact;

//static methods only - cannot be instantiated
class MachLogActorMaker
{
public:

    //Return a new instance of a physical construction using type, subType and hwLevel.
    //Parent, position and race defined by pParent, localTransform and race.
    static MachPhysConstruction* newPhysConstruction
        ( MachLog::ObjectType type, int subType, int hwLevel,
          W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Race race );

    //Return a new instance of a logical construction using type, subType and hwLevel.
    static MachLogConstruction* newLogConstruction
        ( MachLog::ObjectType type, int subType, int hwLevel,
          const MexPoint3d& location, const MexRadians& angle, MachPhys::Race race );

    //Return a new instance of a logical construction using type, subType and hwLevel.
    static MachLogConstruction* newLogConstruction
        ( MachLog::ObjectType type, int subType, int hwLevel,
          const MexPoint3d& location, const MexRadians& angle, MachPhys::Race race, 
          UtlId withId );

	//return a new machine instance.
	static MachLogMachine* newLogMachine
	(
	    MachLog::ObjectType type, int subType, int hwLevel, int swLevel, MachPhys::Race race,
	    const MexPoint3d& location, MachPhys::WeaponCombo wc 
	);

	//as above but with predefined id.
	static MachLogMachine* newLogMachine
	(
	    MachLog::ObjectType type, int subType, int hwLevel, int swLevel, MachPhys::Race race,
	    const MexPoint3d& location, MachPhys::WeaponCombo wc, 
        UtlId withId
	);

    //Return a new instance of a logical oreholo
    static MachLogOreHolograph* newLogOreHolograph
        ( MachPhys::Race race, int concentration, int quantity, const MexPoint3d& location );

    //Return a new instance of a logical oreholo
    static MachLogOreHolograph* newLogOreHolograph
        ( MachPhys::Race race, int concentration, int quantity, const MexPoint3d& location,
          UtlId withId );

    //Return a new instance of a logical debris
    static MachLogDebris* newLogDebris
        ( MachPhys::Race race, int quantity, const MexPoint3d& location, const MexAlignedBox2d& boundary );

    //Return a new instance of a logical debris
    static MachLogDebris* newLogDebris
        ( MachPhys::Race race, int quantity, const MexPoint3d& location, const MexAlignedBox2d& boundary, UtlId withId );

    //Return a new instance of a logical land mine
    static MachLogLandMine* newLogLandMine
        ( MachPhys::Race race, const MexPoint3d& location );

    //Return a new instance of a logical land mine
    static MachLogLandMine* newLogLandMine
        ( MachPhys::Race race, const MexPoint3d& location, UtlId withId );

    //Return a new instance of a logical artefact
    static MachLogArtefact* newLogArtefact
        ( int subType, const MexPoint3d& location, const MexRadians& angle );

    //Return a new instance of a logical artefact
    static MachLogArtefact* newLogArtefact
        ( int subType, const MexPoint3d& location, const MexRadians& angle, UtlId withId );

    //Return a new dumb artefact (no logical actor involved), located in the appropriate domain
    static W4dEntity* newDumbArtefact
        ( int subType, const MexPoint3d& location, const MexRadians& angle );


private:
    // Operations deliberately revoked
    MachLogActorMaker( void );
    ~MachLogActorMaker( void );
    MachLogActorMaker( const MachLogActorMaker& );
    MachLogActorMaker& operator =( const MachLogActorMaker& );
    bool operator ==( const MachLogActorMaker& );

};


#endif
/* End ACTMAKER.HPP *************************************************/
