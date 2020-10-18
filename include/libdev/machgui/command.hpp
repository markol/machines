/*
 * C O M M A N D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiCommand

    An ABC for the objects used to implement each interactive command.
    Is notified of mouse events in the world view window, and determines
    appropriate action.
    On completion of command input, applies the command to each appropriate
    actor. Determines which actors can execute the command.
*/

#ifndef _MACHGUI_COMMAND_HPP
#define _MACHGUI_COMMAND_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "machgui/gui.hpp"
#include "machlog/machlog.hpp"

//Forward refs
class MexPoint2d;
class MexPoint3d;
class MachInGameScreen;
class MachActor;
class MachLogOperation;
class MachLogAdministrator;
class MachGuiCommandImpl;
class MachLogMachine;
template < class X, class Y > class pair;
template < class T > class ctl_pvector;
template < class T > class ctl_list;

//Orthodox canonical (revoked)
class MachGuiCommand
{
public:
    //ctor.
    MachGuiCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiCommand();

    //Return new instance of the command
    virtual MachGuiCommand* clone() const = 0;

    //reference to the up and down bitmap names for this command
    virtual const std::pair<string, string>& iconNames() const = 0;

    //The resource file id of the prompt to be displayed when the cursor moves
    //over the command icon
    virtual uint cursorPromptStringId() const = 0;

    //The resource file id of the prompt to be displayed when the command is clicked
    virtual uint commandPromptStringid() const = 0;

    //Called when the command becomes active
    //Default override does nothing
    virtual void start();

    //Called when the command ceases to be active
    //Default override does nothing
    virtual void finish();

    //Respond to pick at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed ) = 0;

    //respond to pick on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed ) = 0;

    //Respond to cursor at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnTerrain( const MexPoint3d& location,
                         bool ctrlPressed, bool shiftPressed, bool altPressed ) = 0;

    //Respond to cursor on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed ) = 0;

    //Set the type data associated with the command.
    virtual void typeData( MachLog::ObjectType objectType, int subType, uint level ) = 0;

    //True if this command should be visible based on selected actor set.
    //Ignores actor race in determining result.
    virtual bool actorsCanExecute() const;

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const = 0;

    //True iff there is an administer version of the command for squadrons
    //with an administrator. Default implementation returns false.
    virtual bool canAdminApply() const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const = 0;

    //Execute the command for each selected actor which can do it.
    void apply();
    //PRE( interactionIsComplete() )

    //true when the command has been executed
    bool isExecuted() const;

    //true iff command currently can be rightfully executed
    bool isVisible() const;

    //Set flag indicating whether command currently can be rightfully executed
    void isVisible( bool visible );

    //The top level in-game component
    MachInGameScreen& inGameScreen() const;
    void CLASS_INVARIANT;

	// useful to prevent multiple voicemails being issued from the same command's execution
	bool hasPlayedVoiceMail() const;
	void hasPlayedVoiceMail( bool );

	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

	// Indicate if the command is being executed in a FOW area
	static bool cursorInFogOfWar();
	static void cursorInFogOfWar( bool );

    friend ostream& operator <<( ostream& o, const MachGuiCommand& t );

protected:

    enum ObstacleModifier {
        USE_ALL_OBSTACLES,                  // Check against all obstacles
                                            // in the configuration space
        IGNORE_SELECTED_ACTOR_OBSTACLES,    // Check against all obstacles except those
                                            // which the currently selected set of actors
                                            // can travel over
        IGNORE_ALL_ACTOR_OBSTACLES          // Check against all obstacles except those
                                            // which some actor can travel over (e.g. water,
                                            // low obstacle)
    };

    //  Return true iff the point is in a valid domain and
    //  it is not inside an obstacle
    bool isPointValidOnTerrain( const MexPoint2d&, ObstacleModifier ) const;

	// Takes a point that we'd like to move/locate/patrol to and finds the closest point
	// that we can get to the original point.
	bool findClosestPointValidOnTerrain(const MexPoint2d& point,
		 							    ObstacleModifier modifier,
		 					    		MachLogMachine* pMachine,
		 					    		MexPoint2d* pClosestPoint ) const;

	bool findClosestPointValidOnTerrain(const MexPoint2d& endPoint,		// Point that we are trying to get close to
										const MexPoint2d& startPoint, 	// Point we are starting from
		 							    ObstacleModifier modifier,		// Which obstacles to take into account
		 					    		MATHEX_SCALAR clearance,		// How far the point must be from an obstacle
										MATHEX_SCALAR stepDistance,		// How far we move each time we fail to find a valid point
		 					    		MexPoint2d* pClosestPoint ) const; // Closest point returned in this variable

	// Takes a vector of potentially invalid points and converts them to valid points.
	// Converts from vector of 3d points to list of 2d points.
	// PRE( pPath );
	// PRE( pPath->size() == 0 );
	bool convertPointsToValidPoints(	ObstacleModifier modifier,
										MachLogMachine* pMachine,
										const ctl_vector< MexPoint3d >& invalidPath,
										ctl_list< MexPoint2d >* pPath );

	// Converts from vector of 2d points to list of 2d points.
	bool convertPointsToValidPoints(	ObstacleModifier modifier,
										MachLogMachine* pMachine,
										const ctl_vector< MexPoint2d >& invalidPath,
										ctl_list< MexPoint2d >* pPath );

	// Converts from vector of 3d points to vector of 2d points.
	bool convertPointsToValidPoints(	ObstacleModifier modifier,
										MachLogMachine* pMachine,
										const ctl_vector< MexPoint3d >& invalidPath,
										ctl_vector< MexPoint2d >* pPath );

    static size_t& commandId();

    void add( MachLogMachine*, MachLogOperation* );

    PhysPathFindingPriority pathFindingPriority() const;

    typedef ctl_pvector< MachActor > Actors;

private:
    // Operation deliberately revoked
    MachGuiCommand( const MachGuiCommand& );
    MachGuiCommand& operator =( const MachGuiCommand& );
    bool operator ==( const MachGuiCommand& );

    //Execute the command for pActor, returning true if done so,
    //or false if not possible. If the latter, a prompt string explaining
    //the reason is returned in pReason.
    virtual bool doApply( MachActor* pActor, string* pReason ) = 0;
    //PRE( isInteractionComplete() )

    //Execute the command as a sqaudron whose most intelligent administrator
    //is pAdministrator. If successful return true. Otherwise false, with
    //a prompt string indicating reason for failure in pReason.
    virtual bool doAdminApply( MachLogAdministrator* pAdministrator, string* pReason );
    //PRE( canAdminApply() );

    //  True iff this command can be applied to a group of actors at once
    virtual bool canApplyToGroup() const;

    //typedef ctl_pvector< MachActor > Actors;

    //Execute the command for all actors in actors, returning true if done so,
    //or false if not possible. If the latter, a prompt string explaining
    //the reason is returned in pReason.
    virtual bool doGroupApply( const Actors& actors, string* pReason );
    //PRE( isInteractionComplete() )

    typedef uint32  ObstacleFlags;

    //  Return the bitwise OR of the obstacle flags for all selected
    //  actors that are machines
    ObstacleFlags selectedActorObstacleFlags() const;

	static bool& inFogOfWarFlag();

    //Data members
    bool isExecuted_; // True when the command has been executed
    bool isVisible_; //True if currently deemed available for display and interaction
    bool isSquadronContext_; //True if the command was given in the context of squadron
	bool hasPlayedVoiceMail_;	// initially false - becomes true once any voicemail assoc'd with the command has played

    MachGuiCommandImpl* pImpl_;
};


#endif

/* End COMMAND.HPP **************************************************/

