/*
 * L M I N E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/lmine.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "world4d/root.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "system/pathname.hpp"
#include "sim/manager.hpp"
#include "machphys/fireball.hpp"
#include "machphys/matconv.hpp"
#include "machphys/machphys.hpp"
#include "machphys/snddata.hpp"
#include "machphys/objexplo.hpp"

PER_DEFINE_PERSISTENT( MachPhysLandMine );

MachPhysLandMine::MachPhysLandMine( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Race race )
:   W4dEntity( exemplar(), pParent, localTransform ),
	race_( race )
{
    TEST_INVARIANT;
	W4dSoundManager::instance().play(this, SID_LMINE_DEPLOY, PhysAbsoluteTime( 0 ), 1);
}

//One-time ctor
MachPhysLandMine::MachPhysLandMine()
:   W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID )
{
    //Load the mesh data
    readLODFile( SysPathName( "models/weapons/lmine/lmine.lod" ) );

//    //TBD: Disable the bcak fuce cull via static cast here.
//    //These needs to be changed so the property is defined in the .x file.
//    Ren::ConstMeshPtr myMesh = mesh().mesh();
//    _CONST_CAST( RenMesh&, *myMesh ).backFace( false );
    TEST_INVARIANT;
}

MachPhysLandMine::MachPhysLandMine( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysLandMine::~MachPhysLandMine()
{
    TEST_INVARIANT;

}

void MachPhysLandMine::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLandMine& t )
{

    o << "MachPhysLandMine " << (void*)&t << " start" << std::endl;
    o << "MachPhysLandMine " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysLandMine& MachPhysLandMine::exemplar()
{
    //Use the one time constructor
    //static MachPhysLandMine& mine = *_NEW( MachPhysLandMine );
    return MachPhysOtherPersistence::instance().landMineExemplar();
}

//virtual
bool MachPhysLandMine::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy) const
{
    return defaultIntersectsLine( line, pDistance, accuracy );

}

//virtual
PhysRelativeTime MachPhysLandMine::beDestroyed( void )
{
    //Make the landmine invisible
    visible( false );
/*
    //Construct a fireball
    const MATHEX_SCALAR size = 3.0;
    const MATHEX_SCALAR depthOffset = -3.0;
    const PhysRelativeTime duration = 1.5;
    const PhysRelativeTime now = SimManager::instance().currentTime();

    //Position above ground
    MexTransform3d fireballTransform = localTransform();
    MexPoint3d location = fireballTransform.position();
    location.z( location.z() + size * 0.45 );
    fireballTransform.position( location );

    MachPhysFireball* pFireball =
        _NEW( MachPhysFireball( pParent(), fireballTransform, FIREBALL_1,
                                size, depthOffset, now, duration ) );

    //Garbage collect it soon
    W4dGarbageCollector::instance().add( pFireball, now + duration );

	return duration;
*/
    //Construct and use an explosion
    MachPhysObjectExplosion exploder( this );
	MachPhys::DemolitionType type;
	type.objectType=MachPhys::NOT_CONSTRUCTION;

    return exploder.explode( SimManager::instance().currentTime(), type );

}

void perWrite( PerOstream& ostr, const MachPhysLandMine& landMine )
{
    const W4dEntity& base = landMine;

    ostr << base;

    ostr << landMine.race_;
}

void perRead( PerIstream& istr, MachPhysLandMine& landMine )
{
    W4dEntity& base = landMine;

    istr >> base;

    istr >> landMine.race_;
}


/* End LMINE.CPP *************************************************/
