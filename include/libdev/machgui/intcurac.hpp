/*
 * I N T C U R A C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiIntelligentCursorOnActor

    Works out what the intelligent cursor id should be for a given selected actor,
    when the cursor is over a second actor.
*/

#ifndef _MACHGUI_INTCURAC_HPP
#define _MACHGUI_INTCURAC_HPP

#include "base/base.hpp"
#include "machgui/restorer.hpp"
#include "machgui/gui.hpp"

//Forward refs
class MachActor;
class MachInGameScreen;

//orthodox canonical (revoked)
class MachGuiIntelligentCursorOnActor : public MachLogTypeRestorer
{
public:
    //ctor.
    MachGuiIntelligentCursorOnActor();

    //dtor.
    virtual ~MachGuiIntelligentCursorOnActor();

    //returns the intelligent cursor to be displayed for the action to be performed
    //by pSelectedActor if a click is made on pActorUnderCursor.
    MachGui::Cursor2dType cursorType( MachActor* pSelectedActor,
                                      MachActor* pActorUnderCursor,
									  MachInGameScreen* pInGameScreen,
                                      bool ctrlPressed,
                                      bool shiftPressed,
                                      bool altPressed );

protected:
    ////////////////////////////////////////////////////////////
    // Inherited from MachGuiTypeRestorer

	virtual ProcessAsSubtype doProcessConstruction( MachLogConstruction * );
	virtual ProcessAsSubtype doProcessMachine( MachLogMachine * );
	virtual ProcessAsSubtype doDefaultProcess( MachActor * );

	virtual ProcessAsSubtype doProcessAdministrator( MachLogAdministrator * );
	virtual ProcessAsSubtype doProcessAggressor( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessConstructor( MachLogConstructor * );
	virtual ProcessAsSubtype doProcessGeoLocator( MachLogGeoLocator * );
	virtual ProcessAsSubtype doProcessSpyLocator( MachLogSpyLocator * );
	virtual ProcessAsSubtype doProcessTechnician( MachLogTechnician * );
	virtual ProcessAsSubtype doProcessPersonnelCarrier( MachLogAPC * );
	virtual ProcessAsSubtype doProcessResourceCarrier( MachLogResourceCarrier * );

	virtual ProcessAsSubtype doProcessLocator( MachLogMachine * );
	virtual ProcessAsSubtype doProcessTransporter( MachLogMachine * );

    ////////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiIntelligentCursorOnActor& t );

private:
    // Operations deliberately revoked
    MachGuiIntelligentCursorOnActor( const MachGuiIntelligentCursorOnActor& );
    MachGuiIntelligentCursorOnActor& operator =( const MachGuiIntelligentCursorOnActor& );
    bool operator ==( const MachGuiIntelligentCursorOnActor& );

    //Do standard machine processing
    void processStandardMachine( MachLogMachine* pActor );

	//bool canAtLeastOneCorralActorEnterConstruction( const MachLogConstruction& );
 	
    //Data members
    MachActor* pCursorActor_; //The actor under the cursor
    MachGui::Cursor2dType useCursor_; //Set as the cursor type to use
	bool ctrlPressed_;
	bool shiftPressed_;
	bool altPressed_;
	MachInGameScreen* pInGameScreen_;
};


#endif

/* End INTCURAC.HPP *************************************************/
