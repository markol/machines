set(AFX_SOURCES
    AfxConfiguration.cpp
    AfxConfigurationData.hpp
    AfxConfiguration.hpp
    AfxConfigurationLoader.cpp
    AfxConfigurationLoader.hpp
    AfxSdlApp.hpp
    app.cpp
    app.hpp
    castfns.hpp
    dosapp.hpp
    invkargs.cpp
    invkargs.hpp
    osapp.hpp
    printmsg.hpp
    resource.cpp
    resource.hpp
    singapp.hpp
    slimwin.hpp
    win95app.hpp
)

# afx_lite is used with test suite
add_library(afx_lite OBJECT ${AFX_SOURCES})
add_library(afx OBJECT ${AFX_SOURCES} AfxSdlApp.cpp)
