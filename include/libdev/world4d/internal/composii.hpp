/*
 * C O M P O S I I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCompositeImpl

    Data implementation for W4dComposite
*/

#ifndef _WORLD4D_COMPOIMPL_HPP
#define _WORLD4D_COMPOIMPL_HPP

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/double.hpp"

#include "render/hierbuil.hpp"

#include "world4d/world4d.hpp"
#include "world4d/fwrdplan.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"

class SysPathName;
class UtlLineTokeniser;
class W4dCompositePlan;
class W4dComposite;

class W4dCompositeImpl
// Canonical form revoked
{
public:

    ~W4dCompositeImpl();

    //Persistence
    PER_MEMBER_PERSISTENT( W4dCompositeImpl );
    PER_FRIEND_READ_WRITE( W4dCompositeImpl );

private:
	friend class W4dComposite;
    /////////////////////////////////////////////////////////////////
    typedef ctl_vector< W4dCompositePlanPtr > CompositePlans;

    //Used in implementaion of .xan file parsing
    typedef size_t KeyFrameId;
    typedef std::pair<KeyFrameId, MexQuaternion> KeyFrameOrientation;
    typedef ctl_vector<KeyFrameOrientation> KeyFrameOrientations;
    typedef std::pair<KeyFrameId, MexVec3> KeyFrameLocation;
    typedef ctl_vector<KeyFrameLocation> KeyFrameLocations;
    typedef std::pair<W4dEntity*, W4dTransform3d> StoredLinkPosition;
    typedef ctl_vector< StoredLinkPosition > StoredLinkPositions;


//    W4dCompositeImpl( W4dComposite* pParent, const W4dTransform3d& localTransform, W4dComposite::Solidity solid );
	W4dCompositeImpl( W4dComposite* pComposite );
    W4dCompositeImpl( const W4dCompositeImpl& copyMe, W4dComposite* pComposite );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCompositeImpl& t );
    W4dCompositeImpl& operator =( const W4dCompositeImpl& );

    bool readAnimation( const SysPathName& fileName,
                         const string& animationName,
                         W4dCompositePlan* pCompositePlan,
                         MATHEX_SCALAR framesPerSecond ) const;

    void parseModel( const SysPathName& directoryname, UtlLineTokeniser* pParser, W4dLOD id );
    void parseMeshRanges( const SysPathName& directoryname, UtlLineTokeniser* pParser );
    void parseShadow( const SysPathName& directoryname, UtlLineTokeniser* pParser );
    void parseAnimation( const SysPathName& directoryname, UtlLineTokeniser* pParser );

    void buildHierarchy( W4dEntity* pParent, const RenHierarchyBuilder::Node* pNode, double distance, W4dLOD id );
    void addEmptyMeshes( double distance, W4dLOD id );

    bool parseLinkAnimation(
        UtlLineTokeniser* pParser,
        MATHEX_SCALAR framesPerSecond,
        const string& linkName,
        PhysMotionPlanPtr* pPlanPtr ) const;

    bool animationValid(
        const string& linkName,
        const KeyFrameOrientations& orientations,
        const KeyFrameLocations& locations ) const;

    //True if a link named id exists, in which case it's pointer is
    //returned in ppLink.
    bool findLink( const string& id, W4dLink** ppLink ) const;

    PhysMotionPlanPtr makeAnimationPlan( const KeyFrameOrientations& orientations,
                                   const KeyFrameLocations& locations,
                                   MATHEX_SCALAR framesPerSecond ) const;

	MexQuaternion interpolatedOrientation(const KeyFrameOrientations& orientations, const KeyFrameId& id) const;
	MexPoint3d interpolatedPosition(const KeyFrameLocations& locations, const KeyFrameId& id) const;


	//data members

    W4dComposite*						pComposite_;
    W4dComposite::W4dLinks				links_;
    W4dLink::Id 						maxLinkId_;
    StoredLinkPositions*			 	pStoredLinkPositions_;
    W4dComposite::HeldEntities*			pHeldEntities_; //Entities attached to links via the hold() method

	// This is the maximum LOD range for any mesh in this composite.
	W4dEntity::W4dDistance	maxRange_;
    MexAlignedBox3d         compositeBoundingVolume_; //Encloses composite and all links
                                                      //Only updated on request

    CompositePlans          compositePlans_;

    /////////////////////////////////////////////////////////////////
};

PER_DECLARE_PERSISTENT( W4dCompositeImpl );

#endif

/* End ENTYDATA.HPP *************************************************/
