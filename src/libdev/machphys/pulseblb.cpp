/*
 * P U L S E B L B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/pulseblb.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/composit.hpp"

#include "machphys/pulsewep.hpp"
#include "machphys/fireball.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/pulsplat.hpp"
#include "machphys/pulsaura.hpp"

PER_DEFINE_PERSISTENT( MachPhysPulseBlob );

MachPhysPulseBlob::MachPhysPulseBlob
(
	W4dEntity* pParent,
	const MexTransform3d& localTransform,
	MachPhys::WeaponType type,
	CreateLights lights
)
:   MachPhysLinearProjectile( exemplar( type ), pParent, localTransform,
							 (lights==CREATE_LIGHTS)? COPY_LIGHTS: DONT_COPY_LIGHTS ),
    pFireball_( NULL )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    TEST_INVARIANT;
}

MachPhysPulseBlob::MachPhysPulseBlob(MachPhys::WeaponType type)
:   MachPhysLinearProjectile( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d() ),
    pFireball_( NULL )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //Load the STF mesh
    switch( type )
    {
        case MachPhys::PULSE_RIFLE:
        {
            readCompositeFile( SysPathName( "models/weapons/pulboltr/pulboltr.cdf" ) );
            break;
        }

        case MachPhys::PULSE_CANNON:
        {
            readCompositeFile( SysPathName( "models/weapons/pulboltc/pulboltc.cdf" ) );
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    TEST_INVARIANT;
}

MachPhysPulseBlob::MachPhysPulseBlob( PerConstructor con )
: MachPhysLinearProjectile( con )
{
}

MachPhysPulseBlob::~MachPhysPulseBlob()
{
    TEST_INVARIANT;

    _DELETE( pFireball_ );
}

void MachPhysPulseBlob::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPulseBlob& t )
{

    o << "MachPhysPulseBlob " << (void*)&t << " start" << std::endl;
    o << "MachPhysPulseBlob " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysPulseBlob& MachPhysPulseBlob::exemplar(MachPhys::WeaponType type)
{
    //Use the one time constructor
/*    static MachPhysPulseBlob* result = NULL;

	switch(type)
	{
		case MachPhys::PULSE_RIFLE:
		{
    		static MachPhysPulseBlob* blob = _NEW( MachPhysPulseBlob( type ) );
			result = blob;
			break;
		}
		case MachPhys::PULSE_CANNON:
		{
    		static MachPhysPulseBlob* blob = _NEW( MachPhysPulseBlob(type) );
			result = blob;
			break;
		}

        DEFAULT_ASSERT_BAD_CASE( type );
	}

    return *result;
*/
    return MachPhysOtherPersistence::instance().pulseBlobExemplar( type );
}

//virtual
PhysRelativeTime MachPhysPulseBlob::doBeDestroyedAt
(
    const PhysAbsoluteTime& time, MachPhys::StrikeType strikeType
)
{
    PhysRelativeTime duration = 0.33;

    switch( strikeType )
    {
        case MachPhys::ON_OBJECT:
        case MachPhys::ON_TERRAIN:
        {
            //get the local position of the blob at the destruct time
            MexTransform3d destructPosition;
            uint n;
            const W4dEntityPlan& myEntityPlan = entityPlan();
            if( myEntityPlan.hasMotionPlan() )
                entityPlan().transform( time, &destructPosition, &n );
            else
                destructPosition = localTransform();

            //Create a disc owned by the blob's parent
            //animationDuration =
               // MachPhysPulseWeapon::createPulseDisc( pParent(), destructPosition, time );
			//create splat and aura

			MachPhysPulseAura* pAura = _NEW( MachPhysPulseAura( pParent(), destructPosition) );
			pAura->startPulseAura( time, duration );

			MachPhysPulseSplat* pSplat = _NEW( MachPhysPulseSplat( pParent(), destructPosition) );
			pSplat->startPulseSplat( time, duration );

		     //Add to the garbage collection list. Let it survive, so the sound
		    //associated with it isn't stopped too early
		    W4dGarbageCollector::instance().add( pSplat, time + duration );
		    W4dGarbageCollector::instance().add( pAura, time + duration );

            break;
        }
    }

    return duration;
}

void perWrite( PerOstream& ostr, const MachPhysPulseBlob& blob )
{
    const MachPhysLinearProjectile& base = blob;

    ostr << base;
    ostr << blob.pFireball_;
}

void perRead( PerIstream& istr, MachPhysPulseBlob& blob )
{
    MachPhysLinearProjectile& base = blob;

    istr >> base;
    istr >> blob.pFireball_;
}

/* End PULSEBLB.CPP *************************************************/
