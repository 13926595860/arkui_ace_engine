/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "core/components_ng/pattern/button/button_view.h"

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/button_event_hub.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/ui_task_scheduler.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::NG {
void ButtonView::CreateWithLabel(const std::string& label)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::BUTTON_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    CHECK_NULL_VOID(frameNode);
    if (frameNode->GetChildren().empty()) {
        auto textNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, -1, AceType::MakeRefPtr<TextPattern>());
        CHECK_NULL_VOID(textNode);
        auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        textLayoutProperty->UpdateContent(label);
        frameNode->AddChild(textNode);
    } else {
        auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
        ACE_DCHECK(textChild);
        auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
        ACE_DCHECK(textLayoutProperty);
        textLayoutProperty->UpdateContent(label);
    }
    stack->Push(frameNode);
}

void ButtonView::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::BUTTON_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    stack->Push(frameNode);
}

void ButtonView::SetType(ButtonType buttonType)
{
    ACE_UPDATE_LAYOUT_PROPERTY(ButtonLayoutProperty, Type, buttonType);
}

void ButtonView::SetStateEffect(bool stateEffect)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto buttonEventHub = frameNode->GetEventHub<ButtonEventHub>();
    CHECK_NULL_VOID(buttonEventHub);
    buttonEventHub->SetStateEffect(stateEffect);
}

void ButtonView::SetFontSize(const Dimension& fontSize)
{
    auto frameNode = ViewStackProcessor ::GetInstance()->GetMainFrameNode();
    if (frameNode->GetChildren().empty()) {
        return;
    }
    auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ACE_DCHECK(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    ACE_DCHECK(textLayoutProperty);
    textLayoutProperty->UpdateFontSize(fontSize);
}

void ButtonView::SetFontWeight(Ace::FontWeight fontWeight)
{
    auto frameNode = ViewStackProcessor ::GetInstance()->GetMainFrameNode();
    if (frameNode->GetChildren().empty()) {
        return;
    }
    auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ACE_DCHECK(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    ACE_DCHECK(textLayoutProperty);
    textLayoutProperty->UpdateFontWeight(fontWeight);
}

void ButtonView::SetItalicFontStyle(Ace::FontStyle fontStyle)
{
    auto frameNode = ViewStackProcessor ::GetInstance()->GetMainFrameNode();
    if (frameNode->GetChildren().empty()) {
        return;
    }
    auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ACE_DCHECK(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    ACE_DCHECK(textLayoutProperty);
    textLayoutProperty->UpdateItalicFontStyle(fontStyle);
}

void ButtonView::SetFontFamily(const std::vector<std::string>& fontFamilies)
{
    auto frameNode = ViewStackProcessor ::GetInstance()->GetMainFrameNode();
    if (frameNode->GetChildren().empty()) {
        return;
    }
    auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ACE_DCHECK(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    ACE_DCHECK(textLayoutProperty);
    textLayoutProperty->UpdateFontFamily(fontFamilies);
}

void ButtonView::SetTextColor(const Color& textColor)
{
    auto frameNode = ViewStackProcessor ::GetInstance()->GetMainFrameNode();
    if (frameNode->GetChildren().empty()) {
        return;
    }
    auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ACE_DCHECK(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    ACE_DCHECK(textLayoutProperty);
    textLayoutProperty->UpdateTextColor(textColor);
}
} // namespace OHOS::Ace::NG