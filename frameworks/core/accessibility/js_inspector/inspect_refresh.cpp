/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "inspect_refresh.h"

namespace OHOS::Ace::Framework {
InspectRefresh::InspectRefresh(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectRefresh::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("refreshing", "false"));
    attrs_.insert(std::make_pair("type", "auto"));
    attrs_.insert(std::make_pair("lasttime", "false"));
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        attrs_.insert(std::make_pair("friction", "42"));
    } else if (deviceType == DeviceType::WEARABLE || deviceType == DeviceType::WATCH) {
        styles_.insert(std::make_pair("friction", "62"));
    }
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));

    // add for the styles
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("background-color", "white"));
        styles_.insert(std::make_pair("progress-color", "black"));
    } else {
        styles_.insert(std::make_pair("background-color", "black"));
        styles_.insert(std::make_pair("progress-color", "white"));
    }
    styles_.insert(std::make_pair("min-width", "0"));
    styles_.insert(std::make_pair("min-height", "0"));
    styles_.insert(std::make_pair("box-shadow", "0"));
}
} // namespace OHOS::Ace::Framework
