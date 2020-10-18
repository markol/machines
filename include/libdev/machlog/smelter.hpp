/*
 * S M E L T E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogSmelter

    Handle logical operations for a Smelter construction
*/

#ifndef _MACHLOG_SMELTER_HPP
#define _MACHLOG_SMELTER_HPP

#include "base/base.hpp"

#include "machphys/machphys.hpp"

#include "machlog/constron.hpp"
#include "machlog/mine.hpp"

//Forward declarations
class MachPhysSmelter;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachPhysSmelterData;

//Orthodox canonical (revoked)
class MachLogSmelter : public MachLogConstruction
{
public:
    //Construct smelter of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogSmelter( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle );

    MachLogSmelter( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId );

    ~MachLogSmelter( void );

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysSmelterData& data() const;
	
	static bool validSmelterSite( const MexPoint3d&, const MachLogMine::CheckDiscoveredFlag );
	static bool validForAllMines ( const MexPoint3d& location );
	//static bool outsideMinimumMineSmelterRange ( const MexPoint3d& location, const MachLogMine* pMine);

	virtual void assignToDifferentRace( MachLogRace& newRace );
	
    void CLASS_INVARIANT;

	MachPhys::BuildingMaterialUnits		inStorage() const;
	MachPhys::BuildingMaterialUnits&	inStorage();

	void droppedOffOre();
	
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, EchoBeHit = MachActor::ECHO );
						
	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, 
						MachActor* pByActor = NULL, EchoBeHit = MachActor::ECHO );

    friend ostream& operator <<( ostream& o, const MachLogSmelter& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSmelter );
	PER_FRIEND_READ_WRITE( MachLogSmelter );

private:
    // Operations deliberately revoked
    MachLogSmelter( const MachLogSmelter& );
    MachLogSmelter& operator =( const MachLogSmelter& );
    bool operator ==( const MachLogSmelter& );

    //Construct a new physical smelter. Used in ctor initializer list
    static MachPhysSmelter* pNewPhysSmelter( MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );
 
    //The physical missile emplacement
    MachPhysSmelter* pPhysSmelter();
    const MachPhysSmelter* pPhysSmelter() const;

    //Data members
	MachPhys::BuildingMaterialUnits		inStorage_;
	PhysAbsoluteTime					droppedOffOreTime_;
	bool								addedBMUStorageToRace_;
   
};

PER_DECLARE_PERSISTENT( MachLogSmelter );

#ifdef _INLINE
    #include "machlog/smelter.ipp"
#endif


#endif

/* End SMELTER.HPP **************************************************/
