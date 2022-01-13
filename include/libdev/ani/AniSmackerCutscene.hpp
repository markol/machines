#pragma once

#include <cstddef>
#include "ani/AniSmackerRegular.hpp"
#include "ani/FrameScaler.hpp"

class AniSmackerCutscene : public AniSmackerRegular {
public:
    AniSmackerCutscene(const SysPathName& path, size_t xCoordTo, size_t yCoordTo, size_t fsWidth, size_t fsHeight);
    virtual ~AniSmackerCutscene();

    AniSmackerCutscene(const AniSmackerCutscene& other) = delete;
    AniSmackerCutscene& operator=(const AniSmackerCutscene) = delete;
    bool operator==(const AniSmackerCutscene& ) const = delete;

protected:
    virtual RenSurface createSmackerSurface(RenDevice *pDevice) override;
    virtual void copyCurrentVideoFrameToBuffer(RenSurface& renderSurface) override;

private:
    FrameScaler* fetchOrInitializeScaler();

    FrameScaler* pFrameScaler_;
    size_t fullScreenWidth_;
    size_t fullScreenHeight_;
};
