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

#include "core/components_ng/event/drag_event.h"

#include "base/utils/utils.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/gestures/recognizers/long_press_recognizer.h"
#include "core/components_ng/gestures/recognizers/pan_recognizer.h"
#include "core/components_ng/gestures/recognizers/sequenced_recognizer.h"
#include "core/pipeline_ng/pipeline_context.h"
#ifdef ENABLE_DRAG_FRAMEWORK
#include "base/subwindow/subwindow_manager.h"
#include "core/animation/animation_pub.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_ng/render/render_context.h"
#endif // ENABLE_DRAG_FRAMEWORK

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t PAN_FINGER = 1;
constexpr double PAN_DISTANCE = 5.0;
constexpr int32_t LONG_PRESS_DURATION = 500;
#ifdef ENABLE_DRAG_FRAMEWORK
constexpr float FILTER_RADIUS = 100.0f;
constexpr float PIXELMAP_ANIMATION_SCALE = 1.1f;
constexpr int32_t PIXELMAP_ANIMATION_DURATION = 300;
#endif // ENABLE_DRAG_FRAMEWORK
} // namespace

DragEventActuator::DragEventActuator(
    const WeakPtr<GestureEventHub>& gestureEventHub, PanDirection direction, int32_t fingers, float distance)
    : gestureEventHub_(gestureEventHub), direction_(direction), fingers_(fingers), distance_(distance)
{
    if (fingers_ < PAN_FINGER) {
        fingers_ = PAN_FINGER;
    }

    if (LessOrEqual(distance_, PAN_DISTANCE)) {
        distance_ = PAN_DISTANCE;
    }

    panRecognizer_ = MakeRefPtr<PanRecognizer>(fingers_, direction_, distance_);
    longPressRecognizer_ = AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION, fingers_, false, false);
}

