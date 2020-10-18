/*
 * W A T E R F A L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWaterfall

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_WATERFAL_HPP
#define _MACHPHYS_WATERFAL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"

class MachPhysWaterfall : public W4dEntity
// Canonical form revoked
{
public:

    MachPhysWaterfall(  W4dEntity* pParent, const MexTransform3d& localTransform  );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysWaterfall& exemplar();

    ~MachPhysWaterfall( void );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWaterfall& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysWaterfall );

private:
	//deliberately revoked
    MachPhysWaterfall( const MachPhysWaterfall& );
    MachPhysWaterfall& operator =( const MachPhysWaterfall& );
    bool operator ==( const MachPhysWaterfall& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysWaterfall();
};

PER_READ_WRITE( MachPhysWaterfall );
PER_DECLARE_PERSISTENT( MachPhysWaterfall );

#endif

/* End WATERFAL.HPP *************************************************/
