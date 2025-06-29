/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"

#include "base/json/json_util.h"
#include "base/log/ace_performance_monitor.h"
#include "base/log/ace_trace.h"
#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent.h"

namespace OHOS::Ace::Framework {

JsFunction::JsFunction(const JSRef<JSFunc>& jsFunction)
{
    jsFunction_ = jsFunction;
}

JsFunction::JsFunction(const JSRef<JSObject>& jsObject, const JSRef<JSFunc>& jsFunction)
{
    jsThis_ = jsObject;
    jsFunction_ = jsFunction;
}

JsWeakFunction::JsWeakFunction(const JSRef<JSFunc>& jsFunction)
{
    jsWeakFunction_ = jsFunction;
}

JsWeakFunction::JsWeakFunction(const JSRef<JSObject>& jsObject, const JSRef<JSFunc>& jsFunction)
{
    jsThis_ = jsObject;
    jsWeakFunction_ = jsFunction;
}

JSRef<JSObject> JsFunction::GetTapLocation(const FingerInfo& fingerInfo)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    JSRef<JSObject> tapLocation = objectTemplate->NewInstance();
    const OHOS::Ace::Offset& localOffset = fingerInfo.localLocation_;
    const OHOS::Ace::Offset& globalOffset = fingerInfo.globalLocation_;
    const OHOS::Ace::Offset& screenOffset = fingerInfo.screenLocation_;
    tapLocation->SetProperty<double>("windowX", PipelineBase::Px2VpWithCurrentDensity(globalOffset.GetX()));
    tapLocation->SetProperty<double>("windowY", PipelineBase::Px2VpWithCurrentDensity(globalOffset.GetY()));
    tapLocation->SetProperty<double>("x", PipelineBase::Px2VpWithCurrentDensity(localOffset.GetX()));
    tapLocation->SetProperty<double>("y", PipelineBase::Px2VpWithCurrentDensity(localOffset.GetY()));
    tapLocation->SetProperty<double>("displayX", PipelineBase::Px2VpWithCurrentDensity(screenOffset.GetX()));
    tapLocation->SetProperty<double>("displayY", PipelineBase::Px2VpWithCurrentDensity(screenOffset.GetY()));

    return tapLocation;
}

JsFunction::~JsFunction() {}

void JsFunctionBase::Execute(const JSRef<JSObject>& jsParamsObject)
{
    JSRef<JSVal> paramObj = JSRef<JSVal>::Cast(jsParamsObject);
    ExecuteJS(1, &paramObj);
}

void JsFunctionBase::Execute(const std::vector<std::string>& keys, const std::string& param)
{
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(param);
    if (!argsPtr) {
        return;
    }
    JSRef<JSObject> eventInfo = JSRef<JSObject>::New();
    for (auto iter = keys.begin(); iter != keys.end(); iter++) {
        const std::string key = *iter;
        const auto value = argsPtr->GetValue(key);
        if (!value) {
            LOGD("key[%{public}s] not exist.", key.c_str());
            continue;
        }
        ExecuteInternal(value, key, eventInfo);
    }

    JSRef<JSVal> paramObj = JSRef<JSVal>::Cast(eventInfo);
    ExecuteJS(1, &paramObj);
}

void JsFunctionBase::ExecuteInternal(const std::unique_ptr<JsonValue>& value, const std::string& key,
    const JSRef<JSObject>& eventInfo)
{
    if (value->IsString()) {
        eventInfo->SetProperty<std::string>(key.c_str(), value->GetString().c_str());
    } else if (value->IsNumber()) {
        eventInfo->SetProperty<double>(key.c_str(), value->GetDouble());
    } else if (value->IsBool()) {
        eventInfo->SetProperty<bool>(key.c_str(), value->GetBool());
    } else if (value->IsObject()) {
        eventInfo->SetPropertyJsonObject(key.c_str(), value->ToString().c_str());
    } else if (value->IsArray()) {
        JSRef<JSArray> valueArray = JSRef<JSArray>::New();
        for (auto index = 0; index < value->GetArraySize(); index++) {
            auto item = value->GetArrayItem(index);
            if (item && item->IsString()) {
                valueArray->SetValueAt(index, JSRef<JSVal>::Make(ToJSValue(item->GetString())));
            }
            if (item && item->IsNumber()) {
                valueArray->SetValueAt(index, JSRef<JSVal>::Make(ToJSValue(item->GetInt())));
            }
        }
        eventInfo->SetPropertyObject(key.c_str(), valueArray);
    }
}

void JsFunctionBase::ExecuteNew(const std::vector<std::string>& keys, const std::string& param)
{
    JSRef<JSVal> jsVal;
    if (keys.size() > 1) {
        auto result = XComponentClient::GetInstance().GetJSVal(keys[1], jsVal);
        RefPtr<JSXComponentController> controller =
            XComponentClient::GetInstance().GetControllerFromJSXComponentControllersMap(keys[1]);
        if (result && controller) {
            controller->SetXComponentContext(jsVal);
        }
    }
    ExecuteJS(1, &jsVal);
}

