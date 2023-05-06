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

#include "core/components_ng/pattern/window_scene/screen/screen_pattren.h"

#include "display_info.h"
#include "input_manager.h"
#include "ipc_skeleton.h"
#include "ui/rs_display_node.h"

#include "base/utils/utils.h"
#include "core/common/container.h"
#include "core/components_ng/pattern/window_scene/scene/container/window_pattern.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"

namespace OHOS::Ace::NG {
namespace {
MMI::Direction ConvertDegreeToMMIRotation(float degree)
{
    if (NearEqual(degree, 0)) {
        return MMI::DIRECTION0;
    }
    if (NearEqual(degree, 90)) {
        return MMI::DIRECTION90;
    }
    if (NearEqual(degree, 180)) {
        return MMI::DIRECTION180;
    }
    if (NearEqual(degree, 270)) {
        return MMI::DIRECTION270;
    }
    return MMI::DIRECTION0;
}
} // namespace

void ScreenPattern::OnAttachToFrameNode()
{
    CHECK_NULL_VOID(screenSession_);

    Rosen::RSDisplayNodeConfig config = { .screenId = screenSession_->GetScreenId() };

    auto displayNode = Rosen::RSDisplayNode::Create(config);

    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto context = AceType::DynamicCast<NG::RosenRenderContext>(host->GetRenderContext());
    CHECK_NULL_VOID(context);
    context->SetRSNode(displayNode);

    UpdateDisplayInfo();

    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    auto window = static_cast<WindowPattern*>(container->GetWindow());
    CHECK_NULL_VOID(window);
    auto screenBounds = screenSession_->GetScreenProperty().GetBounds();
    auto rect = Rect(screenBounds.rect_.left_, screenBounds.rect_.top_,
        screenBounds.rect_.width_, screenBounds.rect_.height_);
    window->UpdateViewportConfig(rect, Rosen::WindowSizeChangeReason::UNDEFINED);
}

void ScreenPattern::UpdateDisplayInfo()
{
    CHECK_NULL_VOID(screenSession_);

    auto pid = IPCSkeleton::GetCallingPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto screenId = screenSession_->GetScreenId();
    auto screenProperty = screenSession_->GetScreenProperty();
    auto screenRotation = screenProperty.GetRotation();
    auto screenBounds = screenProperty.GetBounds();

    MMI::Rect screenRect = {
        screenBounds.rect_.left_,
        screenBounds.rect_.top_,
        screenBounds.rect_.width_,
        screenBounds.rect_.height_
    };

    MMI::WindowInfo windowInfo = {
        .id = 0,    // root scene id 0
        .pid = pid,
        .uid = uid,
        .area = screenRect,
        .defaultHotAreas = { screenRect },
        .pointerHotAreas = { screenRect },
        .agentWindowId = 0, // root scene id 0
        .flags = 0  // touchable
    };

    MMI::DisplayInfo displayInfo = {
        .id = screenId,
        .x = screenBounds.rect_.left_,
        .y = screenBounds.rect_.top_,
        .width = screenBounds.rect_.width_,
        .height = screenBounds.rect_.height_,
        .dpi = 240, // TODO: get dpi
        .name = "display" + std::to_string(screenId),
        .uniq = "default" + std::to_string(screenId),
        .direction = ConvertDegreeToMMIRotation(screenRotation)
    };

    MMI::DisplayGroupInfo displayGroupInfo = {
        .width = screenBounds.rect_.width_,
        .height = screenBounds.rect_.height_,
        .focusWindowId = 0, // root scene id 0
        .windowsInfo = { windowInfo },
        .displaysInfo = { displayInfo }
    };
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
}
} // namespace OHOS::Ace::NG
