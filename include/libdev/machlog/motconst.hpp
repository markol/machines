/*
 * M O T C O N S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGroundCameraMotionConstraint
	MachLogZenithCameraMotionConstraint
	MachLogPlanetCameraConstraint

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_MOTCONST_HPP
#define _MACHLOG_MOTCONST_HPP

#include "base/base.hpp"
#include "mathex/radians.hpp"
#include "phys/phys.hpp"
#include "phys/mcground.hpp"
#include "phys/mczenith.hpp"
#include "phys/mcpers3c.hpp"

class MachLogCamera;
class MachLogMachine;
class DevKeyToCommandTranslator;
class DevButtonEvent;

class MachLogGroundCameraMotionConstraint : public PhysGroundMotionConstraint
// Canonical form revoked
{
public:
 	MachLogGroundCameraMotionConstraint( MachLogCamera* pCamera );
	virtual ~MachLogGroundCameraMotionConstraint( void );

    void CLASS_INVARIANT;
	
	virtual void move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime );
	
	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* trans );

	void zTerrainDelta( MATHEX_SCALAR );
	MATHEX_SCALAR zTerrainDelta() const;

private:
	// Operations revoked
    MachLogGroundCameraMotionConstraint( const MachLogGroundCameraMotionConstraint& );
    MachLogGroundCameraMotionConstraint& operator =( const MachLogGroundCameraMotionConstraint& );
    bool operator ==( const MachLogGroundCameraMotionConstraint& );

	friend ostream& operator <<( ostream& o, const MachLogGroundCameraMotionConstraint& t );

	MachLogCamera* pCamera_;
	MATHEX_SCALAR zTerrainDelta_;
};

///////////////////////////////////////////////////////////////////////////

class MachLogZenithCameraMotionConstraint : public PhysZenithMotionConstraint
{
public:
	MachLogZenithCameraMotionConstraint( MachLogCamera* pCamera );
	virtual ~MachLogZenithCameraMotionConstraint();

    void CLASS_INVARIANT;
	
	virtual void move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime );

	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* trans );
 
    //Update the view so the camera would be at cameraTransform
    void setCameraPosition( const MexTransform3d& cameraTransform );

    //get/set the data necessary to save/restore the camera position
    void cameraPositionData( MATHEX_SCALAR* pZoomDistance, MATHEX_SCALAR* pX, MATHEX_SCALAR* pY,
                             MexRadians* pHeading) const;
    void cameraPositionData( MATHEX_SCALAR zoomDistance, MATHEX_SCALAR x, MATHEX_SCALAR y,
                             MexRadians heading);

private:
	// Operations revoked
    MachLogZenithCameraMotionConstraint( const MachLogZenithCameraMotionConstraint& );
    MachLogZenithCameraMotionConstraint& operator =( const MachLogZenithCameraMotionConstraint& );
    bool operator ==( const MachLogZenithCameraMotionConstraint& );

	friend ostream& operator <<( ostream& o, const MachLogZenithCameraMotionConstraint& t );

	void footprint( MATHEX_SCALAR z, MexRadians headingAngle );

    //Computes and stores the centre of view and heading from camera's current transform
    void setCameraDataFromTransform();

	MachLogCamera* pCamera_;

    //computed by footprint() method. Stores relative offsets of footprint using
    //current height and heading angle
    MATHEX_SCALAR footprintXMin_;    
    MATHEX_SCALAR footprintYMin_;    
    MATHEX_SCALAR footprintXMax_;    
    MATHEX_SCALAR footprintYMax_;    

    //Unit direction vectors pointing from camera through corners of its
    //projection window
    MexVec3 cornerDirectionVectors_[4];

    bool isUpToDate_; //True if camera position has been set using this data
    MATHEX_SCALAR xViewPoint_; //x coord of centre of view
    MATHEX_SCALAR yViewPoint_; //y coord of centre of view
    MexRadians heading_; //angle of rotation about z axis for this view
	MexRadians pitchAngle_; //The standard desired pitch angle
    MexRadians highPitchAngle_; //A higher pitch angle used to keep camera over planet surface
    MATHEX_SCALAR sinPitchAngle_; //sine of the pitch angle
    MATHEX_SCALAR zoomDistance_; //distance of camera from centre of view on terrain

    enum {N_MAX_STORED_HEIGHTS = 50};
    MATHEX_SCALAR aStoredTerrainHeights_[ N_MAX_STORED_HEIGHTS ]; //Terrain heights on last few frames
    PhysAbsoluteTime aStoredTimes_[ N_MAX_STORED_HEIGHTS ]; //Times on last few frames
    uint nStoredTerrainHeights_; //Number of entries in above array
    uint storedTerrainHeightIndex_; //Next entry to write in above array
};

///////////////////////////////////////////////////////////////////////////

class MachLogPlanetCameraConstraint : public PhysMotionConstraint
// Canonical form revoked
{
public:
 	MachLogPlanetCameraConstraint( void );
	virtual ~MachLogPlanetCameraConstraint( void );

    void CLASS_INVARIANT;
	
	virtual void move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime );
	
	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* trans );

private:
	// Operations revoked
    MachLogPlanetCameraConstraint( const MachLogPlanetCameraConstraint& );
    MachLogPlanetCameraConstraint& operator =( const MachLogPlanetCameraConstraint& );
    bool operator ==( const MachLogPlanetCameraConstraint& );

	friend ostream& operator <<( ostream& o, const MachLogPlanetCameraConstraint& t );
};

///////////////////////////////////////////////////////////////////////////

class MachLogMachineThirdPerson : public PhysThirdPerson
{
// Canonical form revoked
public:
	MachLogMachineThirdPerson( const MachLogMachine* pMachine );
	virtual ~MachLogMachineThirdPerson();

    void CLASS_INVARIANT;

	// When following different things it is possible that they will come and go
	// out of existance (e.g. get destroyed ). 
	virtual bool thirdPersonExists() const;

  	virtual void getIdealCameraLocation( MexTransform3d* pTransform );

	static bool processButtonEvent( const DevButtonEvent& );

	const MachLogMachine* machine() const;

private:
	// Operations deliberately revoked
    MachLogMachineThirdPerson( const MachLogMachineThirdPerson& );
    MachLogMachineThirdPerson& operator =( const MachLogMachineThirdPerson& );
    bool operator ==( const MachLogMachineThirdPerson& );

	friend ostream& operator <<( ostream& o, const MachLogMachineThirdPerson& t );

	enum Command { ZOOMOUT, ZOOMIN, LEFT, RIGHT, UP, DOWN, RESET };

	static DevKeyToCommandTranslator& keyTranslator();
	static MexTransform3d& thirdPersonOffset( void );

	// data members
	const MachLogMachine* pMachine_;
};

class MachLogThirdPersonCameraConstraint : public PhysThirdPersonCameraConstraint
{
public:
 	MachLogThirdPersonCameraConstraint( PhysThirdPerson* pThirdPerson, MachLogCamera* pCamera );
	virtual ~MachLogThirdPersonCameraConstraint( void );

    void CLASS_INVARIANT;
	
	virtual void move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime );

private:
	// Operations revoked
    MachLogThirdPersonCameraConstraint( const MachLogThirdPersonCameraConstraint& );
    MachLogThirdPersonCameraConstraint& operator =( const MachLogThirdPersonCameraConstraint& );
    bool operator ==( const MachLogThirdPersonCameraConstraint& );

	friend ostream& operator <<( ostream& o, const MachLogThirdPersonCameraConstraint& t );

	MachLogCamera* pCamera_;
};

#endif

/* End MOTCONST.HPP *************************************************/
