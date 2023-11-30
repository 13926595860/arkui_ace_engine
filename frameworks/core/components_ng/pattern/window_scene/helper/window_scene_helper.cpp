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

#include "core/components_ng/pattern/window_scene/helper/window_scene_helper.h"

#include "core/common/container.h"
#include "core/components_ng/pattern/window_scene/scene/system_window_scene.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "session/host/include/session.h"

#if not defined(ACE_UNITTEST)
#if defined(ENABLE_STANDARD_INPUT)
#include "input_method_controller.h"
#endif
#endif

namespace OHOS::Ace::NG {
RefPtr<UINode> WindowSceneHelper::FindWindowScene(RefPtr<FrameNode> targetNode)
{
    CHECK_NULL_RETURN(targetNode, nullptr);

    auto container = Container::Current();
    if (!container || !container->IsScenceBoardWindow() || !container->IsSceneBoardEnabled()) {
        TAG_LOGI(AceLogTag::ACE_KEYBOARD, "Container nullptr Or not ScenceBoardWindow.");
        return nullptr;
    }

    TAG_LOGD(AceLogTag::ACE_KEYBOARD, "FindWindowScene start.");
    auto parent = targetNode->GetParent();
    while (parent && parent->GetTag() != V2::WINDOW_SCENE_ETS_TAG) {
        parent = parent->GetParent();
    }
    CHECK_NULL_RETURN(parent, nullptr);
    TAG_LOGD(AceLogTag::ACE_KEYBOARD, "FindWindowScene successfully.");

    return parent;
}

sptr<Rosen::Session> GetCurSession(RefPtr<FrameNode> focusedFrameNode)
{
    RefPtr<UINode> sceneBoardWindowUINode = WindowSceneHelper::FindWindowScene(focusedFrameNode);
    if (sceneBoardWindowUINode == nullptr) {
        TAG_LOGD(AceLogTag::ACE_KEYBOARD, "FindWindowScene failed.");
        return nullptr;
    }

    auto windowSceneFrameNode = AceType::DynamicCast<FrameNode>(sceneBoardWindowUINode);
    if (windowSceneFrameNode == nullptr) {
        TAG_LOGD(AceLogTag::ACE_KEYBOARD, "WindowFrameNode to FrameNode failed.");
        return nullptr;
    }

    auto windowScenePattern = windowSceneFrameNode->GetPattern<SystemWindowScene>();
    if (windowScenePattern == nullptr) {
        TAG_LOGD(AceLogTag::ACE_KEYBOARD, "windowScenePattern is nullptr.");
        return nullptr;
    }

    auto window2patternSession = windowScenePattern->GetSession();
    if (window2patternSession == nullptr) {
        TAG_LOGI(AceLogTag::ACE_KEYBOARD, "The session is nullptr between window and pattern.");
        return nullptr;
    }
    sptr<Rosen::Session> session;
    session = window2patternSession;

    return session;
}

bool WindowSceneHelper::IsWindowScene(RefPtr<FrameNode> focusedFrameNode)
{
    bool isWindowSceneFlag = false;
    sptr<Rosen::Session> window2patternSession = GetCurSession(focusedFrameNode);
    if (window2patternSession == nullptr) {
        TAG_LOGI(AceLogTag::ACE_KEYBOARD, "The session between window and pattern is nullptr.");
        return isWindowSceneFlag;
    }
    isWindowSceneFlag = window2patternSession->GetSessionInfo().isSystem_;

    return isWindowSceneFlag;
}

uint32_t WindowSceneHelper::GetFocusSystemWindowId(RefPtr<FrameNode> focusedFrameNode)
{
    uint32_t focusSystemWindowId = 0;
    bool isWindowScene = IsWindowScene(focusedFrameNode);
    sptr<Rosen::Session> window2patternSession = GetCurSession(focusedFrameNode);
    if (window2patternSession == nullptr) {
        TAG_LOGI(AceLogTag::ACE_KEYBOARD, "The session between window and pattern is nullptr.");
        return focusSystemWindowId;
    }
    if (isWindowScene) {
        focusSystemWindowId = window2patternSession->GetPersistentId();
        LOGI("Get systemWindowScene id( %{public}u ) successfully.", focusSystemWindowId);
    }

    return focusSystemWindowId;
}

bool WindowSceneHelper::IsFocusWindowSceneCloseKeyboard(RefPtr<FrameNode> focusedFrameNode)
{
    bool isWindowSceneSaveKeyboardFlag = false;
    sptr<Rosen::Session> window2patternSession = GetCurSession(focusedFrameNode);
    if (window2patternSession == nullptr) {
        TAG_LOGI(AceLogTag::ACE_KEYBOARD, "The session between window and pattern is nullptr.");
        return isWindowSceneSaveKeyboardFlag;
    }
    isWindowSceneSaveKeyboardFlag = window2patternSession->GetSCBKeepKeyboardFlag();

    return isWindowSceneSaveKeyboardFlag;
}

void WindowSceneHelper::IsWindowSceneCloseKeyboard(bool isFocusTypeNode, RefPtr<FrameNode> frameNode)
{
#if defined (ENABLE_STANDARD_INPUT)
    // If focus pattern does not need softkeyboard, close it, in windowScene.
    auto curPattern = frameNode->GetPattern<NG::Pattern>();
    CHECK_NULL_VOID(curPattern);
    bool isNeedKeyBoard = curPattern->NeedSoftKeyboard();
    auto isWindowScene = IsWindowScene(frameNode);
    auto saveKeyboard = IsFocusWindowSceneCloseKeyboard(frameNode);
    TAG_LOGD(AceLogTag::ACE_KEYBOARD, "isWindowScene/saveKeyboard(%{public}d/%{public}d)", isWindowScene, saveKeyboard);
    if ((isWindowScene && !saveKeyboard) && (isFocusTypeNode && !isNeedKeyBoard)) {
        TAG_LOGD(AceLogTag::ACE_KEYBOARD, "WindowSceneFrameNode not NeedSoftKeyboard.");
        auto inputMethod = MiscServices::InputMethodController::GetInstance();
        if (inputMethod) {
            inputMethod->Close();
            TAG_LOGD(AceLogTag::ACE_KEYBOARD, "SoftKeyboard Closes Successfully.");
        }
    }
#endif
}

void WindowSceneHelper::IsCloseKeyboard(bool isFocusTypeNode, RefPtr<FrameNode> frameNode)
{
#if defined (ENABLE_STANDARD_INPUT)
    // If focus pattern does not need softkeyboard, close it, not in windowScene.
    auto curPattern = frameNode->GetPattern<NG::Pattern>();
    CHECK_NULL_VOID(curPattern);
    bool isNeedKeyBoard = curPattern->NeedSoftKeyboard();
    if (isFocusTypeNode && !isNeedKeyBoard) {
        TAG_LOGD(AceLogTag::ACE_KEYBOARD, "FrameNode not NeedSoftKeyboard.");
        auto inputMethod = MiscServices::InputMethodController::GetInstance();
        if (inputMethod) {
            inputMethod->Close();
            TAG_LOGD(AceLogTag::ACE_KEYBOARD, "SoftKeyboard Closes Successfully.");
        }
    }
#endif
}
} // namespace OHOS::Ace::NG
