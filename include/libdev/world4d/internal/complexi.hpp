/*
 * C O M P L E X I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dComplexity

    A brief description of the class should go in here
*/

#ifndef _COMPLEXI_HPP
#define _COMPLEXI_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "ctl/vector.hpp"

class W4dComplexity
// Canonical form revoked
{
public:
	// defines the type of schem used for incrementing/decrementing the complexity of the scene
	enum SequencingType	{ BY_LOD, BY_PRIORITY };

    W4dComplexity( uint nPriorities = 2 );
    ~W4dComplexity();

    void CLASS_INVARIANT;

	// increase/decrease of one step the complexity of the scene
	bool increase();
	bool decrease();

	bool enabled() const { return enabled_; }
	void enabled( bool setEnabled );
	bool enabledLOD() const { return enabledLOD_; }
	void enabledLOD( bool setEnabledLOD );

	void reset();

	// true if we can no increase/decrease more the complexity of the scene
	bool highestComplexityReached() const;
	bool lowestComplexityReached() const;

	uint nPriorities() const;

	const MATHEX_SCALAR& range( ) const;
	void range( const MATHEX_SCALAR& );

	void usePriority( uint priority );
	// The following methods use the priority set by usePriority() to
	// the modified range of an entity for purpose of frame rate management
	const MATHEX_SCALAR& modifiedRange( ) const;
	const int& level( ) const;
	const MATHEX_SCALAR& rangeMultiplier( ) const;


private:
	class W4dRangeModifier
	{
	public:
		W4dRangeModifier();
		~W4dRangeModifier();
		void resetLevel();
		void increaseLevel();
		void decreaseLevel();
		const MATHEX_SCALAR& rangeMultiplier() const;
		const int& level() const { return level_; }
		const int& minLevel() const { return minLevel_; }
		const int& maxLevel() const { return maxLevel_; }

	private:
		void updateRangeMultiplier();

		MATHEX_SCALAR	factor_;
		int	minLevel_;
		int	maxLevel_;
		int level_;
		bool rangeMultiplierUptodate_;
		MATHEX_SCALAR rangeMultiplier_;
	};

	typedef ctl_vector< W4dRangeModifier > W4dRangeModifiers;

    friend ostream& operator <<( ostream& o, const W4dComplexity& t );

    W4dComplexity( const W4dComplexity& );
    W4dComplexity& operator =( const W4dComplexity& );

	void updateModifiedRange();

	// All the priority are increased/decreasded to the same LOD,
	// then the LOD is increased/decreased
	bool increaseComplexityByLOD();
	bool decreaseComplexityByLOD();

	void updateHighestComplexityReached();
	void updateLowestComplexityReached();

	SequencingType			sequencingType_;

	// list of lod range modifiers (one for each priority level)
	W4dRangeModifiers		rangeModifiers_;
	// cycle through the priorities during the increase/decrease calls
	uint					priorityCycler_;

	// which element of element of rangeModifier_ is actually used
	uint					priority_;

	bool					lowestComplexityReached_;
	bool					highestComplexityReached_;

	bool					lowestComplexityReachedUptodate_;
	bool					highestComplexityReachedUptodate_;

	// The range of the actual entity being rendered
	MATHEX_SCALAR			range_;
	MATHEX_SCALAR			modifiedRange_;

	bool 					enabled_;
	bool 					enabledLOD_;
};


#endif

/* End COMPLEXI.HPP *************************************************/
