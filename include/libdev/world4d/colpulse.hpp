/*
 * C O L P U L S E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dColourPulsePlan

    Defines a material plan in which the diffuse ( emissive id defined ) colour pulses for a period of time. 
*/

#ifndef _WORLD4D_COLPULSE_HPP
#define _WORLD4D_COLPULSE_HPP

#include "base/base.hpp"
#include "world4d/matplan.hpp"
#include "world4d/coldata.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "phys/phys.hpp"

//forward refs
class RenMaterialVec;
class RenColour;
class RenTexture;
class W4dEntity;

class W4dColourPulsePlan : public W4dMaterialPlan
// Canonical form revoked
{
public:
	//only the textures listed in dataVec will be pulsed
	typedef ctl_pvector< W4dColourPulseData > ColourPulseDataVec;
    W4dColourPulsePlan( const ColourPulseDataVec& dataVec, const RenMaterialVec& matVec);

	//no texture key is needed with this constructor. all the materials will be pulsed.
    W4dColourPulsePlan( const RenMaterialVec& matVec, const RenColour& fromColour, const RenColour& toColour, const PhysRelativeTime& duration );
    
    //dtor
    virtual ~W4dColourPulsePlan();


    ///////////////////////////////////////////////////
    //Inherited from W4dMaterialPlan

    //True if the plan is defined for level of detail lodId.
    virtual bool isLODDefined( W4dLOD lodId ) const;

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
    virtual const Ren::MaterialVecPtr& materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD lodId ) const;
    //PRE( isLODDefined( lodId ) );
    ///////////////////////////////////////////////////

	//makes a W4dColourPulsePlan from dataVec, then apply to entity from startTime
	static void makePlan( W4dEntity* pEntity, const ColourPulseDataVec& dataVec, const PhysAbsoluteTime& startTime, uint nRepetations = 0, uint animId = 0 );  

	//pulse the colour of all the materials of pEntity
	static void changeAllColour( W4dEntity* pEntity, 
                          const RenColour& fromColour, 
                          const RenColour& toColour, 
                          const PhysRelativeTime& period,
                          const PhysAbsoluteTime& startTime, 
                          uint nPeriods, uint animId );

	//find the material in the MatVec of pEntity which uses texture, then change its colour to color, return true if found
	static bool changeColour( W4dEntity* pEntity, const RenTexture& texture, const RenColour& colour );
	  
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dColourPulsePlan& t );

private:
    W4dColourPulsePlan( const W4dColourPulsePlan& );
    W4dColourPulsePlan& operator =( const W4dColourPulsePlan& );
    bool operator ==( const W4dColourPulsePlan& );

	RenColour pulsingColour( const PhysRelativeTime timeOffset, size_t iPosition ) const;

	ColourPulseDataVec colourPulseDataVec_;
    Ren::MaterialVecPtr materialVecPtr_; 
    ctl_vector< size_t > pulsingTexturePositions_; 
};


#endif

/* End COLPULSE.HPP *************************************************/
