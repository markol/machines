#pragma once

#include <cstddef>

extern "C" {
    #include <libswscale/swscale.h>
}

class FrameScaler {
public:
    FrameScaler(size_t srcWidth, size_t srcHeight, size_t dstWidth, size_t dstHeight);
    virtual ~FrameScaler();

    size_t getSourceWidth() const;
    size_t getSourceHeight() const;
    size_t getDestWidth() const;
    size_t getDestHeight() const;

    // Returned frame is the scaled destination frame.
    uint8_t* scaleFrame(uint8_t* srcFrame);

private:
    void resetFrameData();

    size_t srcWidth_;
    size_t srcHeight_;
    size_t dstWidth_;
    size_t dstHeight_;

    SwsContext* pSwsContext_;
    SwsFilter* pSrcFilter_;
    SwsFilter* pDstFilter_;

    uint8_t* pDstFrame_;
};
