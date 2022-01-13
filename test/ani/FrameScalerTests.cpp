#include <memory>
#include <gtest/gtest.h>
#include "ani/FrameScaler.hpp"

TEST(FrameScalerTests, FrameScalerInitializes) {
    std::unique_ptr<FrameScaler> frameScaler(new FrameScaler(2,2,4,4));

    ASSERT_EQ(2, frameScaler->getSourceWidth());
    ASSERT_EQ(2, frameScaler->getSourceHeight());
    ASSERT_EQ(4, frameScaler->getDestWidth());
    ASSERT_EQ(4, frameScaler->getDestHeight());
}

TEST(FrameScalerTests, FrameScalerResizesFrame) {
    std::unique_ptr<FrameScaler> frameScaler(new FrameScaler(2,2,4,4));

    uint8_t image2x2[] {0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB,
                        0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB};

    uint8_t* image4x4 = frameScaler->scaleFrame(image2x2);

    ASSERT_EQ((uint8_t)0xBB, *static_cast<uint8_t*>(image4x4+14));
    ASSERT_EQ((uint8_t)0xAA, *static_cast<uint8_t*>(image4x4+16));
}
