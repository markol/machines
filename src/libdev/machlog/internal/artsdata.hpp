/*
 * A R T S D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogArtefactSubType
    MachLogArtefactsData

    Implementation classes for MachLogArtefacts
*/

#ifndef _MACHLOG_ARTSDATA_HPP
#define _MACHLOG_ARTSDATA_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "machphys/artfdata.hpp"

//forward refs
class W4dEntity;

class MachLogArtefactSubType
{
public:
    MachLogArtefactSubType( int subType, const W4dEntity* pExemplar, const string& name )
    :   subType_( subType ),
        pExemplar_( pExemplar ),
        name_( name )
    {};

    //////////////////////////////////////////////
    //These functions required by ctl_vector instantiation. Not required otherwise.
    MachLogArtefactSubType()
    :   subType_( 0 ),
        pExemplar_( NULL )
    {};

    friend bool operator< ( const MachLogArtefactSubType& lhs, const MachLogArtefactSubType& rhs )
    {
        return lhs.subType_ < rhs.subType_;
    };

    friend bool operator== ( const MachLogArtefactSubType& lhs, const MachLogArtefactSubType& rhs )
    {
        return lhs.subType_ == rhs.subType_;
    };

    //////////////////////////////////////////////

    //data members
    int subType_; //The subtype value
    const W4dEntity* pExemplar_; //The exemplar for this subtype
    string name_; //Name as referenced in the artefacts repository
};

class MachLogArtefactsData
// Canonical form revoked
{
private:
    friend class MachLogArtefacts;
    friend class MachLogArtefactsImpl;

    MachLogArtefactsData()
    :   pSubTypes_( NULL )
    {
        artefactDatas_.reserve( 64 );
    };

    typedef ctl_pvector< MachPhysArtefactData > ArtefactDatas;

    //data members
    ctl_vector< MachLogArtefactSubType >* pSubTypes_;
    ArtefactDatas artefactDatas_;
};


#endif

/* End ARTSDATA.HPP *************************************************/
