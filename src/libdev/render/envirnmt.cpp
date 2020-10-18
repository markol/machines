/*
 * E N V I R N M T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/envirnmt.hpp"

#include <string>
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"

#include "render/device.hpp"
#include "render/colour.hpp"
#include "render/light.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"

#include "utility/linetok.hpp"

#include "system/pathname.hpp"

static std::string concatRemainingTokens(UtlLineTokeniser& parser, size_t fromToken)
{
	std::string result;
	const UtlLineTokeniser::Tokens& tokens = parser.tokens();

	for (int i=fromToken; i<tokens.size(); ++i)
		result += tokens[i];

	return result;
}

static RenColour parseColour(UtlLineTokeniser& parser, size_t fromToken)
{
	const std::string tmp = concatRemainingTokens(parser, fromToken);
	char* stringCopy = strdup(tmp.c_str());
	std::istringstream istr(stringCopy);

	RenColour colour;
	istr >> colour;
	free(stringCopy);

	return colour;
}

static float parseFloat(UtlLineTokeniser& parser, size_t fromToken)
{
	const std::string tmp = concatRemainingTokens(parser, fromToken);
	char* stringCopy = strdup(tmp.c_str());
	std::istringstream istr(stringCopy);

	float f;
	istr >> f;
	free(stringCopy);

	return f;
}

/*
	fog:			[1|on]
	  start:		1.0
	  end:			300.0
	  density:		0.01
	  colour:		(1.0, 0.92, 0.5)
*/
static void parseFog(UtlLineTokeniser& parser, RenDevice* device)
{
	// The fog: keyword has been read -- look for [on|off].
	const bool on = (parser.tokens()[1] == "on" || parser.tokens()[1] == "1");
	parser.parseNextLine();

	RENDER_STREAM("  Reading fog " << ((on)? "(on)": "(off)") << std::endl);

	if (!on)
	{
		device->fogOff();
	}
	else
	{
		float start=1, end=300, density=0.01;
		RenColour colour;

		// Loop until we hit a non-fog keyword.
		bool parsingFog = true;
		while (parsingFog && !parser.finished())
		{
	        if (parser.tokens()[0] == "start:")
	        {
				start = parseFloat(parser, 1);
				parser.parseNextLine();
	        }
	        else if (parser.tokens()[0] == "end:")
	        {
				end = parseFloat(parser, 1);
				parser.parseNextLine();
	        }
	        else if (parser.tokens()[0] == "density:")
	        {
				density = parseFloat(parser, 1);
				parser.parseNextLine();
	        }
	        else if (parser.tokens()[0] == "colour:")
	        {
				colour = parseColour(parser, 1);
				parser.parseNextLine();
	        }
			else
			{
				// Don't parse the next line -- leave it for something else.
				parsingFog = false;
			}
		}

    	device->fogOn(start, end, density);
		device->fogColour(colour);
	}
}

static RenDirectionalLight* createDirLight
(
	MexDegrees zAngle,
	MexDegrees yAngle,
	const RenColour& colour,
	bool on,
	RenDevice* device,
	ctl_pvector<RenDirectionalLight>* dirs
)
{
	RenDirectionalLight* light = _NEW(RenDirectionalLight);
	device->addLight(light);

	if (dirs)
		dirs->push_back(light);

	const MexEulerAngles lightAngles(zAngle, yAngle, 0);
	MexTransform3d lightXform(lightAngles);
	light->direction(lightXform.xBasis());
	light->colour(colour);

	if (!on)
		light->turnOff();

	RENDER_STREAM("Created " << *light << std::endl);

	return light;
}

static RenPointLight* createPointLight
(
	MATHEX_SCALAR range,
	RenDevice* device,
	ctl_pvector<RenPointLight>* points
)
{
	RenPointLight* light = _NEW(RenPointLight(range));
	device->addLight(light);

	if (points)
		points->push_back(light);

	return light;
}

