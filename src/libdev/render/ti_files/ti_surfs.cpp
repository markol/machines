#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/map.hpp"
#include <string.hpp>
#include "render/render.hpp"

void dummyFunctionNameMap()
{				
  static ctl_map< string, Ren::TexId, less<string> > dummyBiteMap;
}
