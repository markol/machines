/*
 * C O N S T R E E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Conserved
 */

/*
    MachLogConstructionTree

    Contains a vector of ConstructionItems in a nice convenient place.
*/

#ifndef _MACHLOG_CONSTREE_HPP
#define _MACHLOG_CONSTREE_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"

class SysPathName;
class MachLogConstructionItem;
class MachLogNotifiable;
class MachLogConstructionTreeImpl;
template < class T > class ctl_pvector;

// Warnings about foward references of template classes
#ifndef CB_NOWARNINGS // Are Charybdis warnings turned on?
#ifndef _MACHLOG_CONSITEM_HPP
#pragma message( __FILE__ "(28): Warning : ctl_pvector< MachLogConstructionItem > foward referenced" )
#endif

#ifndef _MACHLOG_MLNOTIF_HPP
#pragma message( __FILE__ "(32): Warning : ctl_pvector< MachLogNotifiable > foward referenced" )
#endif
#endif

class MachLogConstructionTree
{
public:
	//Useful type definitions

	typedef ctl_pvector< MachLogConstructionItem > ConstructionItems;
	typedef ctl_pvector< MachLogNotifiable > Notifiables;

    MachLogConstructionTree();
	// PRE 	A text file called "global.cst" exists (in the data/ path) with a list of construction items specified in the
	//		following format:
	//
	//		TYPE	(SUBTYPE)	HARDWARE_LEVEL
	//		TYPE	(SUBTYPE)	HARDWARE_LEVEL
	//		...etc
	//
	//		e.g.
	//		BEACON 1
	//		FACTORY CIVILIAN 1
	//		MISSILE_EMPLACEMENT TURRET 2
	//		...etc
	//
	// POST	A global tree of construction items is created, all not activated.
	
    ~MachLogConstructionTree( void );

    void CLASS_INVARIANT;

	ConstructionItems& constructionItems();
	const ConstructionItems& constructionItems() const;
	MachLogConstructionItem& constructionItem( MachPhys::ConstructionType constructionType, int subType, size_t hwLevel, MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );
	
	// note that if any parameter is set or left as DONTCARE, *no* further parameters will be checked even if passed.
	bool activated( MachPhys::Race r, MachLog::ObjectType constructionType, int subType=MachLog::DONTCARE, size_t hwLevel=MachLog::DONTCARE, MachPhys::WeaponCombo wc =MachPhys::N_WEAPON_COMBOS  ) const;
	
	void updateAvailableConstructions( MachPhys::Race r, MachPhys::ConstructionType constructionType, int subType, size_t hwLevel, MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );
	void updateAvailableConstructions( MachPhys::Race r, MachLog::ObjectType constructionType, int subType, size_t hwLevel, MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS );
	// PRE	Tree is already constructed and has at least 1 legal element
	// PRE  Parameters passed provide a match for at least 1 element in the tree
	// POST All MachLogConstructionItem elements up to and including the matched element in the (linear) tree, in the order 
	// 		specified in the global.cst file, will have their 'activated' status set to TRUE.
	
	void addMe( MachLogNotifiable* pNewCTreeNot );
	void removeMe( MachLogNotifiable* pCurrentCTreeNot ); 
	
		
    friend ostream& operator <<( ostream& o, const MachLogConstructionTree& t );

private:
	void readAllItems( const SysPathName& );
	
    // Operations deliberately revoked
    MachLogConstructionTree( const MachLogConstructionTree& );
    MachLogConstructionTree& operator =( const MachLogConstructionTree& );
    bool operator ==( const MachLogConstructionTree& );

	MachLogConstructionTreeImpl* pImpl_;	
};


#endif

/* End CONSTREE.HPP **************************************************/
