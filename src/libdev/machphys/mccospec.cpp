/*
 * M C C O S P E C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/mccospec.hpp"

MachPhysMachineSpec::MachPhysMachineSpec
(
     MachPhys::MachineType type, int subType, uint hwLevel, uint swLevel, MachPhys::Race race
)
:   type_( type ),
    subType_( subType ),
    hwLevel_( hwLevel ),
    swLevel_( swLevel ),
    race_( race )
{
    TEST_INVARIANT;
}

MachPhysMachineSpec::MachPhysMachineSpec()
:   type_( MachPhys::AGGRESSOR ),
    subType_( 0 ),
    hwLevel_( 1 ),
    swLevel_( 1 ),
    race_( MachPhys::RED )
{
    TEST_INVARIANT;
}

MachPhysMachineSpec::~MachPhysMachineSpec()
{
    TEST_INVARIANT;

}

void MachPhysMachineSpec::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachineSpec& t )
{

    o << "MachPhysMachineSpec " << (void*)&t << " start" << std::endl;
    o << "MachPhysMachineSpec " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhys::MachineType MachPhysMachineSpec::type() const
{
    return type_;
}

MachPhys::Race MachPhysMachineSpec::race() const
{
    return race_;
}

int MachPhysMachineSpec::subType() const
{
    return subType_;
}

uint MachPhysMachineSpec::hwLevel() const
{
    return hwLevel_;
}

uint MachPhysMachineSpec::swLevel() const
{
    return swLevel_;
}

void MachPhysMachineSpec::type( MachPhys::MachineType type )
{
    type_ = type;
}

void MachPhysMachineSpec::subType( int subType )
{
    subType_ = subType;
}

void MachPhysMachineSpec::hwLevel( uint level )
{
    hwLevel_ = level;
}

void MachPhysMachineSpec::swLevel( uint level )
{
    swLevel_ = level;
}

void MachPhysMachineSpec::race( MachPhys::Race race )
{
    race_ = race;
}

bool operator <( const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs )
{
    return lhs.type_ < rhs.type_  or
               (lhs.type_ == rhs.type_  and
                   (lhs.subType_ < rhs.subType_  or
                       (lhs.subType_ == rhs.subType_  and
                           (lhs.hwLevel_ < rhs.hwLevel_  or
                               (lhs.hwLevel_ == rhs.hwLevel_  and
                                   (lhs.swLevel_ < rhs.swLevel_  or
                                       (lhs.swLevel_ == rhs.swLevel_  and
                                           (lhs.race_ < rhs.race_ ) ) ) ) ) ) ) );
}

bool operator ==( const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs )
{
    return lhs.type_ == rhs.type_  and
           lhs.subType_ == rhs.subType_  and
           lhs.hwLevel_ == rhs.hwLevel_  and
           lhs.swLevel_ == rhs.swLevel_  and
           lhs.race_ == rhs.race_;
}

MachPhysConstructionSpec::MachPhysConstructionSpec
(
     MachPhys::ConstructionType type, int subType, uint hwLevel, MachPhys::Race race
)
:   type_( type ),
    subType_( subType ),
    hwLevel_( hwLevel ),
    race_( race )
{
    TEST_INVARIANT;
}

MachPhysConstructionSpec::MachPhysConstructionSpec()
:   type_( MachPhys::BEACON ),
    subType_( 0 ),
    hwLevel_( 1 ),
    race_( MachPhys::RED )
{
    TEST_INVARIANT;
}

MachPhysConstructionSpec::~MachPhysConstructionSpec()
{
    TEST_INVARIANT;

}

void MachPhysConstructionSpec::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysConstructionSpec& t )
{

    o << "MachPhysConstructionSpec " << (void*)&t << " start" << std::endl;

    o << "Type " << t.type_ << std::endl;
    o << "Subtype " << t.subType_ << std::endl;
    o << "HW level " << t.hwLevel_ << std::endl;
    o << "Race " << t.race_ << std::endl;

    o << "MachPhysConstructionSpec " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhys::ConstructionType MachPhysConstructionSpec::type() const
{
    return type_;
}

MachPhys::Race MachPhysConstructionSpec::race() const
{
    return race_;
}

int MachPhysConstructionSpec::subType() const
{
    return subType_;
}

uint MachPhysConstructionSpec::hwLevel() const
{
    return hwLevel_;
}

void MachPhysConstructionSpec::type( MachPhys::ConstructionType type )
{
    type_ = type;
}

void MachPhysConstructionSpec::subType( int subType )
{
    subType_ = subType;
}

void MachPhysConstructionSpec::hwLevel( uint level )
{
    hwLevel_ = level;
}

void MachPhysConstructionSpec::race( MachPhys::Race race )
{
    race_ = race;
}

bool operator <( const MachPhysConstructionSpec& lhs, const MachPhysConstructionSpec& rhs )
{
    return lhs.type_ < rhs.type_  or
               (lhs.type_ == rhs.type_  and
                   (lhs.subType_ < rhs.subType_  or
                       (lhs.subType_ == rhs.subType_  and
                           (lhs.hwLevel_ < rhs.hwLevel_  or
                               (lhs.hwLevel_ == rhs.hwLevel_  and
                                   (lhs.race_ < rhs.race_ ) ) ) ) ) );
}

bool operator ==( const MachPhysConstructionSpec& lhs, const MachPhysConstructionSpec& rhs )
{
    return lhs.type_ == rhs.type_  and
           lhs.subType_ == rhs.subType_  and
           lhs.hwLevel_ == rhs.hwLevel_  and
           lhs.race_ == rhs.race_;
}

bool MachPhysMachineSpec_Less::operator()
(
    const MachPhysMachineSpec& lhs, const MachPhysMachineSpec& rhs
) const
{
    return lhs < rhs;
}

bool MachPhysConstructionSpec_Less::operator()
(
    const MachPhysConstructionSpec& lhs, const MachPhysConstructionSpec& rhs
) const
{
    return lhs < rhs;
}

MachPhysWeaponSpec::MachPhysWeaponSpec
(
     MachPhys::WeaponType type, MachPhys::Race race
)
:   type_( type ),
    race_( race )
{
    TEST_INVARIANT;
}

MachPhysWeaponSpec::MachPhysWeaponSpec()
//:   type_( MachPhys::L_BOLTER ),
:   type_( _STATIC_CAST(MachPhys::WeaponType, MachPhys::L_BOLTER )),
    race_( MachPhys::RED )
{
    TEST_INVARIANT;
}

MachPhysWeaponSpec::~MachPhysWeaponSpec()
{
    TEST_INVARIANT;

}

void MachPhysWeaponSpec::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWeaponSpec& t )
{

    o << "MachPhysWeaponSpec " << (void*)&t << " start" << std::endl;
    o << "MachPhysWeaponSpec " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhys::WeaponType MachPhysWeaponSpec::type() const
{
    return type_;
}

MachPhys::Race MachPhysWeaponSpec::race() const
{
    return race_;
}

void MachPhysWeaponSpec::type( MachPhys::WeaponType type )
{
    type_ = type;
}

void MachPhysWeaponSpec::race( MachPhys::Race race )
{
    race_ = race;
}

bool operator <( const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs )
{
    return lhs.type_ < rhs.type_  or
               (lhs.type_ == rhs.type_  and
                   (lhs.race_ < rhs.race_ ) );
}

bool operator ==( const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs )
{
    return lhs.type_ == rhs.type_  and
           lhs.race_ == rhs.race_;
}

bool MachPhysWeaponSpec_Less::operator()
(
    const MachPhysWeaponSpec& lhs, const MachPhysWeaponSpec& rhs
) const
{
    return lhs < rhs;
}

/* End MCCOSPEC.CPP *************************************************/
