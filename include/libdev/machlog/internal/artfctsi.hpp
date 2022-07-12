/*
 * A R T F C T S I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogArtefactsImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_ARTFCTSI_HPP
#define _MACHLOG_ARTFCTSI_HPP

#include "base/base.hpp"
#include "machlog/artfacts.hpp"
#include "machphys/artfdata.hpp"

class SysPathName;

class MachLogArtefactsImpl
// Canonical form revoked
{
public:
    MachLogArtefactsImpl();
    ~MachLogArtefactsImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_DEFAULT( MachLogArtefactsImpl );
	PER_FRIEND_READ_WRITE( MachLogArtefactsImpl );

private:
	friend class MachLogArtefacts;
    friend ostream& operator <<( ostream& o, const MachLogArtefactsImpl& t );

    MachLogArtefactsImpl( const MachLogArtefactsImpl& );
    MachLogArtefactsImpl& operator =( const MachLogArtefactsImpl& );

    W4dGenericRepository* 	pExemplars_; //repository of generic artefact models used as exemplars
    MachLogArtefactsData* 	pData_; //Implementation class used for holding the data
	SysPathName*			pArtefactPathName_;	//used to recreate artefacts data on a load game.
	bool 					artefactsParsed_; //gets set when the artefacts are parsed.

};

PER_DECLARE_PERSISTENT( MachLogArtefactsImpl );

#endif

/* End ARTFCTSI.HPP *************************************************/
