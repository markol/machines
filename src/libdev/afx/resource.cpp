/*
 * R E S O U R C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "afx/resource.hpp"

#include <limits.h>
#include "system/pathname.hpp"
#include "utility/rapidxml_utils.hpp"

// This function is only used in the debug version.
#ifndef NDEBUG
static char* formatMsg()
{
	static char msgBuf[] = "test";
	return msgBuf;
}
#endif

AfxResourceLib::AfxResourceLib(const SysPathName& libFile):
	fileName_(libFile)
{
	PRE(libFile.existsAsFile());

	rapidxml::file<> xmlFile(libFile.c_str()); // Default template is char
    rapidxml::xml_document<> doc;
    try
    {
        doc.parse<0>(xmlFile.data());
    }
    catch( const rapidxml::parse_error & e )
    {
        std::cerr << e.what() << " here: " << e.where < char >() << std::endl;
    }

    rapidxml::xml_node <>* node = doc.first_node();

    for( rapidxml::xml_node <>* a = node->first_node(); a; a = a->next_sibling() )
    {
        resourceStrings_.insert(std::make_pair(atoi(a->first_attribute()->value()), a->value()));
    }

	TEST_INVARIANT;
}

AfxResourceLib::~AfxResourceLib()
{
    TEST_INVARIANT;

}

std::string AfxResourceLib::getString(const uint id) const
{
	if(resourceStrings_.count(id) > 0)
        return resourceStrings_.at(id);
    else
        return "";
}

ulong AfxResourceLib::bitmapHandle(const std::string& name) const
{
	return 0;
}

const SysPathName& AfxResourceLib::fileName() const
{
	POST(fileName_.existsAsFile());
	return fileName_;
}

void AfxResourceLib::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const AfxResourceLib& t )
{

    o << "AfxResourceLib " << (void*)&t << " start" << std::endl;
    o << "AfxResourceLib " << (void*)&t << " end" << std::endl;

    return o;
}

/* End RESOURCE.CPP *************************************************/
