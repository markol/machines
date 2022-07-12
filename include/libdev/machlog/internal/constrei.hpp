/*
 * C O N S T R E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogConstructionTreeImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CONSTREI_HPP
#define _MACHLOG_CONSTREI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/constree.hpp"

class MachLogConstructionTreeImpl
// Canonical form revoked
{
public:
    MachLogConstructionTreeImpl();
    ~MachLogConstructionTreeImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogConstructionTreeImpl& t );

    MachLogConstructionTreeImpl( const MachLogConstructionTreeImpl& );
    MachLogConstructionTreeImpl& operator =( const MachLogConstructionTreeImpl& );

	friend class MachLogConstructionTree;

	MachLogConstructionTree::ConstructionItems	constructionItems_;
	MachLogConstructionTree::Notifiables		notifiables_;
	size_t 										nNotifiables_;
};

#define CB_MachLogConstructionTree_DEPIMPL() \
	CB_DEPIMPL( MachLogConstructionTree::ConstructionItems, constructionItems_ ); \
	CB_DEPIMPL( MachLogConstructionTree::Notifiables, notifiables_ ); \
	CB_DEPIMPL( size_t, nNotifiables_ );

#endif

/* End CONSTREI.HPP *************************************************/
