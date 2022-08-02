/*
 * C O N S T R O I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysConstructionImpl

    Implementation class for MachPhysConstruction
*/

#ifndef _MACHPHYS_CONSTROI_HPP
#define _MACHPHYS_CONSTROI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "machphys/consdata.hpp"

class W4dGenericComposite;
class MachPhysEntrance;
class MachPhysEntityDamage;
class MachPhysObjectExplosion;

class MachPhysConstructionImpl
// Canonical form revoked
{
public:
    MachPhysConstructionImpl( double height, size_t level, MachPhys::Race race, const MachPhysConstructionData& data, const MexTransform3d& globalTransform );

    ~MachPhysConstructionImpl();

    typedef ctl_vector< W4dLink* > W4dLinks;
    typedef ctl_vector< MachPhysEntrance* > Entrances;

    enum CompletionBand{ BAND0, BAND1, BAND2, BAND3, BAND4, BAND5, BAND6, BAND7, BAND8, BAND9, BAND10, NBANDS };

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysConstructionImpl );
    PER_FRIEND_READ_WRITE( MachPhysConstructionImpl );

private:
    friend ostream& operator <<( ostream& o, const MachPhysConstructionImpl& t );
    friend class MachPhysConstruction;

    MachPhysConstructionImpl( const MachPhysConstructionImpl& );
    MachPhysConstructionImpl& operator =( const MachPhysConstructionImpl& );

public:
	struct LinkTransformData
	{
		LinkTransformData() {}
		LinkTransformData( W4dLink::Id newId, const MexTransform3d& newTransform )
			: id( newId ),
		  transform( newTransform ) {}
		W4dLink::Id id;
		MexTransform3d transform;
	};

	typedef ctl_vector< LinkTransformData > LinkTransforms;

private:

	void createSavedPadsTransforms();
	LinkTransforms& savedPadsTransforms();
	const LinkTransforms& savedPadsTransforms() const;
	bool savedPadTransform( W4dLink::Id id, MexTransform3d* transform ) const;
	// PRE( transform );

    //data members
    W4dLinks    					adornments_;

    double      					percentageComplete_;
    double      					height_;
    size_t      					level_;
    MachPhys::Race    				race_;

    bool                    isWorking_;
    W4dDomain*              pInteriorDomain_;
    W4dGenericComposite*    pInteriorComposite_;

    MachPhysConstructionData constructionData_;

    //typedef ctl_vector< MachPhysEntrance* > Entrances;

    Entrances   entrances_;
	MachPhysEntityDamage* pDamage_;
	MachPhysObjectExplosion* pExplosion_;
	LinkTransforms* pSavedPadsTransforms_;

	CompletionBand completionBand_;
    bool completionVisualised_; //True once the percent complete animation has been kicked off
                                //after creation
};

PER_DECLARE_PERSISTENT( MachPhysConstructionImpl );


#endif

/* End CONSTROI.HPP *************************************************/
