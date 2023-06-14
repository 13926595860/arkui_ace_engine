/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/container_modal/enhance/container_modal_view_enhance.h"

#include "base/geometry/ng/offset_t.h"
#include "base/i18n/localization.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/gestures/pan_gesture.h"
#include "core/components_ng/gestures/tap_gesture.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/container_modal/enhance/container_modal_pattern_enhance.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/list/list_pattern.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/calc_length.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/divider/divider_layout_property.h"
#include "core/event/mouse_event.h"
#include "core/components_ng/base/frame_node.h"
#include "base/memory/ace_type.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/menu/menu_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_declaration.h"
#include "core/components_ng/pattern/patternlock/patternlock_paint_property.h"
#include "core/image/image_source_info.h"
#include "core/pipeline/base/element_register.h"

namespace OHOS::Ace::NG {
/**
 * The structure of container_modal enhanced is designed as follows :
 * |--container_modal(stack)
 *   |--column
 *      |--container_modal_title(row)
 *          |--icon(image), label(text), [maxRecover, minimize, close](button)
 *      |--stack
 *          |--container_modal_content(stage)
 *              |--page
 *          |--dialog(when show)
 *   |--container_modal_floating_title(row)
 *          |--icon(image), label(text), [maxRecover, minimize, close](button)
 */
namespace {
const Dimension MENU_CONTAINER_WIDTH = 240.0_vp;
const Dimension MENU_CONTAINER_HEIGHT = 209.0_vp;
const Dimension MENU_CONTAINER_DIVIDER_HEIGHT = 9.0_vp;
const Dimension MENU_CONTAINER_DIVIDER_STROKE_HEIGHT = 1.0_vp;
const Dimension MENU_ITEM_RADIUS = 8.0_vp;
const Dimension MENU_ITEM_WIDTH = 232.0_vp;
const Dimension MENU_ITEM_HEIGHT = 48.0_vp;
const Dimension MENU_ITEM_LEFT_PADDING = 12.0_vp;
const Dimension MENU_ITEM_TEXT_WIDTH = 144.0_vp;
const Dimension MENU_ITEM_TEXT_HEIGHT = 22.0_vp;
const Dimension MENU_ITEM_TEXT_PADDING = 8.0_vp;
const Dimension MENU_FLOAT_X = 226.0_vp;
const Dimension MENU_FLOAT_Y = 28.0_vp;
const int32_t MENU_ITEM_MAXLINES = 1;
const int32_t MENU_TASK_DELAY_TIME = 1000;
const Color MENU_ITEM_CHOOSE_COLOR = Color(0x0c000000);
}
bool ContainerModalViewEnhance::sIsHovering = false;
bool ContainerModalViewEnhance::sIsMenuPending_ = false;
CancelableCallback<void()> ContainerModalViewEnhance::sContextTimer_;

RefPtr<FrameNode> ContainerModalViewEnhance::Create(RefPtr<FrameNode>& content)
{
    auto containerModalNode = FrameNode::CreateFrameNode("ContainerModal",
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ContainerModalPatternEnhance>());
    containerModalNode->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
    auto stack = FrameNode::CreateFrameNode(V2::STACK_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<StackPattern>());
    auto column = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));

    column->AddChild(BuildTitle(containerModalNode));
    stack->AddChild(content);
    column->AddChild(stack);
    content->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_CONTENT);
    content->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(1.0, DimensionUnit::PERCENT), CalcLength(1.0, DimensionUnit::PERCENT)));
    containerModalNode->AddChild(column);
    containerModalNode->AddChild(BuildTitle(containerModalNode, true));

    CHECK_NULL_RETURN(stack->GetLayoutProperty(), nullptr);
    stack->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
    CHECK_NULL_RETURN(column->GetLayoutProperty(), nullptr);
    column->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);

    auto containerPattern = containerModalNode->GetPattern<ContainerModalPatternEnhance>();
    CHECK_NULL_RETURN(containerPattern, nullptr);
    containerModalNode->MarkModifyDone();
    containerPattern->InitContainerEvent();
    return containerModalNode;
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildTitle(RefPtr<FrameNode>& containerNode, bool isFloatingTitle)
{
    LOGI("ContainerModalViewEnhance BuildTitle called");
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    auto titleContainer = BuildTitleContainer(containerNode, isFloatingTitle);
    CHECK_NULL_RETURN(titleContainer, nullptr);
    if (!isFloatingTitle) {
        auto eventHub = titleContainer->GetOrCreateGestureEventHub();
        auto tapGesture = AceType::MakeRefPtr<NG::TapGesture>(2, 1);
        tapGesture->SetOnActionId([containerNode, windowManager](GestureEvent& info) {
            LOGD("container window double click.");
            auto mode = windowManager->GetWindowMode();
            if (mode == WindowMode::WINDOW_MODE_FLOATING) {
                if (windowManager->GetCurrentWindowMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
                    LOGD("double click to recover");
                    windowManager->WindowRecover();
                } else {
                    LOGD("double click to maximize");
                    windowManager->WindowMaximize(true);
                }
                containerNode->OnWindowFocused();
            }
        });
        eventHub->AddGesture(tapGesture);
        eventHub->OnModifyDone();
    }
    return AddControlButtons(containerNode, titleContainer);
}

