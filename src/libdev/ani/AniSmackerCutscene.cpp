#include "ani/AniSmackerCutscene.hpp"

AniSmackerCutscene::AniSmackerCutscene(const SysPathName& path, size_t xCoordTo, size_t yCoordTo, size_t fsWidth, size_t fsHeight)
    : AniSmackerRegular(path, xCoordTo, yCoordTo, false),
      pFrameScaler_(nullptr)
{
    fullScreenWidth_ = fsWidth;
    fullScreenHeight_ = fsHeight;
}

AniSmackerCutscene::~AniSmackerCutscene() {
    if (pFrameScaler_ != nullptr) {
        delete pFrameScaler_;
    }
}

RenSurface AniSmackerCutscene::createSmackerSurface(RenDevice *pDevice)
{
    return RenSurface::createAnonymousSurface( fullScreenWidth_, fullScreenHeight_, pDevice->backSurface() );
}


void AniSmackerCutscene::copyCurrentVideoFrameToBuffer(RenSurface& renderSurface) {
    uint8_t* frame = reinterpret_cast<uint8_t*>(fillBufferForCurrentFrame());

    renderSurface.copyFromRGBABuffer(reinterpret_cast<uint*>(fetchOrInitializeScaler()->scaleFrame(frame)));
}

FrameScaler* AniSmackerCutscene::fetchOrInitializeScaler() {
    if (pFrameScaler_ == nullptr) {
        // By now, width() & height() will have the smacker file's w/h values
        pFrameScaler_ = new FrameScaler(this->width(), this->height(), fullScreenWidth_, fullScreenHeight_);
    }

    return pFrameScaler_;
}
