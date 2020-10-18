/*
 * P R E L O A D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPreload

    Handles preloading of machphys models.
    Cannot be instantiated - static methods only.
*/

#ifndef _MACHPHYS_PRELOAD_HPP
#define _MACHPHYS_PRELOAD_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"

class PerIstreamReporter;

//static methods only - cannot be instantiated
class MachPhysPreload
{
public:
	enum DoLoadingProgressUpdate { ECHO_PROGRESS, QUIET_PROGRESS };

    //Force load all the models from the .x files
    static void swPreload( DoLoadingProgressUpdate );

	//preload only the machines from .x files
	static void preloadMachines( DoLoadingProgressUpdate );
	    
	//preload only the constructions from .x files
	static void preloadConstructions( DoLoadingProgressUpdate );

	//preload only the weapons and special effects from .x files
	static void preloadWeaponsAndEffects( DoLoadingProgressUpdate );

    //Preload the models not currently covered by persistence
    static void extrasPreload( DoLoadingProgressUpdate );

    //Save all the preloaded models to the persistent file
    static void persistentSave( DoLoadingProgressUpdate );

    //Load all the preloaded models from the persistent file
    static void persistentPreload( DoLoadingProgressUpdate, PerIstreamReporter* );
    //  PRE( persistentFileName().existsAsFile() );
    
    //Register soundIds
    static void registerSounds();

    static  SysPathName persistentFileName();
    
    //Indicate loading progress
    static void doLoadingProgressUpdate( DoLoadingProgressUpdate );

	static void persistentFileName( const SysPathName& name );

private:
    //Revoked
    MachPhysPreload( void );
    ~MachPhysPreload( void );
    MachPhysPreload( const MachPhysPreload& );
    MachPhysPreload& operator =( const MachPhysPreload& );
    bool operator ==( const MachPhysPreload& );
	static SysPathName& storedPersistentFileName( void );

};


#endif

/* End PRELOAD.HPP **************************************************/