RefPtr<FrameNode> ContainerModalViewEnhance::AddControlButtons(RefPtr<FrameNode>& containerNode,
    RefPtr<FrameNode>& containerTitleRow)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    RefPtr<FrameNode> maximizeBtn = BuildControlButton(InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MAXIMIZE,
        {});
    // maximizeBtn add empty panEvent to over fater container event
    auto maximizeBtnEventHub = maximizeBtn->GetOrCreateGestureEventHub();
    auto panEvent = AceType::MakeRefPtr<PanEvent>(nullptr, nullptr, nullptr, nullptr);
    PanDirection panDirection;
    panDirection.type = PanDirection::ALL;
    maximizeBtnEventHub->AddPanEvent(panEvent, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
    BondingMaxBtnEvent(maximizeBtn, containerNode);
    containerTitleRow->AddChild(maximizeBtn);
    
    RefPtr<FrameNode> minimizeBtn = BuildControlButton(InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MINIMIZE,
        [windowManager] (GestureEvent& info) {
            CHECK_NULL_VOID(windowManager);
            LOGI("minimize button clicked");
            windowManager->WindowMinimize();
        });
    // minimizeBtn add empty panEvent to over fater container event
    auto minimizeBtnEventHub = minimizeBtn->GetOrCreateGestureEventHub();
    minimizeBtnEventHub->AddPanEvent(panEvent, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
    containerTitleRow->AddChild(minimizeBtn);

    RefPtr<FrameNode> closeBtn = BuildControlButton(
        InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_CLOSE,
        [windowManager](GestureEvent& info) {
            CHECK_NULL_VOID(windowManager);
            LOGI("close button clicked");
            windowManager->WindowClose();
        }, true);
    // closeBtn add empty panEvent to over fater container event
    auto closeBtnEventHub = closeBtn->GetOrCreateGestureEventHub();
    closeBtnEventHub->AddPanEvent(panEvent, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
    containerTitleRow->AddChild(closeBtn);

    return containerTitleRow;
}

void ContainerModalViewEnhance::BondingMaxBtnEvent(RefPtr<FrameNode>& maximizeBtn, RefPtr<FrameNode>& containerNode)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    auto windowManager = pipeline->GetWindowManager();
    auto event = [containerNode, windowManager](GestureEvent &info) {
        ResetHoverTimer();
        auto mode = windowManager->GetWindowMode();
        auto currentMode = windowManager->GetCurrentWindowMaximizeMode();
        if (mode == WindowMode::WINDOW_MODE_FULLSCREEN || currentMode== MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
            LOGD("recover button clicked");
            windowManager->WindowRecover();
        } else {
            LOGD("maximize button clicked");
            windowManager->WindowMaximize(true);
        }
        containerNode->OnWindowFocused();
    };
    auto hub = maximizeBtn->GetOrCreateGestureEventHub();
    hub->AddClickEvent(AceType::MakeRefPtr<ClickEvent>(std::move(event)));
    auto hoverEventHub = maximizeBtn->GetOrCreateInputEventHub();
    auto hoverMoveFuc = [maximizeBtn, pipeline](MouseInfo& info) {
            LOGD("container window on hover event action_ = %{public}d sIsMenuPending_ %{public}d",
                info.GetAction(), sIsMenuPending_);
            if (!sIsMenuPending_ && info.GetAction() == MouseAction::MOVE) {
                auto&& callback = [maximizeBtn, info]() {
                    ShowMaxMenu(maximizeBtn, info);
                };
                sContextTimer_.Reset(callback);
                pipeline->GetTaskExecutor()->PostDelayedTask(sContextTimer_, TaskExecutor::TaskType::UI,
                    MENU_TASK_DELAY_TIME);
                sIsMenuPending_ = true;
            }
        };
    hoverEventHub->AddOnMouseEvent(AceType::MakeRefPtr<InputEvent>(std::move(hoverMoveFuc)));

    // add hover in out event
    auto hoverEventFuc = [maximizeBtn, pipeline](bool hover, HoverInfo& hoverInfo) {
        // update container modal background
        if (hover) {
            sIsHovering = true;
        } else {
            ResetHoverTimer();
        }
    };
    hoverEventHub->AddOnHoverEvent(AceType::MakeRefPtr<InputEvent>(std::move(hoverEventFuc)));
}

RefPtr<FrameNode> ContainerModalViewEnhance::ShowMaxMenu(const RefPtr<FrameNode>& targetNode, const MouseInfo& info)
{
    LOGD("ShowMaxMenu called");
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);

    // menu list
    auto menuList = FrameNode::CreateFrameNode(
        V2::LIST_COMPONENT_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ListPattern>());
    auto listLayoutProperty = menuList->GetLayoutProperty<ListLayoutProperty>();
    CHECK_NULL_RETURN(listLayoutProperty, nullptr);
    listLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(MENU_CONTAINER_WIDTH),
        CalcLength(MENU_CONTAINER_HEIGHT)));
    menuList->AddChild(BuildMaximizeMenuItem());
    menuList->AddChild(BuildFullScreenMenuItem());
    menuList->AddChild(BuildDividerMenuItem());
    menuList->AddChild(BuildLeftSplitMenuItem());
    menuList->AddChild(BuildRightSplitMenuItem());

    auto menuPosX = info.GetScreenLocation().GetX() - info.GetLocalLocation().GetX() - MENU_FLOAT_X.ConvertToPx();
    auto menuPosY = info.GetScreenLocation().GetY() - info.GetLocalLocation().GetY() + MENU_FLOAT_Y.ConvertToPx();
    OffsetF menuPosition {menuPosX, menuPosY};

    if ((!SubwindowManager::GetInstance()->GetCurrentWindow()
        || !SubwindowManager::GetInstance()->GetCurrentWindow()->GetShown())) {
        MenuParam menu {};
        ViewAbstract::BindMenuWithCustomNode(menuList, targetNode, true, menuPosition, menu);
    }
    ResetHoverTimer();
    return menuList;
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildMaximizeMenuItem()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    // click maxize Item Event
    auto maximizeClickFunc = [windowManager](GestureEvent &info) {
        CHECK_NULL_VOID_NOLOG(windowManager);
        LOGD("MODE_MAXIMIZE selected");
        ResetHoverTimer();
        if (MaximizeMode::MODE_AVOID_SYSTEM_BAR == windowManager->GetCurrentWindowMaximizeMode()) {
            windowManager->WindowRecover();
        } else {
            windowManager->SetWindowMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
            windowManager->WindowMaximize(true);
        }
    };
    auto maximizeEvent = AceType::MakeRefPtr<ClickEvent>(std::move(maximizeClickFunc));
    auto curMaxMode = windowManager->GetCurrentWindowMaximizeMode();
    auto maximizeTitle = Localization::GetInstance()->GetEntryLetters(
        curMaxMode == MaximizeMode::MODE_AVOID_SYSTEM_BAR ? "window.exitMaximize"
        : "window.maximize");
    auto maximizeRow = BuildMenuItem(maximizeTitle, InternalResource::ResourceId::IC_WINDOW_MENU_MAXIMIZE,
        maximizeEvent, windowManager->GetWindowMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    PaddingProperty pad;
    pad.left = CalcLength(CONTENT_PADDING);
    return BuildMenuItemPadding(pad, maximizeRow);
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildFullScreenMenuItem()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    auto fullScreenClickFunc = [windowManager](GestureEvent &info) {
        CHECK_NULL_VOID_NOLOG(windowManager);
        ResetHoverTimer();
        LOGD("MODE_FULLSCREEN selected");
        if (MaximizeMode::MODE_FULL_FILL == windowManager->GetCurrentWindowMaximizeMode()) {
            windowManager->WindowRecover();
        } else {
            windowManager->SetWindowMaximizeMode(MaximizeMode::MODE_FULL_FILL);
            windowManager->WindowMaximize(true);
        }
    };
    auto fullScreenEvent = AceType::MakeRefPtr<ClickEvent>(std::move(fullScreenClickFunc));
    auto curMaxMode = windowManager->GetCurrentWindowMaximizeMode();
    auto fullScreenTitle = Localization::GetInstance()->GetEntryLetters(curMaxMode == MaximizeMode::MODE_FULL_FILL ?
        "window.exitFullScreen" : "window.fullScreen");
    auto fullScreenRow = BuildMenuItem(fullScreenTitle, InternalResource::ResourceId::IC_WINDOW_MENU_FULLSCREEN,
        fullScreenEvent, windowManager->GetWindowMaximizeMode() == MaximizeMode::MODE_FULL_FILL);
    PaddingProperty pad;
    pad.left = CalcLength(CONTENT_PADDING);
    return BuildMenuItemPadding(pad, fullScreenRow);
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildDividerMenuItem()
{
    auto divider = FrameNode::CreateFrameNode(V2::DIVIDER_COMPONENT_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<DividerPattern>());
    auto dividerLayout = divider->GetLayoutProperty<DividerLayoutProperty>();
    dividerLayout->UpdateStrokeWidth(MENU_CONTAINER_DIVIDER_STROKE_HEIGHT);
    dividerLayout->UpdateUserDefinedIdealSize(CalcSize(CalcLength(MENU_CONTAINER_WIDTH),
        CalcLength(MENU_CONTAINER_DIVIDER_HEIGHT)));
    PaddingProperty pad;
    pad.left = CalcLength(CONTENT_PADDING);
    return BuildMenuItemPadding(pad, divider);
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildLeftSplitMenuItem()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    auto leftSplitClickFunc = [windowManager](GestureEvent &info) {
        CHECK_NULL_VOID_NOLOG(windowManager);
        LOGD("left split selected");
        windowManager->FireWindowSplitCallBack();
    };
    auto leftSplitEvent = AceType::MakeRefPtr<ClickEvent>(std::move(leftSplitClickFunc));
    auto screenLeftRow = BuildMenuItem(Localization::GetInstance()->GetEntryLetters("window.leftSide"),
        InternalResource::ResourceId::IC_WINDOW_MENU_SCREEN_L, leftSplitEvent, false);
    PaddingProperty pad;
    pad.left = CalcLength(CONTENT_PADDING);
    return BuildMenuItemPadding(pad, screenLeftRow);
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildRightSplitMenuItem()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);
    auto rightSplitClickFunc = [windowManager](GestureEvent &info) {
        CHECK_NULL_VOID_NOLOG(windowManager);
        LOGI("right split selected");
        windowManager->FireWindowSplitCallBack(false);
    };
    auto rightSplitEvent = AceType::MakeRefPtr<ClickEvent>(std::move(rightSplitClickFunc));
    auto screenRightRow = BuildMenuItem(Localization::GetInstance()->GetEntryLetters("window.rightSide"),
        InternalResource::ResourceId::IC_WINDOW_MENU_SCREEN_N, rightSplitEvent, false);
    PaddingProperty pad;
    pad.left = CalcLength(CONTENT_PADDING);
    return BuildMenuItemPadding(pad, screenRightRow);
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildMenuItem(std::string title, InternalResource::ResourceId resourceId,
    RefPtr<ClickEvent> event, bool chooseCurrent)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_RETURN(windowManager, nullptr);

    // padding+pic+padding+text+padding+
    auto containerTitleRow = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    // setRadius 8vp
    auto render = containerTitleRow->GetRenderContext();
    BorderRadiusProperty borderRadiusProperty;
    borderRadiusProperty.SetRadius(MENU_ITEM_RADIUS);
    render->UpdateBorderRadius(borderRadiusProperty);
    // 232 48  leftPadding 4vp
    auto layoutProperty = containerTitleRow->GetLayoutProperty<LinearLayoutProperty>();
    layoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(MENU_ITEM_WIDTH), CalcLength(MENU_ITEM_HEIGHT)));
    layoutProperty->UpdateMainAxisAlign(FlexAlign::FLEX_START);
    layoutProperty->UpdateCrossAxisAlign(FlexAlign::CENTER);
    PaddingProperty rowLeftPadding;
    rowLeftPadding.left = CalcLength(MENU_ITEM_LEFT_PADDING);
    layoutProperty->UpdatePadding(rowLeftPadding);

    auto leftIcon = BuildMenuItemIcon(resourceId);
    containerTitleRow->AddChild(leftIcon);

    // text 144 22  padding 8vp
    auto titleLabel = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = titleLabel->GetLayoutProperty<TextLayoutProperty>();
    textLayoutProperty->UpdateContent(title);
    textLayoutProperty->UpdateMaxLines(MENU_ITEM_MAXLINES);
    textLayoutProperty->UpdateFontSize(TITLE_TEXT_FONT_SIZE);
    textLayoutProperty->UpdateTextOverflow(TextOverflow::ELLIPSIS);
    textLayoutProperty->UpdateAlignment(Alignment::CENTER_LEFT);
    textLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(MENU_ITEM_TEXT_WIDTH),
        CalcLength(MENU_ITEM_TEXT_HEIGHT)));
    PaddingProperty padding;
    padding.left = CalcLength(MENU_ITEM_TEXT_PADDING);
    auto text = BuildMenuItemPadding(padding, titleLabel);

    // add icon and label
    containerTitleRow->AddChild(leftIcon);
    containerTitleRow->AddChild(text);
    if (chooseCurrent) {
        auto chooseIcon = BuildMenuItemIcon(InternalResource::ResourceId::IC_WINDOW_MENU_OK);
        auto iconLayoutProperty = chooseIcon->GetLayoutProperty<ImageLayoutProperty>();
        PaddingProperty chooseIconLeftPadding;
        chooseIconLeftPadding.left = CalcLength(MENU_ITEM_TEXT_PADDING);
        containerTitleRow->AddChild(BuildMenuItemPadding(chooseIconLeftPadding, chooseIcon));
        auto renderContext = containerTitleRow->GetRenderContext();
        renderContext->UpdateBackgroundColor(MENU_ITEM_CHOOSE_COLOR);
    }
    auto hub = containerTitleRow->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(hub, nullptr);
    hub->AddClickEvent(event);
    return containerTitleRow;
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildMenuItemIcon(InternalResource::ResourceId resourceId)
{
    auto icon = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    auto iconLayoutProperty = icon->GetLayoutProperty<ImageLayoutProperty>();
    ImageSourceInfo sourceInfo;
    sourceInfo.SetResourceId(resourceId);
    iconLayoutProperty->UpdateImageSourceInfo(sourceInfo);
    iconLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(TITLE_BUTTON_SIZE),
        CalcLength(TITLE_BUTTON_SIZE)));
    icon->MarkModifyDone();
    return icon;
}

RefPtr<FrameNode> ContainerModalViewEnhance::BuildMenuItemPadding(PaddingProperty padding, RefPtr<FrameNode> node)
{
    auto row = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
        
    auto rowLayoutProperty = row->GetLayoutProperty<LinearLayoutProperty>();
    rowLayoutProperty->UpdatePadding(padding);
    row->AddChild(node);

    return row;
}

void ContainerModalViewEnhance::ResetHoverTimer()
{
    LOGD("ContainerModalViewEnhance ResetHoverTimer sIsMenuPending_ %{public}d", sIsMenuPending_);
    sContextTimer_.Reset(nullptr);
    sIsMenuPending_ = false;
}
} // namespace OHOS::Ace::NG
