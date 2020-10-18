/*
 * C O L O D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/colodata.hpp"

#include "stdlib/string.hpp"
#include "render/texture.hpp"
#include "system/pathname.hpp"
#include "render/texmgr.hpp"

PER_DEFINE_PERSISTENT(W4dColourPlanData);


W4dColourPlanData::W4dColourPlanData(const ScalarVec& times, const ColourVec& colours):
 times_(times),
 colours_(colours)
{
    TEST_INVARIANT;
}

W4dColourPlanData::W4dColourPlanData( const W4dColourPlanData& copyMe):
times_(copyMe.times_),
colours_(copyMe.colours_)
{
    TEST_INVARIANT;
}


W4dColourPlanData::~W4dColourPlanData()
{
    TEST_INVARIANT;

}

void W4dColourPlanData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	INVARIANT(colours_.size() - times_.size() == 1);

}

const PhysRelativeTime& W4dColourPlanData::duration() const
{
	return times_[times_.size()-1];
}


const W4dColourPlanData::ScalarVec& W4dColourPlanData::times() const
{
	return times_;
}

const W4dColourPlanData::ColourVec& W4dColourPlanData::colours() const
{
	return colours_;
}

void  W4dColourPlanData::times(const ScalarVec& times)
{
	times_ = times;
}

void  W4dColourPlanData::colours(const ColourVec& colours)
{
	colours_ = colours;
}


RenColour W4dColourPlanData::colour(const PhysRelativeTime& timeOffset) const
{
	PRE( timeOffset <= times_[times_.size() -1] );

	size_t i = 0;
	RenColour colour;
	PhysRelativeTime t1 = 0;
	bool done = false;

	while(  (i < times_.size()) and not done )
	{
		PhysRelativeTime t2 = times_[i];

		if( (t1 <= timeOffset ) && ( timeOffset <= t2) )
		{
			MATHEX_SCALAR alphared = (colours_[i+1].r() - colours_[i].r())/(t2 - t1);
			colour.r( colours_[i].r() + alphared * (timeOffset - t1));

			MATHEX_SCALAR alphagreen = (colours_[i+1].g() - colours_[i].g())/(t2 - t1);
			colour.g( colours_[i].g() + alphagreen * (timeOffset - t1));

			MATHEX_SCALAR alphablue = (colours_[i+1].b() - colours_[i].b())/(t2 - t1);
			colour.b( colours_[i].b() + alphablue * (timeOffset - t1));

			done = true;
		}

		t1 = t2;
		++i;
	}


	return colour;
}

ostream& operator <<( ostream& o, const W4dColourPlanData& t )
{

    o << "W4dColourPlanData " << (void*)&t << " start" << std::endl;
    o << "W4dColourPlanData " << (void*)&t << " end" << std::endl;

    return o;
}

W4dColourPlanData::W4dColourPlanData( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const W4dColourPlanData& plan )
{
    ostr << plan.times_;
    ostr << plan.colours_;
}

void perRead( PerIstream& istr, W4dColourPlanData& plan )
{
    istr >> plan.times_;
    istr >> plan.colours_;
}

/* End COLODATA.CPP **************************************************/
