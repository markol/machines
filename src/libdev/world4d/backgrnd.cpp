/*
 * B A C K G R N D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/backgrnd.hpp"

#include "world4d/generic.hpp"
#include "system/pathname.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/angle.hpp"

// This auxilliary function reads a tree of generic entities to use as
// backgound objects.  The objects fall into three animation categories:
//   1) no animation
//   2) animated as celestial bodies (rotating around the sky)
//   3) animated as clouds (translated around the sky)
// Although this function doesn't define the animations, it is assumed
// that the caller will create appropriate animations for the differnt
// categories.  The caller must create the top-level root, but not the
// object which represents the primary light source (the sun).  It may
// optionally create the cloud and celestial roots.  If these parameters
// are null, then the corresponding objects will just be skipped.
bool W4dReadBackground
(
	const SysPathName& pathname,
	W4dEntity*  root,			// the root for unanimated objects
	W4dEntity** primaryLight,	// an object representing the main light source
	W4dEntity*  cloudRoot,		// root for objects animated as clouds
	W4dEntity*  celestialRoot,	// root for objects animated as suns, moons, etc.
	MATHEX_SCALAR* cloudSpeed,	// max translation speed of clouds per sec
	MexDegrees* rotSpeed		// max rotation speed of celestials per sec
)
{
	PRE(root);
	PRE(primaryLight && !(*primaryLight));

	std::ifstream ifs(pathname.pathname().c_str());
	std::istream& is = ifs;

	if (!is)
	{
		std::cout << "Couldn't read file " << pathname << std::endl;
		return false;
	}

	// Initially, new objects are added to the unanimated root.
	W4dEntity* addTo = root;

	bool readingSun = false;
	while (is)
	{
		string tmp;
		is >> tmp;

		if (tmp == "clouds")
		{
			addTo = cloudRoot;
			is >> *cloudSpeed;
		}
		else if (tmp == "celestial")
		{
			addTo = celestialRoot;
			MATHEX_SCALAR degrees;
			is >> degrees;
			*rotSpeed = degrees;
		}
		else if (tmp == "sun")
		{
			readingSun = true;
		}
		else if (tmp.length() > 0)
		{
			// If the string isn't either of those keywords, treat it as a filename.
			SysPathName meshFile = tmp;

			if (!meshFile.existsAsFile())
			{
				std::cout << "Couldn't read file " << meshFile << std::endl;
				return false;
			}
			else
			{
				// Some file types require a model name.
				string modelName;
				if (!meshFile.hasExtension() || meshFile.extension() == "x")
					is >> modelName;

				MATHEX_SCALAR x,y,z, az, el, scale;
				is >> x >> y >> z >> az >> el >> scale;

				// Only try to read the model if addTo has been set.  It may be
				// null if cloudRoot or celestialRoot is null.
				ASSERT(implies(!addTo, !cloudRoot || !celestialRoot), "Backgound parsing logic error.");
				if (addTo)
				{
					MexEulerAngles angles(MexDegrees(az), MexDegrees(el), 0);
					MexTransform3d xform(angles, MexPoint3d(x,y,z));

					W4dGeneric* gen = NULL;
					if (!meshFile.hasExtension() || meshFile.extension() == "x")
					{
						ASSERT(modelName.length() != 0, "No model name specified");
						gen = _NEW(W4dGeneric(addTo, xform));
						gen->loadSingleMesh(meshFile, modelName, scale);
					}
					else if (meshFile.extension() == "lod")
					{
						gen = _NEW(W4dGeneric(addTo, xform));
						gen->loadLODFile(meshFile);
					}
					else
					{
						std::cout << "Unsupported background extension " << meshFile.extension() << std::endl;
						return false;
					}

					ASSERT(gen, logic_error("Failed to read background object"));

					// If the sun keyword is encountered, assign the current object
					// to the primary light pointer and then cancel the keyword.
					if (readingSun)
					{
						if (*primaryLight)
						{
							std::cout << "Encountered two \"sun\" keywords in background file." << std::endl;
							return false;
						}

						*primaryLight  = gen;
						readingSun = false;
					}
				}
			}
		}
	}

	return true;
}

/* End BACKGRND.CPP *************************************************/
