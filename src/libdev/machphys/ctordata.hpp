/*
 * C T O R D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstructorData

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_CTORDATA_HPP
#define _MACHPHYS_CTORDATA_HPP

#include "base/base.hpp"
#include "machphys/machdata.hpp"

class MachPhysConstructorData : public MachPhysMachineData
{
public:
	MachPhysConstructorData();
    ~MachPhysConstructorData();

	//get methods public
	MachPhys::SizeUnits capacity() const;
	MachPhys::BuildingMaterialUnits constructionRate() const;
	MachPhys::ConstructionType		mostAdvancedConstructionType() const;
	int								mostAdvancedSubType() const;
	size_t							mostAdvancedConstructionLevel() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysConstructorData& t );

private:
	// Friend classes allowed to use private set functions
	friend class MachPhysDataParser;
	//friend class MachLogScenario;

    // Operation deliberately revoked
    MachPhysConstructorData( const MachPhysConstructorData& );

    // Operation deliberately revoked
    MachPhysConstructorData& operator =( const MachPhysConstructorData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysConstructorData& );

    //MachPhysConstructorData();

	//set methods private
	void capacity( const MachPhys::SizeUnits& );
	void constructionRate( const MachPhys::BuildingMaterialUnits& );

	void mostAdvancedConstructionType( MachPhys::ConstructionType );
	void mostAdvancedSubType( int );
	void mostAdvancedConstructionLevel( size_t );

	MachPhys::SizeUnits				capacity_;
	MachPhys::BuildingMaterialUnits	constructionRate_;


	// Data specifying unique entry point into construction tree
	MachPhys::ConstructionType		mostAdvancedConstructionType_;
	int								mostAdvancedSubType_;
	size_t							mostAdvancedConstructionLevel_;
	//wcombo??

};


#endif

/* End CTORDATA.HPP *************************************************/
