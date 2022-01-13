#include <algorithm>
#include <libavutil/pixfmt.h>
#include "ani/FrameScaler.hpp"

FrameScaler::FrameScaler(size_t srcWidth, size_t srcHeight, size_t dstWidth, size_t dstHeight) {
    this->srcWidth_ = srcWidth;
    this->srcHeight_ = srcHeight;
    this->dstWidth_ = dstWidth;
    this->dstHeight_ = dstHeight;

    pSrcFilter_ = sws_getDefaultFilter(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
    pDstFilter_ = sws_getDefaultFilter(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

    pSwsContext_ = sws_getContext(
                srcWidth,
                srcHeight,
                AVPixelFormat::AV_PIX_FMT_RGBA,
                dstWidth,
                dstHeight,
                AVPixelFormat::AV_PIX_FMT_RGBA,
                SWS_LANCZOS,
                pSrcFilter_,
                pDstFilter_,
                nullptr
    );

    pDstFrame_ = new uint8_t[dstWidth*dstHeight*4];
    resetFrameData();
}

FrameScaler::~FrameScaler() {
    sws_freeContext(pSwsContext_);
    sws_freeFilter(pSrcFilter_);
    sws_freeFilter(pDstFilter_);

    delete[] pDstFrame_;
}

size_t FrameScaler::getSourceWidth() const {
    return this->srcWidth_;
}

size_t FrameScaler::getSourceHeight() const {
    return this->srcHeight_;
}

size_t FrameScaler::getDestWidth() const {
    return this->dstWidth_;
}

size_t FrameScaler::getDestHeight() const {
    return this->dstHeight_;
}

uint8_t* FrameScaler::scaleFrame(uint8_t* srcFrame) {
    int strideSrc = srcWidth_*4;
    int strideDst = dstWidth_*4;
    // Apparently, you only use the first indice when doing RGB scaling
    uint8_t *rgb_src[3] = {srcFrame, nullptr, nullptr};
    uint8_t *rgb_dst[3] = {pDstFrame_, nullptr, nullptr};
    int src_stride[3]= {strideSrc, 0, 0};
    int dst_stride[3]= {strideDst, 0, 0};

    resetFrameData();
    sws_scale(pSwsContext_, rgb_src, src_stride, 0, srcHeight_, rgb_dst, dst_stride);
    return pDstFrame_;
}

void FrameScaler::resetFrameData() {
    const size_t dstSize = dstWidth_*dstHeight_*4;

    std::fill(pDstFrame_, pDstFrame_+dstSize, 0x00);
}
