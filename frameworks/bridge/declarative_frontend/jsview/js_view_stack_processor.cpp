/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_view_stack_processor.h"

#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/engine/js_types.h"
#include "bridge/declarative_frontend/jsview/models/view_stack_model_impl.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/common/container.h"
#include "core/components_ng/base/view_stack_model.h"
#include "core/components_ng/base/view_stack_model_ng.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "frameworks/core/pipeline/base/element_register.h"

namespace OHOS::Ace {

std::unique_ptr<ViewStackModel> ViewStackModel::instance_ = nullptr;
std::mutex ViewStackModel::mutex_;

ViewStackModel* ViewStackModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::ViewStackModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::ViewStackModelNG());
            } else {
                instance_.reset(new Framework::ViewStackModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace
namespace OHOS::Ace::Framework {

void JSViewStackProcessor::JSVisualState(const JSCallbackInfo& info)
{
    LOGD("JSVisualState");
    if ((info.Length() < 1) || (!info[0]->IsString())) {
        LOGD("JSVisualState: is not a string.");
        ViewStackModel::GetInstance()->ClearVisualState();
        return;
    }

    std::string state = info[0]->ToString();
    VisualState visualState = JSViewStackProcessor::StringToVisualState(state);
    ViewStackModel::GetInstance()->SetVisualState(visualState);
}

// public static emthods exposed to JS
void JSViewStackProcessor::JSBind(BindingTarget globalObj)
{
    LOGD("JSViewStackProcessor::Bind");
    JSClass<JSViewStackProcessor>::Declare("ViewStackProcessor");
    MethodOptions opt = MethodOptions::NONE;

    JSClass<JSViewStackProcessor>::StaticMethod(
        "AllocateNewElmetIdForNextComponent", &JSViewStackProcessor::JsAllocateNewElmetIdForNextComponent, opt);
    JSClass<JSViewStackProcessor>::StaticMethod(
        "StartGetAccessRecordingFor", &JSViewStackProcessor::JsStartGetAccessRecordingFor, opt);
    JSClass<JSViewStackProcessor>::StaticMethod(
        "SetElmtIdToAccountFor", &JSViewStackProcessor::JsSetElmtIdToAccountFor, opt);
    JSClass<JSViewStackProcessor>::StaticMethod(
        "GetElmtIdToAccountFor", &JSViewStackProcessor::JsGetElmtIdToAccountFor, opt);
    JSClass<JSViewStackProcessor>::StaticMethod(
        "StopGetAccessRecording", &JSViewStackProcessor::JsStopGetAccessRecording, opt);
    JSClass<JSViewStackProcessor>::StaticMethod(
        "ImplicitPopBeforeContinue", &JSViewStackProcessor::JsImplicitPopBeforeContinue, opt);
    JSClass<JSViewStackProcessor>::StaticMethod("visualState", JSVisualState, opt);
    JSClass<JSViewStackProcessor>::StaticMethod("MakeUniqueId", &JSViewStackProcessor::JSMakeUniqueId, opt);
    JSClass<JSViewStackProcessor>::StaticMethod("UsesNewPipeline", &JSViewStackProcessor::JsUsesNewPipeline, opt);
    JSClass<JSViewStackProcessor>::Bind<>(globalObj);
}

VisualState JSViewStackProcessor::StringToVisualState(const std::string& stateString)
{
    if (stateString == "normal") {
        return VisualState::NORMAL;
    }
    if (stateString == "focused") {
        return VisualState::FOCUSED;
    }
    if (stateString == "pressed") {
        return VisualState::PRESSED;
    }
    if (stateString == "disabled") {
        return VisualState::DISABLED;
    }
    if (stateString == "hover") {
        return VisualState::HOVER;
    }
    LOGE("Unknown visual state \"%{public}s\", resetting to UNDEFINED", stateString.c_str());
    return VisualState::NOTSET;
}

void JSViewStackProcessor::JsStartGetAccessRecordingFor(ElementIdType elmtId)
{
    ViewStackModel::GetInstance()->StartGetAccessRecordingFor(elmtId);
}

int32_t JSViewStackProcessor::JsGetElmtIdToAccountFor()
{
    return ViewStackModel::GetInstance()->GetElmtIdToAccountFor();
}

void JSViewStackProcessor::JsSetElmtIdToAccountFor(ElementIdType elmtId)
{
    ViewStackModel::GetInstance()->SetElmtIdToAccountFor(elmtId);
}

void JSViewStackProcessor::JsStopGetAccessRecording()
{
    return ViewStackModel::GetInstance()->StopGetAccessRecording();
}

void JSViewStackProcessor::JsImplicitPopBeforeContinue()
{
    ViewStackModel::GetInstance()->ImplicitPopBeforeContinue();
}

void JSViewStackProcessor::JSMakeUniqueId(const JSCallbackInfo& info)
{
    const auto result = ElementRegister::GetInstance()->MakeUniqueId();
    info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(result)));
}
/**
 * return true of current Container uses new Pipeline
 */
bool JSViewStackProcessor::JsUsesNewPipeline()
{
    return Container::IsCurrentUseNewPipeline();
}

} // namespace OHOS::Ace::Framework
