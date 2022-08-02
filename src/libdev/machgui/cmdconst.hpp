/*
 * C M D C O N S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiConstructCommand

    Implement interaction for the Construct command
*/

#ifndef _MACHGUI_CMDCONST_HPP
#define _MACHGUI_CMDCONST_HPP

#include "base/base.hpp"

#include "ctl/pvector.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/point3d.hpp"

#include "world4d/observer.hpp"

#include "machgui/command.hpp"
#include "machgui/gui.hpp"


//Forward refs
class MachPhysConstruction;
class MachLogConstruction;
class MachLogAdministrator;
class W4dDomain;

//orthodox canonical (revoked)
class MachGuiConstructCommand : public MachGuiCommand,
								public W4dObserver

{
public:
    //ctor.
    MachGuiConstructCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiConstructCommand();

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Return new instance of this command
    virtual MachGuiCommand* clone() const;

    //The resource file id of the prompt to be displayed when the cursor moves
    //over the command icon
    virtual uint cursorPromptStringId() const;

    //The resource file id of the prompt to be displayed when the command is clicked
    virtual uint commandPromptStringid() const;

    //reference to the up and down bitmap names for this command
    virtual const std::pair<string, string>& iconNames() const;

    //Called when the command becomes active
    //Default override does nothing
    virtual void start();

    //Called when the command ceases to be active
    //Default override does nothing
    virtual void finish();

    //Respond to pick at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed );

    //respond to pick on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed );

    //Respond to cursor at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed );

    //Respond to cursor on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed );

    //Set the type data associated with the command.
    virtual void typeData( MachLog::ObjectType objectType, int subType, uint level );

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const;

    //True iff there is an administer version of the command for squadrons
    //with an administrator. Default implementation returns false.
    virtual bool canAdminApply() const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	virtual void domainDeleted( W4dDomain* pDomain );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiConstructCommand& t );

private:
    // Operation deliberately revoked
    MachGuiConstructCommand( const MachGuiConstructCommand& );
    MachGuiConstructCommand& operator =( const MachGuiConstructCommand& );
    bool operator ==( const MachGuiConstructCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    //Execute the command as a sqaudron whose most intelligent administrator
    //is pAdministrator. If successful return true. Otherwise false, with
    //a prompt string indicating reason for failure in pReason.
    virtual bool doAdminApply( MachLogAdministrator* pAdministrator, string* pReason );
    //PRE( canAdminApply() );

    /////////////////////////////////////////////////

    //Compute the space to allow around the origin
    void computeBorder();

    //True if the current location is valid
    bool isValidPosition();

    //Ensure the display reflects whether the current location is valid or not
    void markValidPosition( bool isValid );

    //Place a new construction at specified location
    void placeConstruction();
	// PRE( locationIsValid_ );
	// PRE( MachLogRaces::instance().nBuildingMaterialUnits( MachLogRaces::instance().pcController().race() ) >= 5 );

	//Determines which way it faces. Orientation is MexDegrees( 90 * orientation_ )
    static int& orientation();

	// already have this construction in our vector
	bool constructionIsDuplicate( const MachLogConstruction* pCandidateConstruction ) const;

    //  Stop any machines that will run into the given construction in the future.
    void stopIntersectingMachines( const MachLogConstruction& construction );

	enum InvalidPosReason { MINETOCLOSETOSMELTER, SMELTERTOCLOSETOMINE, UNEVENTERRAIN, TOCLOSETOOBSTACLES, INFOGOFWAR, NOTENOUGHCASH, MAXUNITS };

    //Data members
    bool 					hadFinalPick_; 		//True when valid location for the mine has been picked
    bool 					locationIsValid_; 	//True if current location is ok
    int 					subType_; 			//Construction subtype - generic enumeration value
    uint 					level_; 			//hardware level
    W4dDomain* 				pOldDomain_; 		//The last domain in which the construction was drawn
    MexPoint3d 				location_; 			//Location for the building
    MexAlignedBox2d 		border_;			//The space required around the construction
    MachPhysConstruction* 	pPhysConstruction_; //The physical construction
    MachLog::ObjectType 	objectType_;		//Requested construction type
    MachGui::ControlPanelContext previousMenuContext_; //The control panel context on entry to command
	ctl_pvector< MachLogConstruction > allConstructions_;
	ctl_pvector< MachLogConstruction > newConstructions_; // Delete these if operation is aborted
	InvalidPosReason 		invalidPosReason_;  //Reason why a construction cannot be built
};

#endif

/* End CMDCONST.HPP **************************************************/
