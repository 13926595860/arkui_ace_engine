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

#include "bridge/declarative_frontend/jsview/js_radio.h"

#include "base/log/ace_scoring_log.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/radio_model_impl.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/pattern/radio/radio_model_ng.h"

namespace OHOS::Ace {
namespace {
constexpr int FOR_DIMENSION_BOX_CALCULATE_MULTIPLY_TWO = 2;
} // namespace

std::unique_ptr<RadioModel> RadioModel::instance_ = nullptr;
std::mutex RadioModel::mutex_;

RadioModel* RadioModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::RadioModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::RadioModelNG());
            } else {
                instance_.reset(new Framework::RadioModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace
namespace OHOS::Ace::Framework {

void JSRadio::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("radio create error, info is not-valid");
        return;
    }

    std::optional<std::string> value;
    std::optional<std::string> group;
    if ((info.Length() >= 1) && info[0]->IsObject()) {
        auto paramObject = JSRef<JSObject>::Cast(info[0]);
        auto valueTemp = paramObject->GetProperty("value");
        auto groupTemp = paramObject->GetProperty("group");
        if (valueTemp->IsString()) {
            value = valueTemp->ToString();
        }
        if (groupTemp->IsString()) {
            group = groupTemp->ToString();
        }
    }
    RadioModel::GetInstance()->Create(value, group);
}

void JSRadio::JSBind(BindingTarget globalObj)
{
    JSClass<JSRadio>::Declare("Radio");

    JSClass<JSRadio>::StaticMethod("create", &JSRadio::Create);
    JSClass<JSRadio>::StaticMethod("checked", &JSRadio::Checked);
    JSClass<JSRadio>::StaticMethod("width", &JSRadio::JsWidth);
    JSClass<JSRadio>::StaticMethod("height", &JSRadio::JsHeight);
    JSClass<JSRadio>::StaticMethod("size", &JSRadio::JsSize);
    JSClass<JSRadio>::StaticMethod("padding", &JSRadio::JsPadding);
    JSClass<JSRadio>::StaticMethod("radioStyle", &JSRadio::JsRadioStyle);
    JSClass<JSRadio>::StaticMethod("onChange", &JSRadio::OnChange);
    JSClass<JSRadio>::StaticMethod("onClick", &JSRadio::JsOnClick);
    JSClass<JSRadio>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSRadio>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSRadio>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSRadio>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSRadio>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSRadio>::Inherit<JSViewAbstract>();
    JSClass<JSRadio>::Bind<>(globalObj);
}

void JSRadio::Checked(bool checked)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    if (checked) {
        radioComponent->SetGroupValue(radioComponent->GetValue());
        radioComponent->SetOriginChecked(checked);
    } else {
        radioComponent->SetGroupValue("");
    }
}

void ParseCheckedObject(const JSCallbackInfo& args, const JSRef<JSVal>& changeEventVal)
{
    CHECK_NULL_VOID(changeEventVal->IsFunction());

    auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(changeEventVal));
    auto onChecked = [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)](bool check) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Radio.onChangeEvent");
        auto newJSVal = JSRef<JSVal>::Make(ToJSValue(check));
        func->ExecuteJS(1, &newJSVal);
    };
    RadioModel::GetInstance()->SetOnChangeEvent(std::move(onChecked));
}

void JSRadio::Checked(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || info.Length() > 2) {
        LOGE("The arg is wrong, it is supposed to have 1 or 2 arguments");
        return;
    }

    if (info.Length() > 0 && info[0]->IsBoolean()) {
        RadioModel::GetInstance()->SetChecked(info[0]->ToBoolean());
    } else {
        RadioModel::GetInstance()->SetChecked(false);
    }

    if (info.Length() > 1 && info[1]->IsObject()) {
        JSRef<JSVal> checkedObj = JSRef<JSObject>::Cast(info[1]);
        ParseCheckedObject(info, checkedObj);
    }

    if (info.Length() > 1 && info[1]->IsObject()) {
        JSRef<JSVal> checkedObj = JSRef<JSObject>::Cast(info[1]);
        ParseCheckedObject(info, checkedObj);
    }
}

void JSRadio::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSRadio::JsWidth(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        NG::ViewAbstract::SetWidth(NG::CalcLength(value));
        return;
    }

    RadioModel::GetInstance()->SetWidth(value);
}

void JSRadio::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSRadio::JsHeight(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        NG::ViewAbstract::SetHeight(NG::CalcLength(value));
        return;
    }

    RadioModel::GetInstance()->SetHeight(value);
}

