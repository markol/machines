#pragma once

#include "afx/AfxConfigurationData.hpp"

class IAfxConfigurationLoader
{
public:
    virtual ~IAfxConfigurationLoader() = 0;

    // It shall not be the loader's responsibility to clean up this object.
    virtual AfxConfigurationData* load() = 0;
};

//////////////////////////////////////////////////////////////

class AfxConfigurationLoader : public IAfxConfigurationLoader
{
public:
    AfxConfigurationLoader();
    virtual ~AfxConfigurationLoader();

    virtual AfxConfigurationData* load() override;
};
