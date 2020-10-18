/*
 * S H O C K W A V . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysShockWave

    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _SHOCKWAV_HPP
#define _SHOCKWAV_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/entity.hpp"
#include "machphys/flames.hpp"

class MachPhysShockWave : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysShockWave( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar  - ensures the bomb meshes and textures are loaded
    static const MachPhysShockWave& exemplar();

	//start the shock wave as planned
	void startShockWave( const PhysAbsoluteTime& startTime,
                        const PhysRelativeTime& duration,
                        const MATHEX_SCALAR& fromRadius,
                        const MATHEX_SCALAR& toRadius,
                        const MATHEX_SCALAR& zScale);
    //dtor
    ~MachPhysShockWave();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	//shockwave reaches distance at *pTime. return true if distance <= shockwave radius
	static bool reachDistance(const PhysRelativeTime& duration,
                        	  const MATHEX_SCALAR& fromRadius,
                        	  const MATHEX_SCALAR& toRadius,
                        	  const  MATHEX_SCALAR& distance,
                        	  PhysRelativeTime* pTime);

	static const double& shockwaveDefaultSize();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysShockWave& t );
    typedef  MachPhysFlame::Materials Materials;
    static void addMaterial( MachPhysShockWave::Materials* , const RenTexture&  );

    PER_MEMBER_PERSISTENT( MachPhysShockWave );

private:
    //Deliberately revoked
    MachPhysShockWave( const MachPhysShockWave& );
    MachPhysShockWave& operator =( const MachPhysShockWave& );
    bool operator ==( const MachPhysShockWave& );

	friend class MachPhysOtherPersistence;
	friend class MachPhysPreload;
    //One-time constructor used to create the exemplar
    MachPhysShockWave();

	//typedef  MachPhysFlame::Materials Materials;
	typedef  MachPhysFlame::MaterialsPtr MaterialsPtr;

    static  const MaterialsPtr& materialsPtr( void );
};

PER_READ_WRITE( MachPhysShockWave );
PER_DECLARE_PERSISTENT( MachPhysShockWave );

#endif

/* End SHOCKWAV.HPP *************************************************/