void DragEventActuator::OnCollectTouchTarget(const OffsetF& coordinateOffset, const TouchRestrict& touchRestrict,
    const GetEventTargetImpl& getEventTargetImpl, TouchTestResult& result)
{
    CHECK_NULL_VOID_NOLOG(userCallback_);
    auto actionStart = [weak = WeakClaim(this), this](GestureEvent& info) {
        auto actuator = weak.Upgrade();
        CHECK_NULL_VOID(actuator);
#ifdef ENABLE_DRAG_FRAMEWORK
        HideEventColumn();
        HidePixelMap(true, info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY());
        HideFilter();
        SubwindowManager::GetInstance()->HideMenuNG();
#endif // ENABLE_DRAG_FRAMEWORK
        // Trigger drag start event setted by user.
        CHECK_NULL_VOID(actuator->userCallback_);
        auto userActionStart = actuator->userCallback_->GetActionStartEventFunc();
        if (userActionStart) {
            userActionStart(info);
        }
        // Trigger custom drag start event
        CHECK_NULL_VOID(actuator->customCallback_);
        auto customActionStart = actuator->customCallback_->GetActionStartEventFunc();
        if (customActionStart) {
            customActionStart(info);
        }
    };
    panRecognizer_->SetOnActionStart(actionStart);

    auto actionUpdate = [weak = WeakClaim(this)](GestureEvent& info) {
        auto actuator = weak.Upgrade();
        CHECK_NULL_VOID(actuator);
        CHECK_NULL_VOID(actuator->userCallback_);
        auto userActionUpdate = actuator->userCallback_->GetActionUpdateEventFunc();
        if (userActionUpdate) {
            userActionUpdate(info);
        }
        CHECK_NULL_VOID(actuator->customCallback_);
        auto customActionUpdate = actuator->customCallback_->GetActionUpdateEventFunc();
        if (customActionUpdate) {
            customActionUpdate(info);
        }
    };
    panRecognizer_->SetOnActionUpdate(actionUpdate);

    auto actionEnd = [weak = WeakClaim(this)](GestureEvent& info) {
        auto actuator = weak.Upgrade();
        CHECK_NULL_VOID(actuator);
        CHECK_NULL_VOID(actuator->userCallback_);
        auto userActionEnd = actuator->userCallback_->GetActionEndEventFunc();
        if (userActionEnd) {
            userActionEnd(info);
        }
        CHECK_NULL_VOID(actuator->customCallback_);
        auto customActionEnd = actuator->customCallback_->GetActionEndEventFunc();
        if (customActionEnd) {
            customActionEnd(info);
        }
    };
    panRecognizer_->SetOnActionEnd(actionEnd);
#ifdef ENABLE_DRAG_FRAMEWORK
    auto actionCancel = [weak = WeakClaim(this), this]() {
#else
    auto actionCancel = [weak = WeakClaim(this)]() {
#endif // ENABLE_DRAG_FRAMEWORK
        auto actuator = weak.Upgrade();
        CHECK_NULL_VOID(actuator);
#ifdef ENABLE_DRAG_FRAMEWORK
        if (!GetIsBindOverlayValue(actuator)) {
            HideEventColumn();
            HidePixelMap();
            HideFilter();
        }
#endif // ENABLE_DRAG_FRAMEWORK
        CHECK_NULL_VOID(actuator->userCallback_);
        auto userActionCancel = actuator->userCallback_->GetActionCancelEventFunc();
        if (userActionCancel) {
            userActionCancel();
        }
        CHECK_NULL_VOID(actuator->customCallback_);
        auto customActionCancel = actuator->customCallback_->GetActionCancelEventFunc();
        if (customActionCancel) {
            customActionCancel();
        }
    };
    panRecognizer_->SetOnActionCancel(actionCancel);

#ifdef ENABLE_DRAG_FRAMEWORK
    auto longPressUpdate = [weak = WeakClaim(this), this](GestureEvent& info) {
        auto actuator = weak.Upgrade();
        CHECK_NULL_VOID(actuator);
        bool isAllowedDrag = IsAllowedDrag(actuator);
        if (isAllowedDrag) {
            SetFilter(actuator);
            SetPixelMap(actuator);
            SetEventColumn();
        }
    };
    longPressRecognizer_->SetOnActionUpdate(longPressUpdate);
#endif // ENABLE_DRAG_FRAMEWORK
    longPressRecognizer_->SetGestureHub(gestureEventHub_);
    std::vector<RefPtr<NGGestureRecognizer>> recognizers { longPressRecognizer_, panRecognizer_ };
    if (!SequencedRecognizer_) {
        SequencedRecognizer_ = AceType::MakeRefPtr<SequencedRecognizer>(recognizers);
        SequencedRecognizer_->RemainChildOnResetStatus();
    }
    SequencedRecognizer_->SetCoordinateOffset(Offset(coordinateOffset.GetX(), coordinateOffset.GetY()));
    SequencedRecognizer_->SetGetEventTargetImpl(getEventTargetImpl);
    result.emplace_back(SequencedRecognizer_);
}

#ifdef ENABLE_DRAG_FRAMEWORK
void DragEventActuator::SetFilter(const RefPtr<DragEventActuator>& actuator)
{
    auto gestureHub = actuator->gestureEventHub_.Upgrade();
    CHECK_NULL_VOID(gestureHub);
    auto frameNode = gestureHub->GetFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto parent = frameNode->GetParent();
    CHECK_NULL_VOID(parent);
    while (parent->GetDepth() != 1) {
        parent = parent->GetParent();
    }
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    if (!manager->GetHasFilter() && !manager->GetIsOnAnimation()) {
        // insert columnNode to rootNode
        auto columnNode = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
            AceType::MakeRefPtr<LinearLayoutPattern>(true));
        auto children = parent->GetChildren();
        int32_t slot = 0;
        for (auto& child: children) {
            if (child->GetTag() == "Popup") {
                continue;
            }
            parent->RemoveChild(child);
            child->MountToParent(columnNode, slot);
            slot++;
        }
        columnNode->MountToParent(parent, 0);
        columnNode->OnMountToParentDone();
        manager->SetHasFilter(true);
        manager->SetFilterColumnNode(columnNode);
        parent->MarkDirtyNode(NG::PROPERTY_UPDATE_BY_CHILD_REQUEST);
        // set filter
        bool isBindOverlayValue = frameNode->GetLayoutProperty()->GetIsBindOverlayValue(false);
        auto context = DynamicCast<NG::RosenRenderContext>(columnNode->GetRenderContext());
        CHECK_NULL_VOID(context);
        auto rsNode = context->GetRSNode();
        CHECK_NULL_VOID(rsNode);
        std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateBlurFilter(FILTER_RADIUS, FILTER_RADIUS);
        std::shared_ptr<Rosen::RSFilter> filter = Rosen::RSFilter::CreateBlurFilter(FILTER_RADIUS, FILTER_RADIUS);
        if (isBindOverlayValue) {
            rsNode->SetBackgroundFilter(backFilter);
            rsNode->SetFilter(filter);
        }
    }
}

