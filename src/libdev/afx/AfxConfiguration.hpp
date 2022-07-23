#pragma once

#include "afx/AfxConfigurationData.hpp"
#include "afx/AfxConfigurationLoader.hpp"

class AfxConfiguration
{
public:
    AfxConfiguration();
    explicit AfxConfiguration(IAfxConfigurationLoader* loader);
    ~AfxConfiguration();

    const AfxConfigurationData& getConfig();

private:
    void validateAntiAliasingSettings(AfxConfigurationData& config);

    AfxConfigurationLoader   defaultLoader_;
    IAfxConfigurationLoader* pLoader_;

    bool                     configValidated_;
    AfxConfigurationData*    pConfigData_;
};
