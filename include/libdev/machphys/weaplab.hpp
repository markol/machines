/*
 * W E A P L A B . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysWeaponsLab

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_WEAPLAB_HPP
#define _MACHPHYS_WEAPLAB_HPP

#include "base/base.hpp"

#include "machphys/constron.hpp"
#include "machphys/ofactory.hpp"

class MachPhysWeaponsLabData;

class MachPhysWeaponsLab : public MachPhysConstruction
{
public:
    MachPhysWeaponsLab(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysWeaponsLab( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysWeaponsLabData& data() const;

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysWeaponsLab( const MachPhysWeaponsLab& );

    // Operation deliberately revoked
    MachPhysWeaponsLab& operator =( const MachPhysWeaponsLab& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysWeaponsLab& );

    typedef size_t  Id;
    typedef MachPhysObjectFactory< Id, MachPhysWeaponsLab >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysWeaponsLab& Factory::part( const ID&, size_t );

    static  MachPhysWeaponsLab& factory( size_t level );
    SysPathName compositeFileName( size_t level ) const;
    SysPathName interiorCompositeFileName( size_t level ) const;
    void initialiseLevel1();

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a software lab from scratch

    MachPhysWeaponsLab( W4dEntity* pParent, Id bodyLevel );

    MachPhysWeaponsLabData* pData_;
};


#endif

/* End WEAPLAB.HPP *****************************************************/