void JSRadio::JsSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JsWidth(sizeObj->GetProperty("width"));
    JsHeight(sizeObj->GetProperty("height"));
}

void JSRadio::JsPadding(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        JSViewAbstract::JsPadding(info);
        return;
    }

    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("top") || argsPtrItem->Contains("bottom") || argsPtrItem->Contains("left") ||
            argsPtrItem->Contains("right")) {
            CalcDimension topDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension leftDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension rightDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension bottomDimen = CalcDimension(0.0, DimensionUnit::VP);
            ParseJsonDimensionVp(argsPtrItem->GetValue("top"), topDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("left"), leftDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("right"), rightDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("bottom"), bottomDimen);
            if (leftDimen == 0.0_vp) {
                leftDimen = rightDimen;
            }
            if (topDimen == 0.0_vp) {
                topDimen = bottomDimen;
            }
            if (leftDimen == 0.0_vp) {
                leftDimen = topDimen;
            }
            NG::PaddingPropertyF padding;
            padding.left = leftDimen.ConvertToPx();
            padding.right = rightDimen.ConvertToPx();
            padding.top = topDimen.ConvertToPx();
            padding.bottom = bottomDimen.ConvertToPx();
            RadioModel::GetInstance()->SetPadding(padding);
            return;
        }
    }
    CalcDimension length;
    if (!ParseJsDimensionVp(info[0], length)) {
        return;
    }
    NG::PaddingPropertyF padding;
    padding.left = length.ConvertToPx();
    padding.right = length.ConvertToPx();
    padding.top = length.ConvertToPx();
    padding.bottom = length.ConvertToPx();
    RadioModel::GetInstance()->SetPadding(padding);
}

void JSRadio::JsRadioStyle(const JSCallbackInfo& info)
{
    if (info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> checkedBackgroundColor = obj->GetProperty("checkedBackgroundColor");
        JSRef<JSVal> uncheckedBorderColor = obj->GetProperty("uncheckedBorderColor");
        JSRef<JSVal> indicatorColor = obj->GetProperty("indicatorColor");
        Color checkedBackgroundColorVal;
        auto theme = GetTheme<RadioTheme>();
        if (!ParseJsColor(checkedBackgroundColor, checkedBackgroundColorVal)) {
            checkedBackgroundColorVal = theme->GetActiveColor();
        }
        RadioModel::GetInstance()->SetCheckedBackgroundColor(checkedBackgroundColorVal);
        Color uncheckedBorderColorVal;
        if (!ParseJsColor(uncheckedBorderColor, uncheckedBorderColorVal)) {
            uncheckedBorderColorVal = theme->GetInactiveColor();
        }
        RadioModel::GetInstance()->SetUncheckedBorderColor(uncheckedBorderColorVal);
        Color indicatorColorVal;
        if (!ParseJsColor(indicatorColor, indicatorColorVal)) {
            indicatorColorVal = theme->GetPointColor();
        }
        RadioModel::GetInstance()->SetIndicatorColor(indicatorColorVal);
    }
}

void JSRadio::SetPadding(const CalcDimension& topDimen, const CalcDimension& leftDimen)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();

    if (radioComponent) {
        auto width = radioComponent->GetWidth();
        auto height = radioComponent->GetHeight();
        radioComponent->SetHeight(height);
        radioComponent->SetWidth(width);
        box->SetHeight(height + topDimen * FOR_DIMENSION_BOX_CALCULATE_MULTIPLY_TWO);
        box->SetWidth(width + leftDimen * FOR_DIMENSION_BOX_CALCULATE_MULTIPLY_TWO);
        radioComponent->SetHotZoneVerticalPadding(topDimen);
        radioComponent->SetHorizontalPadding(leftDimen);
    }
}

void JSRadio::OnChange(const JSCallbackInfo& args)
{
    if (!args[0]->IsFunction()) {
        return;
    }
    auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(args[0]));
    auto onChange = [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)](bool check) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Radio.onChange");
        auto newJSVal = JSRef<JSVal>::Make(ToJSValue(check));
        func->ExecuteJS(1, &newJSVal);
    };
    RadioModel::GetInstance()->SetOnChange(std::move(onChange));
    args.ReturnSelf();
}

void JSRadio::JsOnClick(const JSCallbackInfo& args)
{
    if (Container::IsCurrentUseNewPipeline()) {
        JSViewAbstract::JsOnClick(args);
        return;
    }

    if (JSViewBindEvent(&CheckableComponent::SetOnClick, args)) {
    } else {
        LOGW("Failed to bind event");
    }

    args.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
