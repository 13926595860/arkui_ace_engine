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

#include "core/components_ng/base/view_abstract.h"

#include "base/utils/utils.h"
#include "core/components_ng/base/view_stack_processor.h"

namespace OHOS::Ace::NG {
void ViewAbstract::SetWidth(const CalcLength& width)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto layoutProperty = frameNode->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateCalcSelfIdealSize(CalcSize(width, CalcLength(-1.0f)));
}

void ViewAbstract::SetHeight(const CalcLength& height)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto layoutProperty = frameNode->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateCalcSelfIdealSize(CalcSize(CalcLength(-1.0f), height));
}

void ViewAbstract::SetBackgroundColor(const Color& color)
{
    ACE_UPDATE_RENDER_CONTEXT(BackgroundColor, color);
}

void ViewAbstract::SetLayoutWeight(int32_t value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, LayoutWeight, static_cast<float>(value));
}

void ViewAbstract::SetPadding(const CalcLength& value)
{
    PaddingProperty padding;
    padding.SetEdges(value);
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, Padding, padding);
}

void ViewAbstract::SetPadding(const PaddingProperty& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, Padding, value);
}

void ViewAbstract::SetOnClick(GestureEventFunc&& clickEventFunc)
{
    auto gestureHub = ViewStackProcessor::GetInstance()->GetMainFrameNodeGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    gestureHub->SetClickEvent(std::move(clickEventFunc));
}

void ViewAbstract::SetOnTouch(TouchEventFunc&& touchEventFunc)
{
    auto gestureHub = ViewStackProcessor::GetInstance()->GetMainFrameNodeGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    gestureHub->SetTouchEvent(std::move(touchEventFunc));
}

void ViewAbstract::Pop()
{
    ViewStackProcessor::GetInstance()->Pop();
}
} // namespace OHOS::Ace::NG
