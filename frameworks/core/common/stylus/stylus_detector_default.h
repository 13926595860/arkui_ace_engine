/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STYLUS_STYLUS_DETECTOR_DEFAULT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STYLUS_STYLUS_DETECTOR_DEFAULT_H

#include "interfaces/inner_api/ace/stylus/stylus_detector_interface.h"

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {
class ACE_FORCE_EXPORT StylusDetectorDefault final : public StylusDetectorInterface, public NonCopyable {
public:
    static StylusDetectorDefault* GetInstance();

    bool IsEnable() override;

    bool RegisterStylusInteractionListener(
        const std::string& bundleName, const std::shared_ptr<IStylusDetectorCallback>& callback) override;

    void UnRegisterStylusInteractionListener(const std::string& bundleName) override;

    bool Notify(const NotifyInfo& notifyInfo) override;

    void ExecuteCommand(const std::vector<std::string>& params);

private:
    std::shared_ptr<IStylusDetectorCallback> defaultCallback_ = nullptr;
    bool isEnable_ = false;
    std::string defaultText_ = "Now use default stylus for input text.";
    int32_t defaultNodeId_ = 0;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STYLUS_STYLUS_DETECTOR_DEFAULT_H