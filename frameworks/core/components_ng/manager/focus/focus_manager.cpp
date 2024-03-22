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

#include "core/components_ng/manager/focus/focus_manager.h"

#include "base/log/dump_log.h"
#include "base/log/log_wrapper.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

void FocusManager::FocusViewShow(const RefPtr<FocusView>& focusView)
{
    CHECK_NULL_VOID(focusView);
    auto lastFocusView = lastFocusView_.Upgrade();
    if (lastFocusView) {
        if (lastFocusView == focusView) {
            return;
        }
        lastFocusView->LostViewFocus();
    }

    auto focusViewWeak = AceType::WeakClaim(AceType::RawPtr(focusView));
    if (std::find(focusViewStack_.begin(), focusViewStack_.end(), focusViewWeak) != focusViewStack_.end()) {
        focusViewStack_.remove(focusViewWeak);
    }
    focusViewStack_.emplace_back(focusViewWeak);
    lastFocusView_ = focusViewWeak;
}

void FocusManager::FocusViewHide(const RefPtr<FocusView>& focusView)
{
    CHECK_NULL_VOID(focusView);
    auto lastFocusView = lastFocusView_.Upgrade();
    if (lastFocusView && lastFocusView == focusView) {
        lastFocusView->LostViewFocus();
        lastFocusView_ = nullptr;
    }
}

void FocusManager::FocusViewClose(const RefPtr<FocusView>& focusView)
{
    CHECK_NULL_VOID(focusView);
    auto lastFocusView = lastFocusView_.Upgrade();
    if (lastFocusView && lastFocusView == focusView) {
        lastFocusView->LostViewFocus();
    }
    auto focusViewWeak = AceType::WeakClaim(AceType::RawPtr(focusView));
    if (std::find(focusViewStack_.begin(), focusViewStack_.end(), focusViewWeak) != focusViewStack_.end()) {
        focusViewStack_.remove(focusViewWeak);
    }
    if (focusViewStack_.empty()) {
        lastFocusView_ = nullptr;
        return;
    }
    if (focusViewStack_.back() != lastFocusView_) {
        lastFocusView_ = focusViewStack_.back();
    }
}

void FocusManager::GetFocusViewMap(FocusViewMap& focusViewMap)
{
    for (const auto& focusViewWeak : focusViewStack_) {
        auto focusView = focusViewWeak.Upgrade();
        if (!focusView) {
            continue;
        }
        auto focusViewId = focusView->GetFrameId();
        auto entryFocusView = focusView->GetEntryFocusView();
        if (entryFocusView && entryFocusView != focusView) {
            auto entryFocusViewId = entryFocusView->GetFrameId();
            auto entryFocusViewWeak = AceType::WeakClaim(AceType::RawPtr(entryFocusView));
            auto iter = focusViewMap.find(entryFocusViewId);
            if (iter == focusViewMap.end()) {
                focusViewMap[entryFocusViewId] = { entryFocusViewWeak, { focusViewWeak } };
            } else {
                iter->second.second.emplace_back(focusViewWeak);
            }
        } else {
            focusViewMap[focusViewId] = { focusViewWeak, {} };
        }
    }
}

void FocusManager::DumpFocusManager()
{
    if (!DumpLog::GetInstance().GetDumpFile()) {
        return;
    }
    DumpLog::GetInstance().Print("Focus view:");
    std::unordered_map<int32_t, std::pair<WeakPtr<FocusView>, std::list<WeakPtr<FocusView>>>> focusViewMap;
    GetFocusViewMap(focusViewMap);
    for (const auto& focusViewInfo : focusViewMap) {
        auto focusView = focusViewInfo.second.first.Upgrade();
        if (!focusView) {
            continue;
        }
        auto childFocusViewWeakList = focusViewInfo.second.second;
        bool isFocusedView = false;
        auto lastFocusView = lastFocusView_.Upgrade();
        auto lastEntryFocusView = lastFocusView ? lastFocusView->GetEntryFocusView() : nullptr;
        if (focusView == lastEntryFocusView) {
            isFocusedView = true;
        }
        std::string information = focusView->GetFrameName();
        information += isFocusedView ? "(*)" : "";
        information += " id:" + std::to_string(focusView->GetFrameId());
        DumpLog::GetInstance().Print(0, information, static_cast<int32_t>(childFocusViewWeakList.size()));
        for (const auto& childWeak : childFocusViewWeakList) {
            auto child = childWeak.Upgrade();
            if (!child) {
                continue;
            }
            std::string childInformation = child->GetFrameName();
            childInformation += child == lastFocusView ? "(*)" : "";
            childInformation += " id:" + std::to_string(child->GetFrameId());
            DumpLog::GetInstance().Print(1, childInformation, 0);
        }
    }
}

} // namespace OHOS::Ace::NG
