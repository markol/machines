#include "device/cd_helper.hpp"

#include "base/compiler.hpp"    // For WEAK_SYMBOL def
#include "device/cd.hpp"
#include "device/DevCDImpl.hpp"
#include "system/registry.hpp"

WEAK_SYMBOL void device::helper::cd::configure(DevCD* cd)
{
    constexpr char configEnableMusic[] = "Options\\Music\\enabled";
    auto configValue                   = std::string{};
    auto dummy                         = SysRegistry::KeyHandle{};

    // by default enable, unless otherwise specified
    bool enableMusic                   = true;

    if (SysRegistry::SUCCESS == SysRegistry::instance().queryValueNoRecord(dummy, configEnableMusic, configValue))
    {
        enableMusic &= ( std::atoi(configValue.c_str()) != 0 );
    }

    cd->pImpl_->musicEnabled_ = enableMusic;
}
