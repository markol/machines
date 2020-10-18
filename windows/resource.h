////////////////////////////////////////////////////////////////////////////////
//
// Machines (PC) WiredForWar Team 2019-07
//
// Desc: resource.h
// Windows resource file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

/// icon
#define IDI_ICON1 101

/// version
#define VERSION_BUILD_YEAR 20
#define VERSION_BUILD_DATE 210
#define VERSION_BUILD_PREFIX "Build"
#define VERSION_BUILD_SUFFIX "a"

#if (VERSION_BUILD_DATE < 1000)
#define VERSION_STRINGIFY2(_y, _d) #_y ".0" #_d
#else
#define VERSION_STRINGIFY2(_y, _d) #_y "." #_d
#endif  // (VERSION_BUILD_DATE < 1000)
#define VERSION_YEAR_STRINGIFY2(_y) "20" #_y
#define VERSION_STRINGIFY(_y, _d) VERSION_STRINGIFY2(_y, _d)
#define VERSION_YEAR_STRINGIFY(_y) VERSION_YEAR_STRINGIFY2(_y)

#define VERSION_BUILD VERSION_BUILD_YEAR, VERSION_BUILD_DATE
#define VERSION_BUILD_STRING VERSION_STRINGIFY(VERSION_BUILD_YEAR, VERSION_BUILD_DATE)
#define VERSION_YEAR_STRING VERSION_YEAR_STRINGIFY(VERSION_BUILD_YEAR)



#endif  // __RESOURCE_H__