static RenUniformLight* createUniformLight
(
	MATHEX_SCALAR range,
	RenDevice* device,
	ctl_pvector<RenUniformLight>* uniforms
)
{
	RenUniformLight* light = _NEW(RenUniformLight(range));
	device->addLight(light);

	if (uniforms)
		uniforms->push_back(light);

	return light;
}

/*
directional_light:	[on|off]
  elevation:		47
  azimuth:			225
  colour:			(0.7, 0.7, 0.7)
*/
static void parseDirLight
(
	UtlLineTokeniser& parser,
	RenDevice* device,
	ctl_pvector<RenDirectionalLight>* dirs
)
{
	MexDegrees zAngle = 225, yAngle = 47;
	RenColour colour = RenColour::white();

	// The directional_light: keyword has been read -- look for [on|off].
	const bool on = (parser.tokens()[1] == "on");
	parser.parseNextLine();

	RENDER_STREAM("  Reading directional light " << ((on)? "(on)": "(off)") << std::endl);

	// Loop until we hit a non-light keyword.
	bool parsingLight = true;
	while (parsingLight && !parser.finished())
	{
        if (parser.tokens()[0] == "elevation:")
        {
			yAngle = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "azimuth:")
        {
			zAngle = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "colour:")
        {
			colour = parseColour(parser, 1);
			parser.parseNextLine();
        }
		else
		{
			// Don't parse the next line -- leave it for something else.
			parsingLight = false;
		}
	}

	createDirLight(zAngle, yAngle, colour, on, device, dirs);
}

/*
point_light:	[on|off]
  x:			10
  y:			10
  z:			2
  colour:		(0.7, 0.7, 0.7)
  range:		20
  attenuation:	0.1
*/
/*
static void parsePointLight
(
	UtlLineTokeniser& parser,
	RenDevice* device,
	ctl_pvector<RenPointLight>* points
)
{
	MATHEX_SCALAR x=0, y=0, z=2, range=10, attenuation=0.1;
	RenColour colour = RenColour::white();

	// The point_light: keyword has been read -- look for [on|off].
	bool on = (parser.tokens()[1] == "on");
	parser.parseNextLine();

	RENDER_STREAM("  Reading point light " << ((on)? "(on)": "(off)") << std::endl);

	// Loop until we hit a non-light keyword.
	bool parsingLight = true;
	while (parsingLight && !parser.finished())
	{
        if (parser.tokens()[0] == "x:")
        {
			x = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "y:")
        {
			y = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "z:")
        {
			z = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "range:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp > 0)
				range = tmp;
			else
				RENDER_STREAM("  Read bad point light range " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "attenuation:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp > 0)
				attenuation = tmp;
			else
				RENDER_STREAM("  Read bad point light attenuation " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "colour:")
        {
			colour = parseColour(parser, 1);
			parser.parseNextLine();
        }
		else
		{
			// Don't parse the next line -- leave it for something else.
			parsingLight = false;
		}
	}

	const MexPoint3d pos(x,y,z);
	createPointLight(pos, colour, range, attenuation, on, device, points);
}
*/