void DragEventActuator::SetPixelMap(const RefPtr<DragEventActuator>& actuator)
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    if (manager->GetHasPixelMap()) {
        return;
    }
    auto gestureHub = actuator->gestureEventHub_.Upgrade();
    CHECK_NULL_VOID(gestureHub);
    auto frameNode = gestureHub->GetFrameNode();
    CHECK_NULL_VOID(frameNode);
    RefPtr<PixelMap> pixelMap = gestureHub->GetPixelMap();
    auto width = pixelMap->GetWidth();
    auto height = pixelMap->GetHeight();
    auto offsetToWindow = frameNode->GetPaintRectOffset();
    auto offsetX = offsetToWindow.GetX();
    auto offsetY = offsetToWindow.GetY();
    // craete imageNode
    auto imageNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    auto props = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    props->UpdateImageSourceInfo(ImageSourceInfo(pixelMap));
    auto targetSize = CalcSize(NG::CalcLength(width), NG::CalcLength(height));
    props->UpdateUserDefinedIdealSize(targetSize);
    auto imageContext = imageNode->GetRenderContext();
    CHECK_NULL_VOID(imageContext);
    imageContext->UpdatePosition(OffsetT<Dimension>(Dimension(offsetX), Dimension(offsetY)));
    // create columnNode
    auto columnNode = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    columnNode->AddChild(imageNode);
    auto hub = columnNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(hub);
    hub->SetPixelMap(gestureHub->GetPixelMap());
    // mount to rootNode
    manager->MountPixelmapToRootNode(columnNode);
    imageNode->MarkModifyDone();
    ShowPixelMapAnimation(imageNode);
}

void DragEventActuator::SetEventColumn()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    if (manager->GetHasEvent()) {
        return;
    }
    auto rootNode = pipelineContext->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto geometryNode = rootNode->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto width = geometryNode->GetFrameSize().Width();
    auto height = geometryNode->GetFrameSize().Height();
    // create columnNode
    auto columnNode = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    auto props = columnNode->GetLayoutProperty<LinearLayoutProperty>();
    auto targetSize = CalcSize(NG::CalcLength(width), NG::CalcLength(height));
    props->UpdateUserDefinedIdealSize(targetSize);
    BindClickEvent(columnNode);
    columnNode->MarkModifyDone();
    manager->MountEventToRootNode(columnNode);
}

void DragEventActuator::HideFilter()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    manager->RemoveFilter();
}

void DragEventActuator::HidePixelMap(bool startDrag, double x, double y)
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    manager->RemovePixelMapAnimation(startDrag, x, y);
}

void DragEventActuator::HideEventColumn()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    manager->RemoveEventColumn();
}

void DragEventActuator::BindClickEvent(const RefPtr<FrameNode>& columnNode)
{
    auto callback = [this, weak = WeakClaim(this)](GestureEvent& /* info */) {
        HideEventColumn();
        HidePixelMap();
        HideFilter();
    };
    auto columnGestureHub = columnNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(columnGestureHub);
    auto clickListener = MakeRefPtr<ClickEvent>(std::move(callback));
    columnGestureHub->AddClickEvent(clickListener);
}

void DragEventActuator::ShowPixelMapAnimation(const RefPtr<FrameNode>& imageNode)
{
    auto imageContext = imageNode->GetRenderContext();
    CHECK_NULL_VOID(imageContext);
    // pixel map animation
    AnimationOption option;
    option.SetDuration(PIXELMAP_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    imageContext->UpdateTransformScale( { 1, 1 } );
    auto shadow = imageContext->GetBackShadow();
    if (!shadow.has_value()) {
        shadow = Shadow::CreateShadow(ShadowStyle::None);
    }
    imageContext->UpdateBackShadow(shadow.value());

    AnimationUtils::Animate(
        option,
        [imageContext, shadow]() mutable {
            auto color = shadow->GetColor();
            auto newColor = Color::FromARGB(1, color.GetRed(), color.GetGreen(), color.GetBlue());
            shadow->SetColor(newColor);
            imageContext->UpdateBackShadow(shadow.value());
            imageContext->UpdateTransformScale({ PIXELMAP_ANIMATION_SCALE, PIXELMAP_ANIMATION_SCALE });
        },
        option.GetOnFinishEvent());
}

bool DragEventActuator::GetIsBindOverlayValue(const RefPtr<DragEventActuator>& actuator)
{
    auto gestureHub = actuator->gestureEventHub_.Upgrade();
    CHECK_NULL_RETURN(gestureHub, true);
    auto frameNode = gestureHub->GetFrameNode();
    CHECK_NULL_RETURN(frameNode, true);
    bool isBindOverlayValue = frameNode->GetLayoutProperty()->GetIsBindOverlayValue(false);
    return isBindOverlayValue;
}

bool DragEventActuator::IsAllowedDrag(const RefPtr<DragEventActuator>& actuator)
{
    auto gestureHub = actuator->gestureEventHub_.Upgrade();
    CHECK_NULL_RETURN(gestureHub, false);
    auto frameNode = gestureHub->GetFrameNode();
    CHECK_NULL_RETURN(frameNode, false);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(eventHub, false);
    bool isAllowedDrag = gestureHub->IsAllowedDrag(eventHub);
    return isAllowedDrag;
}
#endif // ENABLE_DRAG_FRAMEWORK
} // namespace OHOS::Ace::NG
