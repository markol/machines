/*
 * C A M E R A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogCamera

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CAMERA_HPP
#define _MACHLOG_CAMERA_HPP

#include "base/base.hpp"
#include "world4d/camera.hpp"
#include "world4d/observer.hpp"
#include "mathex/mathex.hpp"
#include "machphys/plansurf.hpp"

class MachLogConstruction;
class PhysConfigSpace2d;
class MexPolygon2d;
class MexLine2d;
class MachLogCameraImpl;
class MexConvexPolygon2d;
class MexPoint2d;
struct ObstacleCollisionInfo;
template < class T > class ctl_pvector;

class MachLogCamera : public W4dCamera, public W4dObserver
{
public:
    enum Type { GROUND, FREE_MOVING, ZENITH, THIRD_PERSON, FIRST_PERSON };
    
    MachLogCamera(
        W4dSceneManager* pMgr,
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        Type cameraType );
    //  PRE( pMgr != NULL );
    //  PRE( pParent != NULL );
            
    ~MachLogCamera( void );
	
	// When a camera become the active camera, this should be called.
	void enable(MachLogCamera* lastCamera);

    //  Update the camera's domain, also take care of it entering any buildings.
    void    update( void );
    
    void CLASS_INVARIANT;

    bool    newPositionIsValid( const MexTransform3d& newPosition ) const;

    //  We can only snap to positions if they are not in a building
    bool    canSnapToPosition( const MexPoint3d& newPosition );

    //  Attach the camera to a new parent without changing its gloabl
    //  transform. The current parent is pushed onto a stack so that
    //  it can be restored later. Also set the new config space to be used
    void    pushParent( W4dEntity* pParent, PhysConfigSpace2d* pConfigSpace );

    //  Attach the camera to the previous parent held in its stack
    void    popParent( void );
    //  PRE( stack is not empty );

    //Forces camera to stay in pDomain unless NULL
    void forceDomain( W4dDomain* pDomain );
	
	// Is this camera in a building?
	bool insideConstruction() const;

    //////////////////////////////////////////////////
    // Inherited from W4dObserver

	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain );

    //////////////////////////////////////////////////

	bool alternativeNewPosition( MexTransform3d* newPosition ) const;

    //  Return any floors which this camera might be close to. 
    //  These floors should be taken into account when finding 
    //  the height of the terrain the camera is over.
    const MachPhysPlanetSurface::Floors& floors() const;

    //Logically enter/leave construction. Mostly this is handled internally, but the first person
    //camera domain is switched by the first person update call.
	void leaveConstruction();
	void enterConstruction();

private:
    // Operation deliberately revoked
    MachLogCamera( const MachLogCamera& );

    // Operation deliberately revoked
    MachLogCamera& operator =( const MachLogCamera& );

    // Operation deliberately revoked
    bool operator ==( const MachLogCamera& );

    void    updatePlanetDomain( const MexPoint3d& position );
    void    updatePressurePad( const MexPoint3d& position );
    bool    groundPositionLegal( const MexTransform3d& position ) const;

	// If the camera is colliding with an obstacle in the config space then
	// this routine can be used to find all the points on that obstacle that
	// the camera will collide with if it keeps traveling in the same direction.
	// The list of collision points is returned in distance order (closest first).
	void findObstacleCollisionPoints(const MexPolygon2d& obstacle, 
									 ctl_pvector<ObstacleCollisionInfo>* pCollisionInfo, 
									 const MexLine2d& motionDir ) const;


    //  Return a polygon that extends a little around the given
    //  position. This is used to stop the camera moving right
    //  up to walls.
    MexConvexPolygon2d cameraPolygon( const MexPoint2d& position ) const;

    void    checkThreshold( void );
    Mathex::Side    thresholdSide( void );

    //Setup/remove observer relation with pCurrentPadConstruction_
    void startObservingConstruction();
    void stopObservingConstruction();

    MachLogCameraImpl* pImpl_;
};

PER_ENUM_PERSISTENT( MachLogCamera::Type );

#endif

/* End CAMERA.HPP ***************************************************/
