/*
 * M C C O S P E C . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMachineSpec
    MachPhysConstructionSpec
    MachPhysWeaponSpec

    These classes contain type and level data sufficient to specify a particular type
    of machine, construction or weapon.

    MachPhysMachineSpec_Less
    MachPhysConstructionSpec_Less

    These classes specify a comparison operator for pairs of specification classes
*/

#ifndef _MACHPHYS_MCCOSPEC_HPP
#define _MACHPHYS_MCCOSPEC_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

/////////////////////////////////////////////////////////////////////////////

class MachPhysMachineSpec
// Bitwise canonical
{
public:
    //ctor. Specifies major type, subtype(where applicable), and hardware and software levels
    //and race
    MachPhysMachineSpec( MachPhys::MachineType type, int subType, uint hwLevel, uint swLevel,
                         MachPhys::Race race );

    //default ctor for stl
    MachPhysMachineSpec();

    //dtor
    ~MachPhysMachineSpec();
    
    //standard operators
    friend bool operator ==( const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs );
    friend bool operator <( const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs );

    //Accessors
    MachPhys::MachineType type() const;
    int subType() const;
    uint hwLevel() const;
    uint swLevel() const;
    MachPhys::Race race() const;

    void type( MachPhys::MachineType type );
    void subType( int subType );
    void hwLevel( uint level );
    void swLevel( uint level );
    void race( MachPhys::Race race );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachineSpec& t );

private:
    //data members
    MachPhys::MachineType type_; //Major type
    int subType_; //Subtype if relevant
    uint hwLevel_; //hardware level
    uint swLevel_; //software level
    MachPhys::Race race_; //race
};

/////////////////////////////////////////////////////////////////////////////

class MachPhysMachineSpec_Less
{
public:
    bool operator() ( const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs ) const;
};

/////////////////////////////////////////////////////////////////////////////

class MachPhysConstructionSpec
// Bitwise canonical
{
public:
    //ctor. Specifies major type, subtype(where applicable), and hardware level
    MachPhysConstructionSpec( MachPhys::ConstructionType type, int subType, uint hwLevel,
                              MachPhys::Race race );

    //default ctor for stl
    MachPhysConstructionSpec();

    //dtor
    ~MachPhysConstructionSpec();
    
    //standard operators
    friend bool operator ==( const MachPhysConstructionSpec& lhs, const MachPhysConstructionSpec& rhs );
    friend bool operator <( const MachPhysConstructionSpec& lhs, const MachPhysConstructionSpec& rhs );

    //Accessors
    MachPhys::ConstructionType type() const;
    int subType() const;
    uint hwLevel() const;
    MachPhys::Race race() const;

    void type( MachPhys::ConstructionType type );
    void subType( int subType );
    void hwLevel( uint level );
    void race( MachPhys::Race race );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysConstructionSpec& t );

private:
    //data members
    MachPhys::ConstructionType type_; //Major type
    int subType_; //Subtype if relevant
    uint hwLevel_; //hardware level
    MachPhys::Race race_; //race
};

/////////////////////////////////////////////////////////////////////////////

class MachPhysConstructionSpec_Less
{
public:
    bool operator() ( const MachPhysConstructionSpec& lhs,
                      const MachPhysConstructionSpec& rhs ) const;
};

/////////////////////////////////////////////////////////////////////////////

class MachPhysWeaponSpec
// Bitwise canonical
{
public:
    //ctor. Specifies type and race
    MachPhysWeaponSpec( MachPhys::WeaponType type, MachPhys::Race race );

    //default ctor for stl
    MachPhysWeaponSpec();

    //dtor
    ~MachPhysWeaponSpec();
    
    //standard operators
    friend bool operator ==( const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs );
    friend bool operator <( const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs );

    //Accessors
    MachPhys::WeaponType type() const;
    MachPhys::Race race() const;

    void type( MachPhys::WeaponType type );
    void race( MachPhys::Race race );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWeaponSpec& t );

private:
    //data members
    MachPhys::WeaponType type_; //Type
    MachPhys::Race race_; //race
};

/////////////////////////////////////////////////////////////////////////////

class MachPhysWeaponSpec_Less
{
public:
    bool operator() ( const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs ) const;
};

/////////////////////////////////////////////////////////////////////////////

#endif

/* End MCCOSPEC.HPP *************************************************/
