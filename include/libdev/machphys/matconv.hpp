/*
 * M A T C O N V . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMaterialConverter

    Convert a material from one basic colour to another
*/

#ifndef _MACHPHYS_MATCONV_HPP
#define _MACHPHYS_MATCONV_HPP

#include "base/base.hpp"

#include "world4d/world4d.hpp"
#include "machphys/machphys.hpp"

class RenMaterial;
class W4dComposite;

class MachPhysMaterialConverter
{
public:
    MachPhysMaterialConverter( MachPhys::Race from, MachPhys::Race to );
    ~MachPhysMaterialConverter( void );

    //  If the material is 'close' to the from colour then convert it
    //  to the to colour and return true. Otherwise return false.
    bool convert( RenMaterial* pMaterial ) const;
	
	// Apply this material conversion to all materials in a given composite.
	void convert( W4dComposite* ) const;
    
    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysMaterialConverter( const MachPhysMaterialConverter& );

    // Operation deliberately revoked
    MachPhysMaterialConverter& operator =( const MachPhysMaterialConverter& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMaterialConverter& );

    double hue( MachPhys::Race race ) const;
    bool close( double hue1, double hue2 ) const;

    //  Data members
    double  toHue_;
    double  fromHue_;
};


#endif

/* End MATCONV.HPP **************************************************/
