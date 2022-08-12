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

#include "frameworks/bridge/declarative_frontend/jsview/js_tab_content.h"

#include "base/log/ace_trace.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr char DEFAULT_TAB_BAR_NAME[] = "TabBar";

} // namespace

void JSTabContent::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> components;
    auto tabContentItemComponent = AceType::MakeRefPtr<V2::TabContentItemComponent>(components);
    tabContentItemComponent->SetCrossAxisSize(CrossAxisSize::MAX);
    ViewStackProcessor::GetInstance()->ClaimElementId(tabContentItemComponent);

    RefPtr<V2::TabsComponent>  tabsComponent = nullptr;

    bool usePartialUpdate = Container::IsCurrentUsePartialUpdate();

    if (!usePartialUpdate) {
        tabsComponent = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetTopTabs());
        if (!tabsComponent) {
            LOGE("fail to create tab content due to tabs missing");
            return;
        }
        // GetTabsComponent used only by JSTabContent::SetTabBar
        // To Find TabBarComponent eventually
        tabContentItemComponent->SetTabsComponent(AceType::WeakClaim(AceType::RawPtr(tabsComponent)));

        auto tabBar = tabsComponent->GetTabBarChild();
        tabBar->AppendChild(CreateTabBarLabelComponent(tabContentItemComponent, std::string(DEFAULT_TAB_BAR_NAME)));
    }

    ViewStackProcessor::GetInstance()->Push(tabContentItemComponent);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (box) {
        box->SetBoxClipFlag(true);
    }

    if (usePartialUpdate && info.Length() > 0 && info[0]->IsFunction()) {
        JSRef<JSVal> builderFunctionJS = info[0];
        LOGD("We have a build function for a tab");
        auto jsWrapperFunc =
            [context = info.GetExecutionContext(), builder = builderFunctionJS]() -> RefPtr<Component> {
            JAVASCRIPT_EXECUTION_SCOPE(context)
            JSRef<JSFunc>::Cast(builder)->Call(JSRef<JSObject>());
            return ViewStackProcessor::GetInstance()->Finish();
        };
        tabContentItemComponent->SetBuilder(jsWrapperFunc);
        return;
    }
    LOGE("No build function for a tab provided");
}

void JSTabContent::SetTabBar(const JSCallbackInfo& info)
{
    auto tabContentItemComponent =
        AceType::DynamicCast<V2::TabContentItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!tabContentItemComponent) {
        return;
    }
    auto weakTabs = tabContentItemComponent->GetTabsComponent();

    RefPtr<V2::TabsComponent> tabs;
    RefPtr<TabBarComponent> tabBar;

    if (!Container::IsCurrentUsePartialUpdate()) {
        tabs = weakTabs.Upgrade();
        if (!tabs) {
            LOGE("can not get Tabs parent component error.");
            return;
        }
        tabBar = tabs->GetTabBarChild();
        if (!tabBar) {
            LOGE("can not get TabBar component error.");
            return;
        }
    }

    RefPtr<Component> tabBarChild = nullptr;
    std::string infoStr;
    if (ParseJsString(info[0], infoStr)) {
        auto textVal = infoStr.empty() ? DEFAULT_TAB_BAR_NAME : infoStr;
        if (!Container::IsCurrentUsePartialUpdate()) {
            tabBarChild = CreateTabBarLabelComponent(tabContentItemComponent, textVal);
        } else {
            tabContentItemComponent->SetBarText(textVal);
        }
    } else {
        auto paramObject = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> builderFuncParam = paramObject->GetProperty("builder");
        JSRef<JSVal> textParam = paramObject->GetProperty("text");
        JSRef<JSVal> iconParam = paramObject->GetProperty("icon");
        auto isTextEmpty = textParam->IsEmpty() || textParam->IsUndefined() || textParam->IsNull();
        auto isIconEmpty = iconParam->IsEmpty() || iconParam->IsUndefined() || iconParam->IsNull();

        if (builderFuncParam->IsFunction()) {
            tabBarChild = ProcessTabBarBuilderFunction(tabContentItemComponent, builderFuncParam);
            if (!Container::IsCurrentUsePartialUpdate()) {
                tabBar->ResetIndicator();
                tabBar->SetAlignment(Alignment::TOP_LEFT);
            }
        } else if (!isTextEmpty && !isIconEmpty) {
            tabBarChild = ProcessTabBarTextIconPair(tabContentItemComponent, textParam, iconParam);
        } else if (!isTextEmpty && isIconEmpty) {
            tabBarChild = ProcessTabBarLabel(tabContentItemComponent, textParam);
        }
    }

    if (!Container::IsCurrentUsePartialUpdate()) {
        auto defaultTabChild = tabBar->GetChildren().back();
        tabBar->RemoveChildDirectly(defaultTabChild);
        tabBar->AppendChild(tabBarChild);
        return;
    }

    // Partial Update only
    if (tabContentItemComponent->GetBarElementId() == ElementRegister::UndefinedElementId) {
        const auto id = ElementRegister::GetInstance()->MakeUniqueId();
        tabContentItemComponent->SetBarElementId(id);
        LOGD("Setting ID for tab bar item to %{public}d tabContentItemComponent id %{public}d",
            id, tabContentItemComponent->GetBarElementId());
    }
}

