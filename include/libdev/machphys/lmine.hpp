/*
 * L M I N E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLandMine

    Land Mines. Used by the spies.
*/

#ifndef _MACHPHYS_LMINE_HPP
#define _MACHPHYS_LMINE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"
#include "machphys/destroy.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MexTransform3d;

class MachPhysLandMine : public W4dEntity, public MachPhysCanBeDestroyed
// Canonical form revoked
{
public:
    //ctor. Object attached to pParent at localTransform
    MachPhysLandMine( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Race );

    //dtor
    virtual ~MachPhysLandMine();

    //Return an exemplar disc - ensures the disc mesh is loaded
    static const MachPhysLandMine& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    //  Destroy yourself
    //  Return the time that will be taken for the animation
    virtual PhysRelativeTime beDestroyed( void );

    friend ostream& operator <<( ostream& o, const MachPhysLandMine& t );

    PER_MEMBER_PERSISTENT( MachPhysLandMine );
    PER_FRIEND_READ_WRITE( MachPhysLandMine );

private:
    MachPhysLandMine( const MachPhysLandMine& );
    MachPhysLandMine& operator =( const MachPhysLandMine& );
    bool operator ==( const MachPhysLandMine& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLandMine();

	MachPhys::Race			race_;
};

PER_DECLARE_PERSISTENT( MachPhysLandMine );

#endif

/* End LMINE.HPP *************************************************/
