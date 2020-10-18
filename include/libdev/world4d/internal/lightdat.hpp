/*
 * L I G H T D A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dLightData

    defines the UVAnimationdata specified in a LOD/CDF file
*/

#ifndef _WORLD4D_LIGHTDAT_HPP
#define _WORLD4D_LIGHTDAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "world4d/internal/animdata.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "render/colour.hpp"
#include "ctl/vector.hpp"

class W4dEntity;
class W4dComposite;

class W4dLightData: public  W4dAnimationData
// Canonical form revoked
{
public:
    W4dLightData( const string& DummyMeshName, W4dLOD maxLod );
    virtual ~W4dLightData( void );

	enum Scope : unsigned char {LOCAL_LIGHT, DOMAIN_LIGHT, DOMAIN_COMPOSITE, GLOBAL_LIGHT, DYNAMIC_LIGHT};
	enum Type : unsigned char {POINT, UNIFORM, DIRECTIONAL};

	void scope( const Scope& scope );
	void type( const Type& type );
	void colour( const RenColour& colour );
	void position( const MexPoint3d& position );
	void direction( const MexVec3& direction );
	void attenuations( MATHEX_SCALAR a1, MATHEX_SCALAR a2, MATHEX_SCALAR a3  );
	void maxRange( MATHEX_SCALAR maxRange );
	void composite( W4dComposite* pComposite);
	void times( const ctl_vector< MATHEX_SCALAR >& times );
	void intensities( const ctl_vector< MATHEX_SCALAR >& intensities );

	const Type& type() const;

	//apply the uv animation to pEntity	 start at statTime
	virtual void apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime);

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dLightData);
    PER_FRIEND_READ_WRITE(W4dLightData);

private:
    friend ostream& operator <<( ostream& o, const W4dLightData& t );

    W4dLightData( const W4dLightData& );
    W4dLightData& operator =( const W4dLightData& );

	Type type_;
	Scope scope_;
	RenColour colour_;
	MexPoint3d position_;
	MexVec3 direction_;
	MATHEX_SCALAR attenuations_[3];
	MATHEX_SCALAR maxRange_;
	W4dComposite* pComposite_;
	ctl_vector< MATHEX_SCALAR > times_;
	ctl_vector< MATHEX_SCALAR > intensities_;
};

PER_DECLARE_PERSISTENT(W4dLightData);
PER_ENUM_PERSISTENT(W4dLightData::Type);
PER_ENUM_PERSISTENT(W4dLightData::Scope);
#endif

/* End LIGHTDAT.HPP ***************************************************/
