#ifndef _D3DAPP_HPP
#define _D3DAPP_HPP

#define MOST_DERIVED_APP SDLApp

#include "base/istrrep.hpp"
#include "device/timer.hpp"
#include "afx/osapp.hpp"

class RenDisplay;
class RenTextureSet;
class W4dEntity;
class W4dSceneManager;
class W4dRoot;
class MachKeyFocus;
class MachActor;
class GuiRoot;
class MexTransform3d;
class MexPoint2d;
class MexPoint3d;
class MachGuiStartupScreens;

// This class represents an application based on Direct3D.  When we know
// what's common to all D3D apps, this probably ought to become an abstract
// base class.  It's based on the application framework, so there's no main,
// just this class.
class SDLApp : public AfxOSSpecificApp<SDLApp>
{
public:
        virtual ~SDLApp();
        enum StartedFromLobby { LOBBY_START, NORMAL_START };

private:
        friend class AfxSingletonApp;
        SDLApp();

        // Provide virtual fns required by base classes.
        virtual void clientShutdown();
        virtual void loopCycle();
        virtual void updateDisplay();
        virtual bool activate();
        virtual bool clientStartup ();
    virtual const string& name() const;

    void moveObject( W4dEntity* pObject );

        // Allow a derived class to override this and provide a window size.
        // This shouldn't be necessary when DevDisplay works with Win95 & D3D.
        virtual void getWindowSize(int& ox, int& oy, int& w, int& h);

        void outputDebugInfo( const MexPoint2d& pos, const MexTransform3d& xform, MachActor* pActor );

        // Sim
        void checkSuspendSimManager();
        void performSimCycle();
        void simResetTime();
        void simSuspend();
        void simResume();

        // Gui
        void cleanUpGui();
        void initialiseGui( StartedFromLobby, BaseProgressReporter* );
        void updateGui();
        void displayGui();
        void checkSwitchGuiRoot();
        void checkFinishApp();
        void activateGui();

        // Sound
        void initSound();
        //Load sounds loads either the 3D or 2D sounds according to
        //the registry setting
        void loadSounds();
        void cleanUpSound();
        void updateSound( const MexPoint3d& pos );

        void loadPlanets();

    int                                 winWidth_, winHeight_;
        double              runTime_;
        RenDisplay*                     pDisplay_;
    W4dRoot*                    pRoot_;
        W4dSceneManager*        manager_;
        MachKeyFocus*           pKeyboardFocus_;
        bool                            showPosition_, showCurrentMachine_, showMemory_;
        bool                aShowRace_[4];
        bool                            initialised_;
    DevTimer                    keyTimer_;
        DevTimer            finishTimer_;
        MachGuiStartupScreens* pStartupScreens_;
        RenTextureSet*          pTextureSet_;

        size_t                          savedSampleVolume_;
        size_t                          savedCDVolume_;
};

#endif
