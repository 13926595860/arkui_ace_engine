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

#include "core/components_ng/pattern/effect_view/effect_view_model_ng.h"

#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/effect_view/effect_view_pattern.h"
// #include "core/components_ng/render/adapter/rosen_render_context.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {
void EffectViewModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::EFFECT_VIEW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<EffectViewPattern>(); });
    stack->Push(frameNode);
#ifdef ENABLE_ROSEN_BACKEND
    // auto context = AceType::DynamicCast<NG::RosenRenderContext>(frameNode->GetRenderContext());
    // CHECK_NULL_VOID(context);
    // context->SetRSNode(rsNode);
#endif
}
} // namespace OHOS::Ace::NG
