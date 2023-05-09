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

#include "bridge/declarative_frontend/jsview/js_richtext.h"

#include <string>

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/richtext_model_impl.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/common/container.h"
#include "core/components/web/web_component.h"
#include "core/components/web/web_property.h"
#include "core/components_ng/pattern/richtext/richtext_model_ng.h"
#include "core/components_ng/pattern/web/web_view.h"

namespace OHOS::Ace {
std::unique_ptr<RichTextModel> RichTextModel::instance_ = nullptr;
std::mutex RichTextModel::mutex_;
RichTextModel* RichTextModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::RichTextModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::RichTextModelNG());
            } else {
                instance_.reset(new Framework::RichTextModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}
} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
void JSRichText::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGI("richtext create error, info is non-valid");
        return;
    }

    std::string data;

    if (ParseJsString(info[0], data)) {
        RichTextModel::GetInstance()->Create(data);
    }
}

void JSRichText::JSBind(BindingTarget globalObj)
{
    JSClass<JSRichText>::Declare("RichText");
    JSClass<JSRichText>::StaticMethod("create", &JSRichText::Create);
    JSClass<JSRichText>::StaticMethod("onStart", &JSRichText::OnStart);
    JSClass<JSRichText>::StaticMethod("onComplete", &JSRichText::OnComplete);
    JSClass<JSRichText>::Inherit<JSViewAbstract>();
    JSClass<JSRichText>::Bind<>(globalObj);
}

void JSRichText::OnStart(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));

        auto event = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        };

        RichTextModel::GetInstance()->SetOnPageStart(event);
    }
}

void JSRichText::OnComplete(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));

        auto event = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        };

        RichTextModel::GetInstance()->SetOnPageFinish(event);
    }
}
} // namespace OHOS::Ace::Framework