/*
uniform_light:	[on|off]
  x:			10
  y:			10
  z:			2
  colour:		(0.7, 0.7, 0.7)
  range:		20
  attenuation:	0.1
*/
static void parseUniformOrPointLight
(
	UtlLineTokeniser& parser,
	bool uniform,
	RenDevice* device,
	ctl_pvector<RenPointLight>* points,
	ctl_pvector<RenUniformLight>* uniforms
)
{
	const char* type = (uniform)? "uniform light": "point light";
	MATHEX_SCALAR x=0, y=0, z=2, range=10, constant=0.1, linear=0.1, quadratic=0.1;
	RenColour colour = RenColour::white();

	// The uniform_light: keyword has been read -- look for [on|off].
	bool on = (parser.tokens()[1] == "on");
	parser.parseNextLine();

	RENDER_STREAM("  Reading " << type << ((on)? " (on)": " (off)") << std::endl);

	// Loop until we hit a non-light keyword.
	bool parsingLight = true;
	while (parsingLight && !parser.finished())
	{
        if (parser.tokens()[0] == "x:")
        {
			x = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "y:")
        {
			y = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "z:")
        {
			z = parseFloat(parser, 1);
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "range:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp > 0)
				range = tmp;
			else
				RENDER_STREAM("  Read bad " << type << " range " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "constant_attenuation:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp >= 0)
				constant = tmp;
			else
				RENDER_STREAM("  Read bad " << type << " attenuation " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "linear_attenuation:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp >= 0)
				linear = tmp;
			else
				RENDER_STREAM("  Read bad " << type << " attenuation " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "quadratic_attenuation:")
        {
			const MATHEX_SCALAR tmp = parseFloat(parser, 1);
			parser.parseNextLine();

			if (tmp >= 0)
				quadratic = tmp;
			else
				RENDER_STREAM("  Read bad " << type << " attenuation " << tmp << " from " << parser.fileName() << std::endl);
        }
        else if (parser.tokens()[0] == "colour:")
        {
			colour = parseColour(parser, 1);
			parser.parseNextLine();
        }
		else
		{
			// Don't parse the next line -- leave it for something else.
			parsingLight = false;
		}
	}

	const MexPoint3d pos(x,y,z);
	RenAttenuatedLight* light;

	if (uniform)
		light = createUniformLight(range, device, uniforms);
	else
		light = createPointLight(range, device, points);

	ASSERT(light, "Failed to create point or uniform light.");

	light->position(pos);
	light->colour(colour);
	light->constantAttenuation(constant);
	light->linearAttenuation(linear);
	light->quadraticAttenuation(quadratic);

	if (!on)
		light->turnOff();

	RENDER_STREAM("Created " << *light << std::endl);
}

void renReadEnvironment
(
	const SysPathName& givenFile,
	RenDevice* device,
	ctl_pvector<RenDirectionalLight>*	dirs,
	ctl_pvector<RenPointLight>*			points,
	ctl_pvector<RenUniformLight>*		uniforms
)
{
	PRE(device);

	RENDER_STREAM("Attempting to read environment file " << givenFile << std::endl);

	SysPathName envFile = givenFile;
	if (!envFile.existsAsFile())
	{
		envFile = "default.env";
		RENDER_STREAM("  " << givenFile << " doesn't exist: switching to " << envFile << std::endl);
	}

	// If there is no file, pick sensible default values for everything.
	// We go with the library's default ambient and light colour.
	if (!envFile.existsAsFile())
	{
		RENDER_STREAM("  " << envFile << " doesn't exist: setting defaults." << std::endl);

		createDirLight(225, 47, RenColour::white(), true, device, dirs);

		RenColour skyCol(0.29, 0.51, 0.87), ambient(0.4), lightCol(0.6);
	    device->backgroundColour(skyCol);
		return;
	}

    UtlLineTokeniser parser(envFile);
    while (!parser.finished())
    {
        if (parser.tokens()[0] == "sky_colour:")
        {
		    device->backgroundColour(parseColour(parser, 1));
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "ambient:")
        {
		    device->ambient(parseColour(parser, 1));
			parser.parseNextLine();
        }
        else if (parser.tokens()[0] == "fog:")
        {
			parseFog(parser, device);
        }
        else if (parser.tokens()[0] == "directional_light:")
        {
			parseDirLight(parser, device, dirs);
        }
        else if (parser.tokens()[0] == "point_light:")
        {
			parseUniformOrPointLight(parser, false, device, points, uniforms);
        }
        else if (parser.tokens()[0] == "uniform_light:")
        {
			parseUniformOrPointLight(parser, true, device, points, uniforms);
        }
		else
		{
			RENDER_STREAM("  Read bad input keyword " << parser.tokens()[0] << " from " << parser.fileName() << std::endl);
			parser.parseNextLine();
		}
    }
}

/* End ENVIRNMT.CPP ****************************************************/
