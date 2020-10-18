/*
 * R A C E C H B O . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysRaceChangerBody

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_RACECHBO_HPP
#define _MACHPHYS_RACECHBO_HPP

#include "base/base.hpp"
#include "world4d/fwmateri.hpp"
#include "machphys/machphys.hpp"
#include "render/texture.hpp"

//forward refs
template< class T > class ctl_vector;
template < class KEY, class T, class COMPARE > class ctl_map;
class MachPhysMachineSpec;
class MachPhysMachineSpec_Less;
class MachPhysConstructionSpec;
class MachPhysConstructionSpec_Less;
class MachPhysWeaponSpec;
class MachPhysWeaponSpec_Less;
class W4dCompositeMaterialVecChanger;
class MachPhysMachine;
class MachPhysMachineData;
class MachPhysConstruction;
class MachPhysConstructionData;
class MachPhysWeapon;
class RenTexture;

class MachPhysRaceChangerBody
// Canonical form revoked
{
public:
    //ctor.
    MachPhysRaceChangerBody();

    //dtor
    ~MachPhysRaceChangerBody();

    //Types
    typedef ctl_vector< MachPhys::Race > Races;

    //Check to see if the material changers have been set up for machine, and if not
    //add the changers for all races to this map.
    //The data for machine is data. Needed to avoid virtual call while constructing machine.
    void add( const MachPhysMachine& machine, const MachPhysMachineData& data );

    //Export the changer for machine to convert to race toRace
    //The data for machine is data. Needed to avoid virtual call while constructing machine.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysMachine& machine,
                                                   const MachPhysMachineData& data,
                                                   MachPhys::Race toRace ) const;

    //Check to see if the material changers have been set up for construction, and if not
    //add the changers for all races to this map.
    //The data for construction is data. Needed to avoid virtual call while constructing.
    void add( const MachPhysConstruction& construction, const MachPhysConstructionData& data );

    //Export the changer for construction to convert to race toRace
    //The data for construction is data. Needed to avoid virtual call while constructing.
    const W4dCompositeMaterialVecChanger& changer( const MachPhysConstruction& construction,
                                                   const MachPhysConstructionData& data,
                                                   MachPhys::Race toRace ) const;

    //Check to see if the material changers have been set up for weapon, and if not
    //add the changers for all races to this map.
    void add( const MachPhysWeapon& weapon );

    //Export the changer for weapon to convert to race toRace
    const W4dCompositeMaterialVecChanger& changer( const MachPhysWeapon& weapon,
                                                   MachPhys::Race toRace ) const;

    //The HSV hue of the median colour for race
    double hue( MachPhys::Race race ) const;

    //True if hue1 is close to hue2 on the HSV hue scale
    bool close( double hue1, double hue2 ) const;

	typedef ctl_vector< RenTexture > Textures;
	typedef ctl_vector< Textures > TexturesVec;

	static const TexturesVec& texturesVec();
	static const Textures& keyTextures();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysRaceChangerBody& t );

private:
    MachPhysRaceChangerBody( const MachPhysRaceChangerBody& );
    MachPhysRaceChangerBody& operator =( const MachPhysRaceChangerBody& );
    bool operator ==( const MachPhysRaceChangerBody& );

    //types
    typedef ctl_map< MachPhysMachineSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysMachineSpec_Less > MachineChangerMap;

    typedef ctl_map< MachPhysConstructionSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysConstructionSpec_Less > ConstructionChangerMap;

    typedef ctl_map< MachPhysWeaponSpec, W4dCompositeMaterialVecChanger*,
                     MachPhysWeaponSpec_Less > WeaponChangerMap;

    //A machine spec suitable for a machine with attributes defined by data for use as a map key
    static MachPhysMachineSpec machineSpec( const MachPhysMachineData& data );

    //A construction spec suitable for a construction with attributes defined by data
    //for use as a map key
    static MachPhysConstructionSpec constructionSpec( const MachPhysConstructionData& data );

    //A weapon spec suitable for a weapon of type type for use as a map key
    static MachPhysWeaponSpec weaponSpec( MachPhys::WeaponType type );

    //data members
    MachineChangerMap* pMachineChangerMap_; //Map from machine spec to material changer
    ConstructionChangerMap* pConstructionChangerMap_; //Map from construction spec to material changer
    WeaponChangerMap* pWeaponChangerMap_; //Map from weapon spec to material changer
    Races* pChangerRaces_; //Collection of the races which require changers
    W4dMaterialVecPtrSet* pMaterialVecPtrSet_;//The collection of material override vectors 
                                              //used for all models handled by the changers
};


#endif

/* End RACECHBO.HPP *************************************************/
