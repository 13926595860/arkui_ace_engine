/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adapter/ohos/osal/resource_convertor.h"

namespace OHOS::Ace {

Global::Resource::DeviceType ConvertDeviceTypeToGlobal(DeviceType type)
{
    switch (type) {
        case DeviceType::PHONE:
            return Global::Resource::DeviceType::DEVICE_PHONE;
        case DeviceType::TV:
            return Global::Resource::DeviceType::DEVICE_TV;
        case DeviceType::WATCH:
            return Global::Resource::DeviceType::DEVICE_WEARABLE;
        case DeviceType::CAR:
            return Global::Resource::DeviceType::DEVICE_CAR;
        case DeviceType::TABLET:
            return Global::Resource::DeviceType::DEVICE_TABLET;
        case DeviceType::TWO_IN_ONE:
            return Global::Resource::DeviceType::DEVICE_TWOINONE;
        case DeviceType::WEARABLE:
            return Global::Resource::DeviceType::DEVICE_WEARABLE;
        default:
            return Global::Resource::DeviceType::DEVICE_NOT_SET;
    }
}

Global::Resource::Direction ConvertDirectionToGlobal(DeviceOrientation orientation)
{
    switch (orientation) {
        case DeviceOrientation::PORTRAIT:
            return Global::Resource::Direction::DIRECTION_VERTICAL;
        case DeviceOrientation::LANDSCAPE:
            return Global::Resource::Direction::DIRECTION_HORIZONTAL;
        default:
            return Global::Resource::Direction::DIRECTION_NOT_SET;
    }
}

Global::Resource::ScreenDensity ConvertDensityToGlobal(double density)
{
    static const std::vector<std::pair<double, Global::Resource::ScreenDensity>> resolutions = {
        { 0.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_NOT_SET },
        { 120.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_SDPI },
        { 160.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_MDPI },
        { 240.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_LDPI },
        { 320.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_XLDPI },
        { 480.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_XXLDPI },
        { 640.0, Global::Resource::ScreenDensity::SCREEN_DENSITY_XXXLDPI },
    };
    double deviceDpi = density * DPI_BASE;
    auto resolution = Global::Resource::ScreenDensity::SCREEN_DENSITY_NOT_SET;
    for (const auto& [dpi, value] : resolutions) {
        resolution = value;
        if (LessOrEqual(deviceDpi, dpi)) {
            break;
        }
    }
    return resolution;
}
Global::Resource::ColorMode ConvertColorModeToGlobal(ColorMode colorMode)
{
    switch (colorMode) {
        case ColorMode::DARK:
            return Global::Resource::ColorMode::DARK;
        case ColorMode::LIGHT:
            return Global::Resource::ColorMode::LIGHT;
        default:
            return Global::Resource::ColorMode::COLOR_MODE_NOT_SET;
    }
}

Global::Resource::InputDevice ConvertInputDevice(bool deviceAccess)
{
    return deviceAccess ? Global::Resource::InputDevice::INPUTDEVICE_POINTINGDEVICE :
        Global::Resource::InputDevice::INPUTDEVICE_NOT_SET;
}

std::shared_ptr<Global::Resource::ResConfig> ConvertConfigToGlobal(const ResourceConfiguration& config)
{
    std::shared_ptr<Global::Resource::ResConfig> newResCfg(Global::Resource::CreateResConfig());
    icu::Locale locale(config.GetLanguage().c_str());
    newResCfg->SetLocaleInfo(locale);
    newResCfg->SetDeviceType(ConvertDeviceTypeToGlobal(config.GetDeviceType()));
    newResCfg->SetDirection(ConvertDirectionToGlobal(config.GetOrientation()));
    newResCfg->SetScreenDensity(config.GetDensity());
    newResCfg->SetColorMode(ConvertColorModeToGlobal(config.GetColorMode()));
    newResCfg->SetInputDevice(ConvertInputDevice(config.GetDeviceAccess()));
    newResCfg->SetAppColorMode(config.GetColorModeIsSetByApp());
    newResCfg->SetMcc(config.GetMcc());
    newResCfg->SetMnc(config.GetMnc());
    icu::Locale preferredLocale(config.GetPreferredLanguage().c_str());
    newResCfg->SetPreferredLocaleInfo(preferredLocale);
    return newResCfg;
}

DeviceType ConvertDeviceTypeToAce(Global::Resource::DeviceType type)
{
    switch (type) {
        case Global::Resource::DeviceType::DEVICE_PHONE:
            return DeviceType::PHONE;
        case Global::Resource::DeviceType::DEVICE_TV:
            return DeviceType::TV;
        case Global::Resource::DeviceType::DEVICE_WEARABLE:
            return DeviceType::WATCH;
        case Global::Resource::DeviceType::DEVICE_CAR:
            return DeviceType::CAR;
        case Global::Resource::DeviceType::DEVICE_TABLET:
            return DeviceType::TABLET;
        default:
            return DeviceType::UNKNOWN;
    }
}

DeviceOrientation ConvertDirectionToAce(Global::Resource::Direction orientation)
{
    switch (orientation) {
        case Global::Resource::Direction::DIRECTION_VERTICAL:
            return DeviceOrientation::PORTRAIT;
        case Global::Resource::Direction::DIRECTION_HORIZONTAL:
            return DeviceOrientation::LANDSCAPE;
        default:
            return DeviceOrientation::ORIENTATION_UNDEFINED;
    }
}

double ConvertDensityToAce(Global::Resource::ScreenDensity density)
{
    switch (density) {
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_SDPI:
            return 120.0 / DPI_BASE;
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_MDPI:
            return 160.0 / DPI_BASE;
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_LDPI:
            return 240.0 / DPI_BASE;
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_XLDPI:
            return 320.0 / DPI_BASE;
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_XXLDPI:
            return 480.0 / DPI_BASE;
        case Global::Resource::ScreenDensity::SCREEN_DENSITY_XXXLDPI:
            return 640.0 / DPI_BASE;
        default:
            return 0.0;
    }
}

} // namespace OHOS::Ace