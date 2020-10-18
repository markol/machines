/***********************************************************

  S K Y D E C L . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifndef _ENVIRNMT_INTERNAL_SKY_DECLARATION_HPP
#define _ENVIRNMT_INTERNAL_SKY_DECLARATION_HPP

/*
  During the parsing of a .env file this class helps by ensuring that only
  one type of sky is being constructed and that its' members are correct.

  Each type of sky is represented by a nested class of EnvISkyDeclaration.

  Single sky declaration assurance is asserted in the create methods.
*/

#include "base/base.hpp"
#include "stdlib/string.hpp"

class MexDegrees;

class RenColour;

class EnvSky;
class EnvUniformSky;
class EnvStaticSky;
class EnvDynamicSky;

class EnvISkyDeclaration
{
// Cannonical form revoked.
public:
	class UniformSky
	{
	public:
		void controller(const string& name);
		void colourTable(const string& name);

		void completeSky();

		friend class EnvISkyDeclaration;

	private:
		UniformSky(EnvUniformSky* const pSky);

		// Operations deliberatly revoked.
		UniformSky(const UniformSky& copyMe);
		UniformSky& operator =(const UniformSky& assignMe);

		bool controllerSet_;
		bool colourTableSet_;
		EnvUniformSky* const pSky_;
	};

	class StaticSky
	{
	public:
		void meshFile(const string& pathname);
		void backgroundColour(const RenColour&);

		void completeSky();

		friend class EnvISkyDeclaration;

	private:
		StaticSky(EnvStaticSky* const pSky);

		// Operations deliberatly revoked.
		StaticSky(const StaticSky& copyMe);
		StaticSky& operator =(const StaticSky& assignMe);

		bool meshFileSet_;
		bool backgroundColourSet_;
		EnvStaticSky* const pSky_;
	};

	class DynamicSky
	{
	public:
		void meshFile(const string& pathname);
		void controller(const string& name);
		void colourTable(MexDegrees at, const string& clutName);

		void completeSky();

		friend class EnvISkyDeclaration;

	private:
		DynamicSky(EnvDynamicSky* const pSky);

		// Operations deliberatly revoked.
		DynamicSky(const DynamicSky& copyMe);
		DynamicSky& operator =(const DynamicSky& assignMe);

		bool meshFileSet_;
		bool controllerSet_;
		bool atLeastOneColourTableSet_;
		EnvDynamicSky* const pSky_;
	};

	EnvISkyDeclaration();
	~EnvISkyDeclaration();

	void createUniformSkyDeclaration(EnvUniformSky* const pSky);
	// PRE(isClear());
	// POST(isUniformSkyDeclaration());
	void createStaticSkyDeclaration(EnvStaticSky* const pSky);
	// PRE(isClear());
	// POST(isStaticSkyDeclaration());
	void createDynamicSkyDeclaration(EnvDynamicSky* const pSky);
	// PRE(isClear());
	// POST(isDynamicSkyDeclaration());

	// Allow another declaration to be processed.
	void clear();
	// PRE(isClear() or isComplete());

	const UniformSky& uniformSky() const;
	// PRE(isUniformSkyDeclaration());
	UniformSky& uniformSky();
	// PRE(isUniformSkyDeclaration());

	const StaticSky& staticSky() const;
	// PRE(isStaticSkyDeclaration());
	StaticSky& staticSky();
	// PRE(isStaticSkyDeclaration());

	const DynamicSky& dynamicSky() const;
	// PRE(isDynamicSkyDeclaration());
	DynamicSky& dynamicSky();
	// PRE(isDynamicSkyDeclaration());

	bool isUniformSkyDeclaration() const;
	// PRE(not isClear());
	bool isStaticSkyDeclaration() const;
	// PRE(not isClear());
	bool isDynamicSkyDeclaration() const;
	// PRE(not isClear());
	bool isClear() const;
	bool isBuilding() const;
	bool isComplete() const;

	// Polymorphic pointer to the sky object passed to the create method.
	EnvSky*	completedSky() const;
	// PRE(isComplete());

	// These methods call the completeSky() member of this class.
	friend void UniformSky::completeSky();
	friend void StaticSky::completeSky();
	friend void DynamicSky::completeSky();

	void CLASS_INVARIANT;

private:
	// Operations deliberatly revoked.
	EnvISkyDeclaration(const EnvISkyDeclaration&);
	EnvISkyDeclaration& operator =(const EnvISkyDeclaration&);
	bool operator ==(const EnvISkyDeclaration&);

	// Called by the completeSky methods of the nested classes.
	void completeSky(EnvSky* const pSky);

	EnvSky* pCompletedSky_;
	UniformSky* pUniformSky_;
	StaticSky* pStaticSky_;
	DynamicSky* pDynamicSky_;
};

#endif /* _ENVIRNMT_INTERNAL_SKY_DECLARATION_HPP **********/