JSRef<JSVal> JsWeakFunction::ExecuteJS(int argc, JSRef<JSVal> argv[], bool isAnimation)
{
    int32_t id = -1;
    if (SystemProperties::GetAcePerformanceMonitorEnabled()) {
        id = Container::CurrentId();
    }
    JS_CALLBACK_DURATION(id);
    JAVASCRIPT_EXECUTION_SCOPE_STATIC
    ACE_FUNCTION_TRACE();
    JSRef<JSVal> jsObject = jsThis_.Lock();
    auto jsFunction = jsWeakFunction_.Lock();
    if (jsFunction.IsEmpty()) {
        LOGW("js function is null.");
        return {};
    }
    JSRef<JSVal> result = jsFunction->Call(jsObject, argc, argv);
    return result;
}

#ifdef USE_ARK_ENGINE
JSRef<JSVal> JsWeakFunction::ExecuteJSWithContext(
    int argc, JSRef<JSVal> argv[], const JSExecutionContext& context, bool isAnimation)
{
    int32_t id = -1;
    if (SystemProperties::GetAcePerformanceMonitorEnabled()) {
        id = Container::CurrentId();
    }
    JS_CALLBACK_DURATION(id);
    JAVASCRIPT_EXECUTION_SCOPE(context);
    ACE_FUNCTION_TRACE();
    JSRef<JSVal> jsObject = jsThis_.Lock();
    auto jsFunction = jsWeakFunction_.Lock();
    if (jsFunction.IsEmpty()) {
        LOGW("js function is null.");
        return {};
    }
    JSRef<JSVal> result = jsFunction->Call(jsObject, argc, argv);
    return result;
}

JSRef<JSVal> JsFunction::ExecuteJSWithContext(
    int argc, JSRef<JSVal> argv[], const JSExecutionContext& context, bool isAnimation)
{
    int32_t id = -1;
    if (SystemProperties::GetAcePerformanceMonitorEnabled()) {
        id = Container::CurrentId();
    }
    JS_CALLBACK_DURATION(id);
    JAVASCRIPT_EXECUTION_SCOPE(context);
    ACE_FUNCTION_TRACE();

    JSRef<JSVal> jsObject = jsThis_.Lock();
    JSRef<JSVal> result = jsFunction_->Call(jsObject, argc, argv, isAnimation);
    return result;
}
#endif

JSRef<JSVal> JsFunction::ExecuteJS(int argc, JSRef<JSVal> argv[], bool isAnimation)
{
    int32_t id = -1;
    if (SystemProperties::GetAcePerformanceMonitorEnabled()) {
        id = Container::CurrentId();
    }
    JS_CALLBACK_DURATION(id);
    JAVASCRIPT_EXECUTION_SCOPE_STATIC
    ACE_FUNCTION_TRACE();

    JSRef<JSVal> jsObject = jsThis_.Lock();
    JSRef<JSVal> result = jsFunction_->Call(jsObject, argc, argv, isAnimation);
    return result;
}

JSRef<JSObject> CreateEventTargetObject(const BaseEventInfo& info)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    JSRef<JSObject> target = objectTemplate->NewInstance();
    JSRef<JSObject> area = objectTemplate->NewInstance();
    JSRef<JSObject> offset = objectTemplate->NewInstance();
    JSRef<JSObject> globalOffset = objectTemplate->NewInstance();
    const auto& localOffset = info.GetTarget().area.GetOffset();
    const auto& origin = info.GetTarget().origin;
    offset->SetProperty<double>("x", localOffset.GetX().ConvertToVp());
    offset->SetProperty<double>("y", localOffset.GetY().ConvertToVp());
    globalOffset->SetProperty<double>("x", (origin.GetX().ConvertToVp() + localOffset.GetX().ConvertToVp()));
    globalOffset->SetProperty<double>("y", (origin.GetY().ConvertToVp() + localOffset.GetY().ConvertToVp()));
    area->SetPropertyObject("position", offset);
    area->SetPropertyObject("globalPosition", globalOffset);
    area->SetProperty<double>("width", info.GetTarget().area.GetWidth().ConvertToVp());
    area->SetProperty<double>("height", info.GetTarget().area.GetHeight().ConvertToVp());
    target->SetPropertyObject("area", area);
    if (info.GetTarget().id.empty()) {
        target->SetPropertyObject("id", JsiValue::Undefined());
    } else {
        target->SetProperty<const char*>("id", info.GetTarget().id.c_str());
    }
    return target;
}

} // namespace OHOS::Ace::Framework
