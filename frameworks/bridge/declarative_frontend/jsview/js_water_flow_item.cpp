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

#include "frameworks/bridge/declarative_frontend/jsview/js_water_flow_item.h"

#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/core/components_v2/water_flow/water_flow_item_component.h"

namespace OHOS::Ace::Framework {
void JSWaterFlowItem::Create()
{
    auto itemComponent = AceType::MakeRefPtr<V2::WaterFlowItemComponent>();
    ViewStackProcessor::GetInstance()->Push(itemComponent);
}

void JSWaterFlowItem::JSBind(BindingTarget globalObj)
{
    LOGD("JSWaterFlowItem:JSBind");
    JSClass<JSWaterFlowItem>::Declare("FlowItem");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSWaterFlowItem>::StaticMethod("create", &JSWaterFlowItem::Create, opt);
    JSClass<JSWaterFlowItem>::StaticMethod("rowSpan", &JSWaterFlowItem::SetRowSpan, opt);
    JSClass<JSWaterFlowItem>::StaticMethod("columnSpan", &JSWaterFlowItem::SetColumnSpan, opt);

    JSClass<JSWaterFlowItem>::Inherit<JSContainerBase>();
    JSClass<JSWaterFlowItem>::Inherit<JSViewAbstract>();
    JSClass<JSWaterFlowItem>::Bind<>(globalObj);
}

void JSWaterFlowItem::SetRowSpan(int32_t rowSpan)
{
    auto flowItem =
        AceType::DynamicCast<V2::WaterFlowItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (flowItem) {
        flowItem->SetRowSpan(rowSpan);
    }
}

void JSWaterFlowItem::SetColumnSpan(int32_t columnSpan)
{
    auto flowItem =
        AceType::DynamicCast<V2::WaterFlowItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (flowItem) {
        flowItem->SetColumnSpan(columnSpan);
    }
}
} // namespace OHOS::Ace::Framework
