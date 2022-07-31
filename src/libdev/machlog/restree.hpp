/*
 * R E S T R E E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogResearchTree

    Contains a vector of ResearchItems in a nice convinient place.
*/

#ifndef _MACHLOG_RESTREE_HPP
#define _MACHLOG_RESTREE_HPP

#include "base/base.hpp"
#include "machlog/machine.hpp"
#include "machlog/machlog.hpp"

class SysPathName;
class MachLogResearchItem;
class MachLogNotifiable;
class MachLogResearchTreeImpl;
template < class T > class ctl_pvector;

class MachLogResearchTree
{
public:
	typedef ctl_pvector< MachLogResearchItem > ResearchItems;
	typedef ctl_pvector< MachLogNotifiable > Notifiables;

    MachLogResearchTree();
    ~MachLogResearchTree();

    void CLASS_INVARIANT;

	ResearchItems& researchItems();
	const ResearchItems& researchItems() const;
	MachLogResearchItem& researchItem( MachLog::ObjectType, int subType, MachLogMachine::Level hwLevel = 0, MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );

	// note that if any parameter is set or left as DONTCARE, *no* further parameters will be checked even if passed.
	bool activated( MachPhys::Race r, MachLog::ObjectType objectType, int subType=MachLog::DONTCARE, size_t hwLevel=MachLog::DONTCARE, MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );
	void updated( MachPhys::Race r );
	
	void addMe( MachLogNotifiable* pNewRTreeNot );
	void removeMe( MachLogNotifiable* pCurrentRTreeNot );	

    friend ostream& operator <<( ostream& o, const MachLogResearchTree& t );

private:
	void readAllItems( const SysPathName& );
    // Operation deliberately revoked
    MachLogResearchTree( const MachLogResearchTree& );
    MachLogResearchTree& operator =( const MachLogResearchTree& );
    bool operator ==( const MachLogResearchTree& );

	MachLogResearchTreeImpl* pImpl_;
};


#endif

/* End RESTREE.HPP **************************************************/
