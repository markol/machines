/*
 * O R E H O L O . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysOreHolograph

    Implements a holograph display for a mineral deposit marker
*/

#ifndef _MACHPHYS_OREHOLO_HPP
#define _MACHPHYS_OREHOLO_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"

#include "machphys/machphys.hpp"

//Orthodox canonical (revoked)
class MachPhysOreHolograph : public W4dComposite
{
public:
    //ctor. race determines its main colour.
    //The size is defined by concentration.
    //The number of satellites is defined by quantity .
    MachPhysOreHolograph( W4dEntity* pParent, const W4dTransform3d& localTransform,
                          MachPhys::Race race, uint concentration, uint quantity );

    //dtor
    ~MachPhysOreHolograph( void );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysOreHolograph );
    PER_FRIEND_READ_WRITE( MachPhysOreHolograph );

private:
    // Operations deliberately revoked
    MachPhysOreHolograph( const MachPhysOreHolograph& );
    MachPhysOreHolograph& operator =( const MachPhysOreHolograph& );
    bool operator ==( const MachPhysOreHolograph& );

	friend class MachPhysOtherPersistence;
    //ctor used by the factory() method
    MachPhysOreHolograph( W4dEntity* pParent, const MexTransform3d& transform );

    //Constructs and returns the original holograph
    static const MachPhysOreHolograph& factory();

    //Returns the holo spin plan
    static const W4dCompositePlanPtr& spinPlan();

    //Constructs and returns the spin plan for the holograph
    static W4dCompositePlanPtr newSpinPlan( const MachPhysOreHolograph& holo );

    //Sets colours for different races
    void convertMaterials( MachPhys::Race race );
};

PER_DECLARE_PERSISTENT( MachPhysOreHolograph );

#endif

/* End OREHOLO.HPP **************************************************/
