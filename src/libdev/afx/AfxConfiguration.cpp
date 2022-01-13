#include <cmath>
#include "afx/AfxConfiguration.hpp"

AfxConfiguration::AfxConfiguration()
    : AfxConfiguration(&defaultLoader_)
{
}

AfxConfiguration::AfxConfiguration(IAfxConfigurationLoader* loader)
{
    pLoader_ = loader;
    configValidated_ = false;
    pConfigData_ = nullptr;
}

AfxConfiguration::~AfxConfiguration()
{
    if (pConfigData_ != nullptr)
    {
        delete pConfigData_;
    }
}

const AfxConfigurationData& AfxConfiguration::getConfig()
{
    if (pConfigData_ == nullptr)
    {
        pConfigData_ = pLoader_->load();
    }

    if (not configValidated_)
    {
        // Add validations here:
        validateAntiAliasingSettings(*pConfigData_);

        configValidated_ = true;
    }

    return *pConfigData_;
}

void AfxConfiguration::validateAntiAliasingSettings(AfxConfigurationData& config)
{
    // Validate buffer count
    config.multisampleBuffers = (config.multisampleBuffers <= 0) ? 1 : config.multisampleBuffers;

    // Validate samples, correcting if necessary
    // Must be 2, 4, 8, or 16
    const int sampleRate = config.multisampleSamples;
    if (sampleRate <= 0)
    {
        config.multisampleSamples = 2;
    }
    else if (sampleRate > 16)
    {
        config.multisampleSamples = 16;
    }
    else
    {
        // Put a number like 3, 15, etc.? NO PROBLEM!!
        const auto        y = static_cast<double>(sampleRate);
        auto       powerOf2 = std::floor(std::log2(y));

        // 2^1 minimum value
        if (powerOf2 < 1.0)
        {
            powerOf2 = 1.0;
        }

        config.multisampleSamples = static_cast<int>(std::pow(2.0, powerOf2));
    }
}