RefPtr<Component> JSTabContent::ProcessTabBarBuilderFunction(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSObject> builderFunc)
{
    tabContent->SetBarText("custom");
    ScopedViewStackProcessor builderViewStackProcessor;
    JsFunction jsBuilderFunc(builderFunc);
    ACE_SCORING_EVENT("TabContent.tabBarBuilder");
    jsBuilderFunc.Execute();
    RefPtr<Component> builderGeneratedRootComponent = ViewStackProcessor::GetInstance()->Finish();
    return builderGeneratedRootComponent;
}

RefPtr<Component> JSTabContent::CreateTabBarLabelComponent(
    RefPtr<V2::TabContentItemComponent>& tabContent, const std::string& labelStr)
{
    tabContent->SetBarText(labelStr);
    return TabBarItemComponent::BuildWithTextIcon(labelStr, std::string());
}

RefPtr<Component> JSTabContent::ProcessTabBarLabel(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> labelVal)
{
    std::string textStr;
    if (!ParseJsString(labelVal, textStr)) {
        textStr = DEFAULT_TAB_BAR_NAME;
    }
    tabContent->SetBarText(textStr);

    if (!Container::IsCurrentUsePartialUpdate()) {
        return CreateTabBarLabelComponent(tabContent, textStr);
    }
    return nullptr;
}

RefPtr<Component> JSTabContent::ProcessTabBarTextIconPair(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> textVal, JSRef<JSVal> iconVal)
{
    std::string iconUri;
    if (!ParseJsMedia(iconVal, iconUri)) {
        return ProcessTabBarLabel(tabContent, textVal);
    }
    std::string textStr;
    if (!ParseJsString(textVal, textStr)) {
        textStr = DEFAULT_TAB_BAR_NAME;
    }
    tabContent->SetBarText(textStr);
    tabContent->SetBarIcon(iconUri);

    if (!Container::IsCurrentUsePartialUpdate()) {
        return TabBarItemComponent::BuildWithTextIcon(textStr, iconUri);
    }
    return nullptr;
}

void JSTabContent::JSBind(BindingTarget globalObj)
{
    JSClass<JSTabContent>::Declare("TabContent");
    JSClass<JSTabContent>::StaticMethod("create", &JSTabContent::Create);
    JSClass<JSTabContent>::StaticMethod("tabBar", &JSTabContent::SetTabBar);
    JSClass<JSTabContent>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTabContent>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTabContent>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTabContent>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSTabContent>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTabContent>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTabContent>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTabContent>::StaticMethod("width", &JSTabContent::SetTabContentWidth);
    JSClass<JSTabContent>::StaticMethod("height", &JSTabContent::SetTabContentHeight);
    JSClass<JSTabContent>::StaticMethod("size", &JSTabContent::SetTabContentSize);
    JSClass<JSTabContent>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSTabContent>::Inherit<JSContainerBase>();
    JSClass<JSTabContent>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
