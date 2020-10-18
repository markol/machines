/*
 * GARRISON . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogGarrison

    Handle logical operations for a Garrison construction
*/

#ifndef _MACHLOG_GARRISON_HPP
#define _MACHLOG_GARRISON_HPP

#include "base/base.hpp"

#include "machlog/constron.hpp"

//Forward declarations
class MachPhysGarrison;
class MachPhysGarrisonData;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachLogGarrisonImpl;

//Orthodox canonical (revoked)
class MachLogGarrison : public MachLogConstruction
{
public:
    //Construct Garrison of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogGarrison( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle );

    MachLogGarrison( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId );

    ~MachLogGarrison( void );

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysGarrisonData& data() const;

	//inherited from MachActor
    virtual int militaryStrength() const;
	
	void healAll();
    	
	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData ); 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogGarrison& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogGarrison );
	PER_FRIEND_READ_WRITE( MachLogGarrison );

private:
    // Operations deliberately revoked
    MachLogGarrison( const MachLogGarrison& );
    MachLogGarrison& operator =( const MachLogGarrison& );
    bool operator ==( const MachLogGarrison& );

    //Construct a new physical Garrison. Used in ctor initializer list
    static MachPhysGarrison* pNewPhysGarrison( MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );
					 
	virtual void doOutputOperator( ostream& o ) const;
					 
	void startHealing( MachLogMachine* pMachine );
	// PRE( not currentlyHealing( pMachine ) );
	// POST( currentlyHealing( pMachine ) );
	
	void stopHealing( MachLogMachine* pMachine );
	// PRE( currentlyHealing( pMachine ) );
	// POST( not currentlyHealing( pMachine ) );
	
	bool currentlyHealing( MachLogMachine* pMachine );
					 
    //The physical garrison
    MachPhysGarrison* pPhysGarrison();
    const MachPhysGarrison* pPhysGarrison() const;


    //Data members
	MachLogGarrisonImpl*			pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogGarrison );

#ifdef _INLINE
    #include "machlog/Garrison.ipp"
#endif


#endif

/* End Garrison.HPP **************************************************/
