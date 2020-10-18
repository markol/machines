/*
 * C O N S T R O N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstruction

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_CONSTRON_HPP
#define _MACHPHYS_CONSTRON_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"

#include "machphys/machphys.hpp"
#include "mathex/abox2d.hpp"

//Forward decls
class MachPhysEntrance;
class W4dGenericComposite;
class MachPhysPlanetSurface;
class MachPhysEntityDamage;
class MachPhysObjectExplosion;
class MachPhysConstructionImpl;
class MachPhysConstructionData;
class MachPhysCanAttack;
class MachPhysMissileEmplacement;
class MachPhysPod;

class MachPhysConstruction : public W4dComposite
{
public:
    virtual ~MachPhysConstruction( void );

    enum { N_BUILDING_LEVELS = 10 };

    void CLASS_INVARIANT;

	static void preloadConstructingPlans();

    void    percentageComplete( double newPercentage );
    // PRE( newPercentage >= 0.0 and newPercentage <= 100.0 );
    double  percentageComplete( void ) const;
    // POST( result >= 0.0 and result <= 100.0 );

    virtual PhysRelativeTime    destroy( void );

	//return the MachPhysData object for the building.
	virtual const MachPhysConstructionData& constructionData() const = 0;

    size_t  level( void );
    size_t  level( void ) const;

    //Get/set working state
    void isWorking( bool doWork );
    bool isWorking() const;

    ////////////////////////////////////

    //Number of entrances for which animation data is available.
    //Default implementation is zero.
    virtual uint nEntrances() const;

    //Initiate closing/opening entrance number index.
    //returns time for any required animation. (Zero if already in state).
    PhysRelativeTime isEntranceOpen( uint index, bool doOpen );
    //PRE( index < nEntrances() );

    //true if entrance number index is in open state. False otherwise.
    bool isEntranceOpen( uint index ) const;
    //PRE( index < nEntrances() );

    ////////////////////////////////////

    //  This is true iff the building was constructed using the constructor
    //  which specifies an interior composite.
    bool    hasInterior( void ) const;

    W4dDomain* pInteriorDomain( void ) const;
    // PRE( hasInterior() );
    // POST( result != NULL );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

	//Called when a building is captured and changed to a new race
	void changeRace( MachPhys::Race );

	//Called when a building takes the colour of that of newRace  without changing race
	void changeColour( MachPhys::Race newRace );

    //The race
    MachPhys::Race race() const;

	//virtual damage level setting let the individual constructions set their own damage smoke type
	virtual void damageLevel( const double& percent );

	const double& damageLevel() const;
	void updateDamageLevel(); //to be called every 60 seconds
	bool damaged() const;

	void damageSmoke1Type( const MachPhysPuffType& type1 );
	void damageSmoke2Type( const MachPhysPuffType& type2 );

    void convertMaterials( const MachPhysConstructionData& data, MachPhys::Race );

    //Does this machine have an associated MachPhysCanAttack
    bool hasCanAttack() const;
    //Return the associated MachPhysCanAttack
    MachPhysCanAttack* canAttack() const;
    //PRE(hasCanAttack());

	bool hasSavedPadsTransforms() const;
	void savePadsTransforms();
	// PRE( not hasSavedPadTransforms() );
	void restorePadsTransformsFromSave();
	void deleteSavedPadsTransforms();

	// At construction time, adjust pads position on an uneven terrain
	void layPadsOnTerrain( const MachPhysPlanetSurface& terrain );

    // Return the largest possible boundary for this construction. This
    // will include the pads amd the entrance polygons.
    MexAlignedBox2d largestBoundary() const;
    // Same as above but this is in local coordinates, not
    // global coordinates.
    MexAlignedBox2d localLargestBoundary() const;

    //  This is just the boundary for the main part of the
    //  construction - it doesn't include pads.
    MexAlignedBox2d externalGlobalBoundary() const;

    PER_MEMBER_PERSISTENT_ABSTRACT( MachPhysConstruction );
    PER_FRIEND_READ_WRITE( MachPhysConstruction );

protected:
    MachPhysConstruction(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      const SysPathName& compositeFileName,
	  const SysPathName& wireframeFileName,
      double height,
      size_t level,
      const MachPhysConstructionData& data );

    MachPhysConstruction(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      const SysPathName& compositeFileName,
      const SysPathName& wireframeFileName,
      const SysPathName& interiorCompositeFileName,
      double height,
      size_t level,
      const MachPhysConstructionData& data );
    // POST( hasInterior() );

    // Copy a building.
    MachPhysConstruction(
      const MachPhysConstruction& copyMe,
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      size_t level,
      MachPhys::Race race );

    void    adornmentsVisible( bool visible );

    void    adornments( const W4dLinks& newAdornments );

    virtual void doPercentageComplete( double newPercentage );

    virtual void doWorking( bool isWorking );

    PhysRelativeTime    doDestroy( void );

    //  Derived classes use this when doing a persistent read to make sure the
    //  internal copy is correctly set up.
    void persistenceConstructionData( const MachPhysConstructionData& data );

	W4dGenericComposite* pInteror();
	//PRE( hasInteror() );

private:
    // Operation deliberately revoked
    MachPhysConstruction( const MachPhysConstruction& );

    // Operation deliberately revoked
    MachPhysConstruction& operator =( const MachPhysConstruction& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysConstruction& );

    //Return pointer to the entrance number index.
    //Default implementation asserts out.
    virtual MachPhysEntrance* pEntrance( uint index ) const;
    //PRE( index < nEntrances() );

    //  Setup the physical entrance data for this building
    void    setupEntrances( void );

    //  The adornments are those parts of the object which are not visible
    //  while the object is being built but pop in to existance when it
    //  is completed.

	// adjust the pad position on an uneven terrain
	void layPadOnTerrain( const MachPhysPlanetSurface& terrain, W4dLink& pad );

    MachPhysMissileEmplacement& asMissileEmplacement() const;
    // PRE( constructionData().constructionType() == MachPhys::MISSILE_EMPLACEMENT );

    MachPhysPod& asPod() const;
    // PRE( constructionData().constructionType() == MachPhys::POD );

    MachPhysConstructionImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysConstruction );

#endif

/* End CONSTRON.HPP *************************************************/
