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

#include "core/components_ng/pattern/marquee/marquee_model_ng.h"

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/marquee/marquee_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"

namespace OHOS::Ace::NG {
void MarqueeModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::MARQUEE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<MarqueePattern>(); });
    if (frameNode->GetChildren().empty()) {
        auto textNode = FrameNode::CreateFrameNode(
            V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
        auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
        textLayoutProperty->UpdateContent(std::string(""));
        frameNode->AddChild(textNode);
    } else {
        auto textChild = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
        CHECK_NULL_VOID(textChild);
        auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        textLayoutProperty->UpdateContent(std::string(""));
    }
    stack->Push(frameNode);
}

void MarqueeModelNG::SetValue(const std::string& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, Src, value);
}

void MarqueeModelNG::SetPlayerStatus(bool playerStatus)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, PlayerStatus, playerStatus);
}

void MarqueeModelNG::SetScrollAmount(double scrollAmount)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, ScrollAmount, scrollAmount);
}

void MarqueeModelNG::SetLoop(int32_t loop)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, Loop, loop);
}

void MarqueeModelNG::SetDirection(MarqueeDirection direction)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, Direction, direction);
}

void MarqueeModelNG::SetAllowScale(bool allowScale)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, AllowScale, allowScale);
}

void MarqueeModelNG::SetTextColor(const Color& textColor)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, FontColor, textColor);
}

void MarqueeModelNG::SetFontSize(const Dimension& fontSize)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, FontSize, fontSize);
}

void MarqueeModelNG::SetFontWeight(const FontWeight& fontWeight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, FontWeight, fontWeight);
}

void MarqueeModelNG::SetFontFamily(const std::vector<std::string>& fontFamilies)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MarqueeLayoutProperty, FontFamily, fontFamilies);
}

void MarqueeModelNG::SetOnStart(std::function<void()>&& onChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnStart(std::move(onChange));
}

void MarqueeModelNG::SetOnBounce(std::function<void()>&& onChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnBounce(std::move(onChange));
}

void MarqueeModelNG::SetOnFinish(std::function<void()>&& onChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnFinish(std::move(onChange));
}
} // namespace OHOS::Ace::NG
