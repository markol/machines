#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "afx/AfxConfiguration.hpp"
#include "afx/AfxConfigurationData.hpp"
#include "afx/AfxConfigurationLoader.hpp"

using ::testing::Return;

class MockLoader : public IAfxConfigurationLoader
{
public:
    MOCK_METHOD(AfxConfigurationData*, load, (), (override));
};

//////////////////////////////////////////////////////////////////////////////

TEST(AfxConfigurationTests, MultisamplesValue_ValidatedAndCorrected_When1)
{
    constexpr int         configuredSamples = 1;
    AfxConfigurationData* config = new AfxConfigurationData();
    MockLoader            configLoader;

    config->multisampleBuffers = 1;
    config->multisampleSamples = configuredSamples;

    EXPECT_CALL(configLoader, load())
            .WillRepeatedly(Return(config));

    auto afxConfiguration = AfxConfiguration{ &configLoader };

    ASSERT_EQ(1, afxConfiguration.getConfig().multisampleBuffers);
    ASSERT_EQ(2, afxConfiguration.getConfig().multisampleSamples);
    // FYI: AfxConfiguration DTOR will cleanup `config`
}

TEST(AfxConfigurationTests, MultisamplesValue_ValidatedAndCorrected_When32)
{
    constexpr int         configuredSamples = 32;
    AfxConfigurationData* config = new AfxConfigurationData();
    MockLoader            configLoader;

    config->multisampleBuffers = 1;
    config->multisampleSamples = configuredSamples;

    EXPECT_CALL(configLoader, load())
            .WillRepeatedly(Return(config));

    auto afxConfiguration = AfxConfiguration{ &configLoader };

    ASSERT_EQ(1, afxConfiguration.getConfig().multisampleBuffers);
    ASSERT_EQ(16, afxConfiguration.getConfig().multisampleSamples);
    // FYI: AfxConfiguration DTOR will cleanup `config`
}

TEST(AfxConfigurationTests, MultisamplesValue_ValidatedAndCorrected_When13)
{
    constexpr int         configuredSamples = 13;
    AfxConfigurationData* config = new AfxConfigurationData();
    MockLoader            configLoader;

    config->multisampleBuffers = 1;
    config->multisampleSamples = configuredSamples;

    EXPECT_CALL(configLoader, load())
            .WillRepeatedly(Return(config));

    auto afxConfiguration = AfxConfiguration{ &configLoader };

    ASSERT_EQ(1, afxConfiguration.getConfig().multisampleBuffers);
    ASSERT_EQ(8, afxConfiguration.getConfig().multisampleSamples);
    // FYI: AfxConfiguration DTOR will cleanup `config`
}
