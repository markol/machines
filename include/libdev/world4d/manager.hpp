/*
 * M A N A G E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dManager

    Manages the physical model represented by the W4dLib classes.

    Responsibilities:
    Attempt to maintain the desired frame rate for each scene when called to refresh the screen.
    Be informed of the current simulation time at which the physical model is defined,
    and export that time.
*/

#ifndef _MANAGER_HPP
#define _MANAGER_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"

//Forward declarations
class W4dSceneManager;
class RenDisplay;

//Singleton, orthodox canonical(revoked)
class W4dManager
{
public:
    // Singleton class
    static W4dManager& instance();
    ~W4dManager();

    // Set/get the current simulation time
    void time( const PhysAbsoluteTime& newTime );
    const PhysAbsoluteTime& time() const;

    //set/get/clear the artificial time.
    //When set, time() returns the artificial time instead of actual current time.
    //This can be used to perform position computations ahead of time etc
    void artificialTime( const PhysAbsoluteTime& newTime );
    void clearArtificialTime();
    bool hasArtificialTime() const;

    //Call from the app's core loop to handle updating of any library level conditions.
    void update();

    //Set/get the current scene manager.
    //This is a temporary interface until multiple concurrent scenes are supported.
    void sceneManager( W4dSceneManager* pSceneManager );

	// Set sceneManager back to NULL.
	void clearSceneManager();

    W4dSceneManager* sceneManager() const;
	//PRE( pSceneManager != NULL );

    //True iff there is a current scene manager
    bool hasSceneManager() const;

    // Perform any rendering required to maintain the frame rates of the
    // current scenes. Not all scenes will necessarily be rendered.
    void render();

    //  Return the frame number. The frame number is incremented after
    //  each call to render
    size_t  frameNumber() const;
    
	// A number which a camera or scene manager can use to uniquely
	// identify a render pass.
	ulong generateRenderPassId();

    //Increments an id to denote that some object may have changed its position
    ulong generateGeneralPositionId();

    //The last generated id
    ulong generalPositionId() const;
	
	// These are error handling functions for DirectX errors.  The first
	// attempts to restore the screen before an abnormal termination.
	// The second simply notes that rendering is unavailable.
	void emergencyShutdown();
	void renderingSuspended();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dManager& t );

private:
    // Operation deliberately revoked
    W4dManager( const W4dManager& );

    // Operation deliberately revoked
    W4dManager& operator =( const W4dManager& );

    // Operation deliberately revoked
    bool operator ==( const W4dManager& );

    W4dManager();

    //data members
    PhysAbsoluteTime	currentTime_;	//Current simulation time
    PhysAbsoluteTime	artificialTime_; //Current time to be used artificially
    W4dSceneManager*	pSceneManager_; //The visualisation scene
	ulong				currentPassId_;
    ulong               generalPositionId_; //Updated whenever anything occurs
                                            //which could change the position of any object
    size_t              frameNumber_;
    bool hasArtificialTime_; //true if artificialTime_ has been set
};

#ifdef _INLINE
    #include "world4d/manager.ipp"
#endif


#endif

/* End MANAGER.HPP **************************************************/
