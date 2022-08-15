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

#include "frameworks/bridge/declarative_frontend/jsview/js_column.h"

#include "base/log/ace_trace.h"
#include "core/components_ng/pattern/linear_layout/column_view.h"

namespace OHOS::Ace::Framework {
std::string JSColumn::inspectorTag_ = "";

void JSColumn::Create(const JSCallbackInfo& info)
{
    std::optional<Dimension> space;
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> spaceVal = obj->GetProperty("space");
        Dimension value;
        if (ParseJsDimensionVp(spaceVal, value)) {
            space = value;
        }
    }
    NG::ColumnView::Create(space);
}

void JSColumn::CreateWithWrap(const JSCallbackInfo& info) {}

void JSColumn::SetInspectorTag(const std::string& inspectorTag)
{
    inspectorTag_ = inspectorTag;
}

void JSColumn::ClearInspectorTag()
{
    inspectorTag_.clear();
}

void JSColumn::SetAlignItems(int32_t value)
{
    if ((value == static_cast<int32_t>(FlexAlign::FLEX_START)) ||
        (value == static_cast<int32_t>(FlexAlign::FLEX_END)) || (value == static_cast<int32_t>(FlexAlign::CENTER)) ||
        (value == static_cast<int32_t>(FlexAlign::STRETCH))) {
        NG::ColumnView::AlignItems(static_cast<FlexAlign>(value));
    } else {
        // FIXME: we have a design issue here, setters return void, can not signal error to JS
        LOGE("invalid value for justifyContent");
    }
}

// TODO: HorizontalAlignDeclaration is not used, remove later
void HorizontalAlignDeclaration::ConstructorCallback(const JSCallbackInfo& args) {}

void HorizontalAlignDeclaration::DestructorCallback(HorizontalAlignDeclaration* obj) {}

} // namespace OHOS::Ace::Framework
