/*
 * R E S T R E E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogResearchTreeImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_RESTREEI_HPP
#define _MACHLOG_RESTREEI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/restree.hpp"

class MachLogResearchTreeImpl
// Canonical form revoked
{
public:
    MachLogResearchTreeImpl();
    ~MachLogResearchTreeImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogResearchTreeImpl& t );

    MachLogResearchTreeImpl( const MachLogResearchTreeImpl& );
    MachLogResearchTreeImpl& operator =( const MachLogResearchTreeImpl& );

	friend class MachLogResearchTree;

	MachLogResearchTree::ResearchItems	researchItems_;
	MachLogResearchTree::Notifiables	notifiables_;
	size_t 								nNotifiables_;
};


#define CB_MachLogResearchTree_DEPIMPL() \
	CB_DEPIMPL( MachLogResearchTree::ResearchItems, researchItems_ ); \
	CB_DEPIMPL( MachLogResearchTree::Notifiables, notifiables_ ); \
	CB_DEPIMPL( size_t, nNotifiables_ ); 

#endif

/* End RESTREEI.HPP *************************************************/
