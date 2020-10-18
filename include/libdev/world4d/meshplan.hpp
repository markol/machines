/*
 * M E S H P L A N. H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dMeshPlan

    Represents a collection of meshes as a function of time.
*/

#ifndef _REN_MESHPLAN_HPP
#define _REN_MESHPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
// #include "world4d/entity.hpp"
#include "stdlib/utility.hpp"

//Forward declarations
class W4dCamera;
class W4dOptions;
class SysPathName;
class W4dEntity;
class W4dRoot;
class SysPathName;
class W4dComplexity;

class W4dMeshPlan
{
public:
    //ctor
    W4dMeshPlan();

    //dtor
    ~W4dMeshPlan();

	//Add a set of meshes up to a time interval.
	//The meshes are defined by lod file pathName.
	//These meshes are to be used up to time endTime from end of previous mesh set.
	//The transform to be used with the mesh is transform.
	void add( const SysPathName& pathName, const PhysRelativeTime& endTime,
              const MexTransform3d& transform );
	//PRE( endTime later than most recently added meshes time )

	//Render the appropriate mesh from the plan using camera and options.
	//The distance from the camera to the entity is range.
	//The entity having the mesh attached is pEntity.
	//The mesh to be selected is th eone at time offset timeOffset.
	void renderAtRange( const W4dCamera& camera, MATHEX_SCALAR range, W4dEntity* pEntity,
                                 const PhysRelativeTime& timeOffset,
                                 const W4dComplexity& complexity ) const;

    //Duration of the plan
    PhysRelativeTime duration( void ) const;

    PER_MEMBER_PERSISTENT_DEFAULT( W4dMeshPlan );
    PER_FRIEND_READ_WRITE( W4dMeshPlan );

protected:

private:
    // Operations deliberately revoked
    W4dMeshPlan( const W4dMeshPlan& );
    const W4dMeshPlan& operator =( const W4dMeshPlan& );
    bool operator ==( const W4dMeshPlan& ) const;

	typedef std::pair<W4dEntity* , PhysRelativeTime> MeshInterval;
	typedef ctl_vector<MeshInterval> MeshIntervals;

    // Data members
	W4dRoot* pRoot_; //Used to own the W4dGenerics when not attached to a main tree
	                 //entity
	MeshIntervals intervals_; //Collection defining display in each time range
};

PER_DECLARE_PERSISTENT( W4dMeshPlan );

typedef CtlCountedPtr<W4dMeshPlan> W4dMeshPlanPtr;

#endif
