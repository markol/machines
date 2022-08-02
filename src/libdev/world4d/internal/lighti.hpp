/*
 * L I G H T I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


#ifndef _W4D_LIGHTI_HPP
#define _W4D_LIGHTI_HPP

#include "world4d/light.hpp"
#include "world4d/lllist.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"

#include "render/colour.hpp"
#include "render/light.hpp"

#include "phys/sclplan.hpp"
#include "mathex/point3d.hpp"
#include "mathex/sphere3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"


#include <algorithm>
#include <limits.h>

class W4dSceneManager;

// A pimple for the light base class.
class W4dLightImpl
{

public:

    PER_MEMBER_PERSISTENT( W4dLightImpl );
    PER_FRIEND_READ_WRITE( W4dLightImpl );
private:
	friend class W4dLight;
	friend class W4dSceneManager;

	W4dLightImpl( W4dLight* light );
	~W4dLightImpl();

	void dynamicDisable( bool );
	bool dynamicDisable() const { return dynamicDisable_; }
	bool isDynamicEnabledOn() const { return dynamicEnabledOn_; }
	bool hasBoundingSphere() { return hasBoundingSphere_; }
	const MexSphere3d& boundingSphere( const W4dCamera* ) const;		// PRE(hasBoundingSphere()); PRE(isOn());
	void updateRenOn();

	typedef ctl_pvector<W4dEntity> Entities;

	W4dLight			*pW4dLight_;
	Entities*			pEntities_;			// the entities illuminated by this W4dLight
    RenLight*			pLight_;
	bool				enabled_, on_;		// on is a client value; enabled is internal
	bool				dynamicDisable_;	// used for dynamic light volume culling
	ulong				spherePassId_;		// compared against RenCamera::passId_
	ulong				renPassId_;			// compared against RenCamera::passId_
	RenColour*			pUnintensifiedColour_;
	PhysScalarPlanPtr	intensityPlanPtr_;
	PhysAbsoluteTime	startTime_;
	MexSphere3d*		sphere_;
	bool 				dynamicEnabledOn_;
	bool 				hasBoundingSphere_;
	uint				nRepetations_;
};

PER_DECLARE_PERSISTENT( W4dLightImpl );

#endif

/* End LIGHTI.HPP ****************************************************/
