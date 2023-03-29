/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "accessibility_property.h"

#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {
constexpr uint64_t ACTIONS = std::numeric_limits<uint64_t>::max();
std::unordered_set<AceAction> AccessibilityProperty::GetSupportAction() const
{
    static const AceAction allActions[] = {
        AceAction::ACTION_NONE,
        AceAction::GLOBAL_ACTION_BACK,
        AceAction::CUSTOM_ACTION,
        AceAction::ACTION_CLICK,
        AceAction::ACTION_LONG_CLICK,
        AceAction::ACTION_SCROLL_FORWARD,
        AceAction::ACTION_SCROLL_BACKWARD,
        AceAction::ACTION_FOCUS,
        AceAction::ACTION_CLEAR_FOCUS,
        AceAction::ACTION_ACCESSIBILITY_FOCUS,
        AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS,
        AceAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY,
        AceAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY,
        AceAction::ACTION_SET_TEXT,
        AceAction::ACTION_COPY,
        AceAction::ACTION_PASTE,
        AceAction::ACTION_CUT,
        AceAction::ACTION_SELECT,
        AceAction::ACTION_SET_SELECTION,
        AceAction::ACTION_CLEAR_SELECTION,
    };

    std::unordered_set<AceAction> supportActions;
    if (supportActions_ == 0) {
        return supportActions;
    }

    auto finalSupportActions = supportActions_ & ACTIONS;
    for (auto action : allActions) {
        if ((finalSupportActions & (1UL << static_cast<uint32_t>(action))) != 0) {
            supportActions.emplace(action);
        }
    }
    return supportActions;
}
} // namespace OHOS::Ace::NG
