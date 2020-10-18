/*
 * R A C E C H N G . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysRaceChanger

    Supports application of material vector overrides for changing the race of
    all model types.
*/

#ifndef _MACHPHYS_RACECHNG_HPP
#define _MACHPHYS_RACECHNG_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

class MachPhysRaceChangerBody;
class W4dCompositeMaterialVecChanger;
class MachPhysMachine;
class MachPhysMachineData;
class MachPhysConstruction;
class MachPhysConstructionData;
class MachPhysWeapon;
class RenColour;

class MachPhysRaceChanger
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysRaceChanger& instance();
    ~MachPhysRaceChanger();

    //Export the changer for machine to convert to race toRace
    //NB do not call from machine's constructor. Use method below.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysMachine& machine,
                                                   MachPhys::Race toRace ) const;

    //Export the changer for machine to convert to race toRace.
    //The data for the machine is data. This method is needed because the call
    //to obtain data directly from machine is virtual, and hence cannot be used
    //in the machine ctor.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysMachine& machine,
                                                   const MachPhysMachineData& data,
                                                   MachPhys::Race toRace ) const;

    //Export the changer for construction to convert to race toRace
    //NB do not call from construction's constructor. Use method below.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysConstruction& construction,
                                                   MachPhys::Race toRace ) const;

    //Export the changer for construction to convert to race toRace.
    //The data for the construction is data. This method is needed because the call
    //to obtain data directly from construction is virtual, and hence cannot be used
    //in the construction ctor.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysConstruction& construction,
                                                   const MachPhysConstructionData& data,
                                                   MachPhys::Race toRace ) const;

    //Export the changer for weapon to convert to race toRace
    const W4dCompositeMaterialVecChanger& changer( const MachPhysWeapon& weapon,
                                                   MachPhys::Race toRace ) const;

    //The standard colour for a given race
    const RenColour& raceColour( MachPhys::Race race ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysRaceChanger& t );

private:
    MachPhysRaceChanger( const MachPhysRaceChanger& );
    MachPhysRaceChanger& operator =( const MachPhysRaceChanger& );
    bool operator ==( const MachPhysRaceChanger& );

    MachPhysRaceChanger();

    //data members
    MachPhysRaceChangerBody *pBody_; //Uses handle-body idiom
};


#endif

/* End RACECHNG.HPP *************************************************/
