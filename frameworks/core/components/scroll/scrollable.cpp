/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/scroll/scrollable.h"

#include <chrono>

#include "base/log/ace_trace.h"
#include "base/log/frame_report.h"
#include "base/log/log.h"
#include "base/ressched/ressched_report.h"
#include "base/utils/time_util.h"
#include "core/common/container.h"
#include "core/event/ace_events.h"
#include "core/common/layout_inspector.h"

namespace OHOS::Ace {
namespace {

constexpr double SPRING_SCROLL_MASS = 0.5;
constexpr double SPRING_SCROLL_STIFFNESS = 100.0;
constexpr double SPRING_SCROLL_DAMPING = 15.55635;
const RefPtr<SpringProperty> DEFAULT_OVER_SPRING_PROPERTY =
    AceType::MakeRefPtr<SpringProperty>(SPRING_SCROLL_MASS, SPRING_SCROLL_STIFFNESS, SPRING_SCROLL_DAMPING);
#ifndef WEARABLE_PRODUCT
constexpr double FRICTION = 0.6;
constexpr double VELOCITY_SCALE = 1.0;
constexpr double MAX_VELOCITY = 800000.0;
constexpr double MIN_VELOCITY = -800000.0;
constexpr double ADJUSTABLE_VELOCITY = 3000.0;
#else
constexpr double DISTANCE_EPSILON = 1.0;
constexpr double FRICTION = 0.9;
constexpr double VELOCITY_SCALE = 0.8;
constexpr double MAX_VELOCITY = 5000.0;
constexpr double MIN_VELOCITY = -5000.0;
constexpr double ADJUSTABLE_VELOCITY = 0.0;
#endif

#ifdef OHOS_PLATFORM
constexpr int64_t INCREASE_CPU_TIME_ONCE = 4000000000; // 4s(unit: ns)
#endif

} // namespace

// Static Functions.
double Scrollable::sFriction_ = FRICTION;
double Scrollable::sVelocityScale_ = VELOCITY_SCALE;

void Scrollable::SetVelocityScale(double sVelocityScale)
{
    if (LessOrEqual(sVelocityScale, 0.0)) {
        LOGW("Invalid velocity scale: %{public}lf", sVelocityScale);
        return;
    }
    sVelocityScale_ = sVelocityScale;
}

void Scrollable::SetFriction(double sFriction)
{
    if (LessOrEqual(sFriction, 0.0)) {
        LOGW("Invalid friction value: %{public}lf", sFriction);
        return;
    }
    sFriction_ = sFriction;
}

Scrollable::~Scrollable()
{
    // If animation still runs, force stop it.
    controller_->Stop();
    springController_->Stop();
}

void Scrollable::OnFlushTouchEventsBegin()
{
    if (panRecognizer_) {
        panRecognizer_->OnFlushTouchEventsBegin();
    }
}

void Scrollable::OnFlushTouchEventsEnd()
{
    if (panRecognizer_) {
        panRecognizer_->OnFlushTouchEventsEnd();
    }
}

void Scrollable::Initialize(const WeakPtr<PipelineBase>& context)
{
    context_ = context;
    PanDirection panDirection;
    if (axis_ == Axis::VERTICAL) {
        panDirection.type = PanDirection::VERTICAL;
    } else {
        panDirection.type = PanDirection::HORIZONTAL;
    }

    auto actionStart = [weakScroll = AceType::WeakClaim(this)](const GestureEvent& info) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            if (info.GetInputEventType() == InputEventType::MOUSE_BUTTON && !scroll->NeedMouseLeftButtonScroll()) {
                return;
            }
            // Send event to accessibility when scroll start.
            auto context = scroll->GetContext().Upgrade();
            if (context) {
                AccessibilityEvent scrollEvent;
                scrollEvent.nodeId = scroll->nodeId_;
                scrollEvent.eventType = "scrollstart";
                context->SendEventToAccessibility(scrollEvent);
            }
            scroll->HandleDragStart(info);
        }
    };

    auto actionUpdate = [weakScroll = AceType::WeakClaim(this)](const GestureEvent& info) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            if (info.GetInputEventType() == InputEventType::MOUSE_BUTTON && !scroll->NeedMouseLeftButtonScroll()) {
                return;
            }
            scroll->HandleDragUpdate(info);
        }
    };

    auto actionEnd = [weakScroll = AceType::WeakClaim(this)](const GestureEvent& info) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            if (info.GetInputEventType() == InputEventType::MOUSE_BUTTON && !scroll->NeedMouseLeftButtonScroll()) {
                return;
            }
            scroll->HandleDragEnd(info);
            // Send event to accessibility when scroll stop.
            auto context = scroll->GetContext().Upgrade();
            if (context && scroll->IsStopped()) {
                AccessibilityEvent scrollEvent;
                scrollEvent.nodeId = scroll->nodeId_;
                scrollEvent.eventType = "scrollend";
                context->SendEventToAccessibility(scrollEvent);
            }
        }
    };

    auto actionCancel = [weakScroll = AceType::WeakClaim(this)]() {
        auto scroll = weakScroll.Upgrade();
        if (!scroll) {
            return;
        }
        if (scroll->dragCancelCallback_) {
            scroll->dragCancelCallback_();
        }
    };

    if (Container::IsCurrentUseNewPipeline()) {
        panRecognizerNG_ =
            AceType::MakeRefPtr<NG::PanRecognizer>(DEFAULT_PAN_FINGER, panDirection, DEFAULT_PAN_DISTANCE);

        panRecognizerNG_->SetOnActionStart(actionStart);
        panRecognizerNG_->SetOnActionUpdate(actionUpdate);
        panRecognizerNG_->SetOnActionEnd(actionEnd);
        panRecognizerNG_->SetOnActionCancel(actionCancel);
    } else {
        panRecognizer_ =
            AceType::MakeRefPtr<PanRecognizer>(context, DEFAULT_PAN_FINGER, panDirection, DEFAULT_PAN_DISTANCE);
        panRecognizer_->SetOnActionStart(actionStart);
        panRecognizer_->SetOnActionUpdate(actionUpdate);
        panRecognizer_->SetOnActionEnd(actionEnd);
        panRecognizer_->SetOnActionCancel(actionCancel);
    }

    // use RawRecognizer to receive next touch down event to stop animation.
    rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();

    rawRecognizer_->SetOnTouchDown([weakScroll = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleTouchDown();
        }
    });
    rawRecognizer_->SetOnTouchUp([weakScroll = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleTouchUp();
        }
    });
    rawRecognizer_->SetOnTouchCancel([weakScroll = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleTouchCancel();
        }
    });

    controller_ = CREATE_ANIMATOR(context);
    springController_ = CREATE_ANIMATOR(context);

    spring_ = GetDefaultOverSpringProperty();
    available_ = true;
}

void Scrollable::SetAxis(Axis axis)
{
    axis_ = axis;
    PanDirection panDirection;
    if (axis_ == Axis::VERTICAL) {
        panDirection.type = PanDirection::VERTICAL;
    } else {
        panDirection.type = PanDirection::HORIZONTAL;
    }
    if (panRecognizer_) {
        panRecognizer_->SetDirection(panDirection);
    }
    if (panRecognizerNG_) {
        panRecognizerNG_->SetDirection(panDirection);
    }
}

void Scrollable::HandleTouchDown()
{
    LOGD("handle touch down");
    isTouching_ = true;
    // If animation still runs, first stop it.
    springController_->Stop();
    if (!controller_->IsStopped()) {
        controller_->Stop();
    } else {
        // Resets values.
        currentPos_ = 0.0;
    }
    LOGD("handle touch down has already stopped the animation");
}

void Scrollable::HandleTouchUp()
{
    LOGD("handle touch up");
    isTouching_ = false;
    if (outBoundaryCallback_ && !outBoundaryCallback_()) {
        return;
    }
    if (springController_->IsStopped() && scrollOverCallback_) {
        LOGD("need scroll to boundary");
        ProcessScrollOverCallback(0.0);
    }
}

void Scrollable::HandleTouchCancel()
{
    LOGD("handle touch cancel");
    isTouching_ = false;
    if (springController_->IsStopped() && scrollOverCallback_) {
        ProcessScrollOverCallback(0.0);
    }
}

bool Scrollable::IsAnimationNotRunning() const
{
    return !isTouching_ && !controller_->IsRunning() && !springController_->IsRunning();
}

bool Scrollable::Idle() const
{
    return !isTouching_ && controller_->IsStopped() && springController_->IsStopped();
}

bool Scrollable::IsStopped() const
{
    return (!springController_ || (springController_->IsStopped())) && (!controller_ || (controller_->IsStopped()));
}

bool Scrollable::IsSpringStopped() const
{
    return !springController_ || (springController_->IsStopped());
}

void Scrollable::StopScrollable()
{
    if (controller_) {
        controller_->Stop();
    }
    if (springController_) {
        springController_->Stop();
    }
}

void Scrollable::HandleScrollEnd()
{
    if (scrollEndCallback_) {
        scrollEndCallback_();
    }
    auto parent = parent_.Upgrade();
    if (parent && nestedOpt_.NeedParent()) {
        parent->HandleScrollEnd();
    }
}

void Scrollable::HandleDragStart(const OHOS::Ace::GestureEvent& info)
{
    ACE_FUNCTION_TRACE();
    const auto dragPositionInMainAxis =
        axis_ == Axis::VERTICAL ? info.GetGlobalLocation().GetY() : info.GetGlobalLocation().GetX();
    LOGD("HandleDragStart. LocalLocation: %{public}s, GlobalLocation: %{public}s",
        info.GetLocalLocation().ToString().c_str(), info.GetGlobalLocation().ToString().c_str());
#ifdef OHOS_PLATFORM
    // Increase the cpu frequency when sliding start.
    auto currentTime = GetSysTimestamp();
    auto increaseCpuTime = currentTime - startIncreaseTime_;
    if (!moved_ || increaseCpuTime >= INCREASE_CPU_TIME_ONCE) {
        LOGI("HandleDragStart increase cpu frequency, moved_ = %{public}d", moved_);
        startIncreaseTime_ = currentTime;
        ResSchedReport::GetInstance().ResSchedDataReport("slide_on");
        if (FrameReport::GetInstance().GetEnable()) {
            FrameReport::GetInstance().BeginListFling();
        }
    }
#endif
    UpdateScrollPosition(dragPositionInMainAxis, SCROLL_FROM_START);
    auto parent = parent_.Upgrade();
    if (parent && nestedOpt_.NeedParent()) {
        parent->UpdateScrollPosition(dragPositionInMainAxis, SCROLL_FROM_START);
    }
    RelatedEventStart();
    auto node = scrollableNode_.Upgrade();
    if (node) {
        node->DispatchCancelPressAnimation();
    }
}

double Scrollable::HandleScrollParentFirst(double& offset, int32_t source, NestedState state)
{
    auto parent = parent_.Upgrade();
    ScrollState scrollState = source == SCROLL_FROM_ANIMATION ? ScrollState::FLING : ScrollState::SCROLL;
    if (state == NestedState::CHILD_OVER_SCROLL) {
        if (edgeEffect_ == EdgeEffect::NONE) {
            return parent->HandleScroll(offset, source, NestedState::CHILD_OVER_SCROLL);
        }
        ExecuteScrollFrameBegin(offset, scrollState);
        return 0;
    }
    offset = parent->HandleScroll(offset, source, NestedState::CHILD_SCROLL);
    if (NearZero(offset)) {
        canOverScroll_ = false;
        return 0;
    }
    double allOffset = offset;
    ExecuteScrollFrameBegin(offset, scrollState);
    auto remainOffset = allOffset - offset;
    auto overOffsets = overScrollOffsetCallback_(offset);
    auto overOffset = offset > 0 ? overOffsets.start : overOffsets.end;
    remainOffset += offset > 0 ? overOffsets.start : overOffsets.end;
    if (NearZero(remainOffset)) {
        canOverScroll_ = false;
        return 0;
    }
    if (state == NestedState::CHILD_SCROLL) {
        offset -= overOffset;
        canOverScroll_ = false;
        return remainOffset;
    }
    if (edgeEffect_ == EdgeEffect::NONE) {
        parent->HandleScroll(remainOffset, source, NestedState::CHILD_OVER_SCROLL);
    }
    canOverScroll_ = !NearZero(overOffset);
    return 0;
}

double Scrollable::HandleScrollSelfFirst(double& offset, int32_t source, NestedState state)
{
    auto parent = parent_.Upgrade();
    ScrollState scrollState = source == SCROLL_FROM_ANIMATION ? ScrollState::FLING : ScrollState::SCROLL;
    if (state == NestedState::CHILD_OVER_SCROLL) {
        auto remain = parent->HandleScroll(offset, source, NestedState::CHILD_OVER_SCROLL);
        if (NearZero(remain)) {
            offset = 0;
            return 0;
        }
        ExecuteScrollFrameBegin(offset, scrollState);
        if (edgeEffect_ == EdgeEffect::NONE) {
            return remain;
        }
        return 0;
    }
    double allOffset = offset;
    ExecuteScrollFrameBegin(offset, scrollState);
    auto remainOffset = allOffset - offset;
    auto overOffsets = overScrollOffsetCallback_(offset);
    auto overOffset = offset > 0 ? overOffsets.start : overOffsets.end;
    if (NearZero(overOffset)) {
        canOverScroll_ = false;
        return 0;
    }
    offset -= overOffset;
    remainOffset += parent->HandleScroll(overOffset, source, NestedState::CHILD_SCROLL);
    if (NearZero(remainOffset)) {
        canOverScroll_ = false;
        return 0;
    }
    if (state == NestedState::CHILD_SCROLL) {
        canOverScroll_ = false;
        return remainOffset;
    }
    auto remain = parent->HandleScroll(remainOffset, source, NestedState::CHILD_OVER_SCROLL);
    offset += remain;
    canOverScroll_ = !NearZero(overOffset);
    return 0;
}

double Scrollable::HandleScrollSelfOnly(double& offset, int32_t source, NestedState state)
{
    double allOffset = offset;
    ScrollState scrollState = source == SCROLL_FROM_ANIMATION ? ScrollState::FLING : ScrollState::SCROLL;
    ExecuteScrollFrameBegin(offset, scrollState);
    auto remainOffset = allOffset - offset;
    auto overOffsets = overScrollOffsetCallback_(offset);
    auto overOffset = offset > 0 ? overOffsets.start : overOffsets.end;
    remainOffset += overOffset;
    if (NearZero(remainOffset)) {
        canOverScroll_ = false;
        return 0;
    }
    bool canOverScroll = false;
    if (state == NestedState::CHILD_SCROLL) {
        offset -= overOffset;
    } else if (state == NestedState::GESTURE) {
        canOverScroll = !NearZero(overOffset);
    } else if (edgeEffect_ != EdgeEffect::NONE) {
        remainOffset = 0;
    }
    canOverScroll_ = canOverScroll;
    return remainOffset;
}

double Scrollable::HandleScrollParallel(double& offset, int32_t source, NestedState state)
{
    auto remainOffset = 0.0;
    auto parent = parent_.Upgrade();
    ScrollState scrollState = source == SCROLL_FROM_ANIMATION ? ScrollState::FLING : ScrollState::SCROLL;
    if (state == NestedState::CHILD_OVER_SCROLL) {
        if (edgeEffect_ == EdgeEffect::NONE) {
            remainOffset = parent->HandleScroll(offset, source, NestedState::CHILD_OVER_SCROLL);
            offset = 0;
        } else {
            ExecuteScrollFrameBegin(offset, scrollState);
        }
        return remainOffset;
    }

    bool canOverScroll = false;
    double parentOffset = offset;
    ExecuteScrollFrameBegin(offset, scrollState);
    auto parentRemain = parent->HandleScroll(parentOffset, source, NestedState::CHILD_SCROLL);

    auto overOffsets = overScrollOffsetCallback_(offset);
    auto overOffset = offset > 0 ? overOffsets.start : overOffsets.end;
    if (!NearZero(overOffset) && !NearZero(parentRemain)) {
        if (state == NestedState::CHILD_SCROLL) {
            remainOffset = overOffset;
            offset = offset - overOffset;
        } else if (edgeEffect_ == EdgeEffect::NONE) {
            auto remain = parent->HandleScroll(parentRemain, source, NestedState::CHILD_OVER_SCROLL);
            canOverScroll = NearZero(remain);
            offset = offset - overOffset;
        } else {
            canOverScroll = true;
        }
    } else if (!NearZero(overOffset)) {
        offset = offset - overOffset;
    }
    canOverScroll_ = canOverScroll;
    return remainOffset;
}

double Scrollable::HandleScroll(double offset, int32_t source, NestedState state)
{
    if (!overScrollOffsetCallback_) {
        ExecuteScrollBegin(offset);
        ExecuteScrollFrameBegin(offset, ScrollState::SCROLL);
        moved_ = UpdateScrollPosition(offset, source);
        return 0;
    }
    auto remainOffset = 0.0;
    ScrollState scrollState = source == SCROLL_FROM_ANIMATION ? ScrollState::FLING : ScrollState::SCROLL;
    auto parent = parent_.Upgrade();
    auto overOffsets = overScrollOffsetCallback_(offset);
    double backOverOffset = offset > 0 ? overOffsets.end : overOffsets.start;
    if (!NearZero(backOverOffset)) {
        ExecuteScrollFrameBegin(offset, scrollState);
    } else if (parent && ((offset < 0 && nestedOpt_.forward == NestedScrollMode::PARENT_FIRST) ||
        (offset > 0 && nestedOpt_.backward == NestedScrollMode::PARENT_FIRST))) {
        remainOffset = HandleScrollParentFirst(offset, source, state);
    } else if (parent && ((offset < 0 && nestedOpt_.forward == NestedScrollMode::SELF_FIRST) ||
        (offset > 0 && nestedOpt_.backward == NestedScrollMode::SELF_FIRST))) {
        remainOffset = HandleScrollSelfFirst(offset, source, state);
    } else if (parent && ((offset < 0 && nestedOpt_.forward == NestedScrollMode::PARALLEL) ||
        (offset > 0 && nestedOpt_.backward == NestedScrollMode::PARALLEL))) {
        remainOffset = HandleScrollParallel(offset, source, state);
    } else {
        remainOffset = HandleScrollSelfOnly(offset, source, state);
    }
    moved_ = UpdateScrollPosition(offset, source);
    return remainOffset;
}

void Scrollable::HandleDragUpdate(const GestureEvent& info)
{
    ACE_FUNCTION_TRACE();
    if (!springController_->IsStopped() || !controller_->IsStopped()) {
        // If animation still runs, first stop it.
        isDragUpdateStop_ = true;
        controller_->Stop();
        springController_->Stop();
        currentPos_ = 0.0;
    }
#ifdef OHOS_PLATFORM
    // Handle the case where you keep sliding past limit time(4s).
    auto currentTime = GetSysTimestamp();
    auto increaseCpuTime = currentTime - startIncreaseTime_;
    if (increaseCpuTime >= INCREASE_CPU_TIME_ONCE) {
        LOGI("HandleDragUpdate increase cpu frequency, moved_ = %{public}d", moved_);
        startIncreaseTime_ = currentTime;
        ResSchedReport::GetInstance().ResSchedDataReport("slide_on");
        if (FrameReport::GetInstance().GetEnable()) {
            FrameReport::GetInstance().BeginListFling();
        }
    }
#endif
    LOGD("handle drag update, offset is %{public}lf", info.GetMainDelta());
    auto mainDelta = info.GetMainDelta();
    if (RelatedScrollEventPrepare(Offset(0.0, mainDelta))) {
        return;
    }
    auto source = info.GetInputEventType() == InputEventType::AXIS ? SCROLL_FROM_AXIS : SCROLL_FROM_UPDATE;
    HandleScroll(mainDelta, source, NestedState::GESTURE);
}

void Scrollable::HandleDragEnd(const GestureEvent& info)
{
    LOGD("handle drag end, position is %{public}lf and %{public}lf, velocity is %{public}lf",
        info.GetGlobalPoint().GetX(), info.GetGlobalPoint().GetY(), info.GetMainVelocity());
    controller_->ClearAllListeners();
    springController_->ClearAllListeners();
    isDragUpdateStop_ = false;
    touchUp_ = false;
    scrollPause_ = false;
    double correctVelocity = std::clamp(info.GetMainVelocity(), MIN_VELOCITY + slipFactor_, MAX_VELOCITY - slipFactor_);
    correctVelocity = correctVelocity * sVelocityScale_;
    currentVelocity_ = correctVelocity;
    if (dragEndCallback_) {
        dragEndCallback_();
    }
    RelatedEventEnd();
    if (!moved_ || info.GetInputEventType() == InputEventType::AXIS) {
        LOGI("It is not moved now,  no need to handle drag end motion");
        HandleScrollEnd();
        currentVelocity_ = 0.0;
#ifdef OHOS_PLATFORM
        LOGI("springController stop increase cpu frequency");
        ResSchedReport::GetInstance().ResSchedDataReport("slide_off");
        if (FrameReport::GetInstance().GetEnable()) {
            FrameReport::GetInstance().EndListFling();
        }
#endif
    } else if (!overScrollOffsetCallback_ && outBoundaryCallback_ && outBoundaryCallback_() && scrollOverCallback_) {
        ProcessScrollOverCallback(correctVelocity);
    } else if (canOverScroll_) {
        HandleOverScroll(correctVelocity);
    } else {
        double mainPosition = GetMainOffset(Offset(info.GetGlobalPoint().GetX(), info.GetGlobalPoint().GetY()));
        LOGD("[scrollMotion]position(%{public}lf), velocity(%{public}lf)", mainPosition, correctVelocity);
        if (motion_) {
            motion_->Reset(sFriction_, mainPosition, correctVelocity);
        } else {
            motion_ = AceType::MakeRefPtr<FrictionMotion>(sFriction_, mainPosition, correctVelocity);
            motion_->AddListener([weakScroll = AceType::WeakClaim(this)](double value) {
                auto scroll = weakScroll.Upgrade();
                if (scroll) {
                    scroll->ProcessScrollMotion(value);
                }
            });
        }

        // change motion param when list item need to be center of screen on watch
        FixScrollMotion(mainPosition);

        // Resets values.
        currentPos_ = mainPosition;
        currentVelocity_ = 0.0;

        // Starts motion.
        controller_->ClearStopListeners();
        controller_->AddStopListener([weak = AceType::WeakClaim(this)]() {
            auto scroll = weak.Upgrade();
            if (scroll) {
                scroll->ProcessScrollMotionStop();
                // Send event to accessibility when scroll stop.
                auto context = scroll->GetContext().Upgrade();
                if (context && scroll->Idle()) {
                    AccessibilityEvent scrollEvent;
                    scrollEvent.nodeId = scroll->nodeId_;
                    scrollEvent.eventType = "scrollend";
                    context->SendEventToAccessibility(scrollEvent);
                }
            }
        });
        controller_->PlayMotion(motion_);
    }
}

void Scrollable::ExecuteScrollBegin(double& mainDelta)
{
    auto context = context_.Upgrade();
    if (!scrollBeginCallback_ || !context) {
        return;
    }

    ScrollInfo scrollInfo;
    if (axis_ == Axis::VERTICAL) {
        scrollInfo = scrollBeginCallback_(0.0_vp, Dimension(mainDelta / context->GetDipScale(), DimensionUnit::VP));
        mainDelta = context->NormalizeToPx(scrollInfo.dy);
    } else if (axis_ == Axis::HORIZONTAL) {
        scrollInfo = scrollBeginCallback_(Dimension(mainDelta / context->GetDipScale(), DimensionUnit::VP), 0.0_vp);
        mainDelta = context->NormalizeToPx(scrollInfo.dx);
    }
}

void Scrollable::ExecuteScrollFrameBegin(double& mainDelta, ScrollState state)
{
    auto context = context_.Upgrade();
    if (!scrollFrameBeginCallback_ || !context) {
        return;
    }

    auto offset = Dimension(mainDelta / context->GetDipScale(), DimensionUnit::VP);
    auto scrollRes = scrollFrameBeginCallback_(-offset, state);
    mainDelta = -context->NormalizeToPx(scrollRes.offset);
}

void Scrollable::FixScrollMotion(double position)
{
#ifdef WEARABLE_PRODUCT
    if (motion_ && needCenterFix_ && watchFixCallback_) {
        double finalPoisition = watchFixCallback_(motion_->GetFinalPosition(), position);
        LOGD("final position before fix(%{public}lf), need to fix to position(%{public}lf)",
            motion_->GetFinalPosition(), finalPoisition);
        if (!NearEqual(finalPoisition, motion_->GetFinalPosition(), DISTANCE_EPSILON)) {
            double velocity = motion_->GetVelocityByFinalPosition(finalPoisition);
            motion_->Reset(sFriction_, position, velocity);

            // fix again when velocity is less than velocity threshold
            if (!NearEqual(finalPoisition, motion_->GetFinalPosition(), DISTANCE_EPSILON)) {
                velocity = motion_->GetVelocityByFinalPosition(finalPoisition, 0.0);
                motion_->Reset(sFriction_, position, velocity, 0.0);
            }
            LOGD("final position after fix (%{public}lf), ", motion_->GetFinalPosition());
        }
    }
#endif
};

void Scrollable::StartSpringMotion(
    double mainPosition, double mainVelocity, const ExtentPair& extent, const ExtentPair& initExtent)
{
    LOGD("[scroll] position(%{public}lf), mainVelocity(%{public}lf), minExtent(%{public}lf), maxExtent(%{public}lf), "
         "initMinExtent(%{public}lf), initMaxExtent(%{public}lf",
        mainPosition, mainVelocity, extent.Leading(), extent.Trailing(), initExtent.Leading(), initExtent.Trailing());
    scrollMotion_ = AceType::MakeRefPtr<ScrollMotion>(mainPosition, mainVelocity, extent, initExtent, spring_);
    if (!scrollMotion_->IsValid()) {
        LOGE("scrollMotion is invalid, no available spring motion.");
        return;
    }
    scrollMotion_->AddListener([weakScroll = AceType::WeakClaim(this)](double position) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->ProcessSpringMotion(position);
        }
    });
    currentPos_ = mainPosition;
    springController_->ClearStopListeners();
    springController_->PlayMotion(scrollMotion_);
    springController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto scroll = weak.Upgrade();
        if (scroll) {
            if (scroll->moved_) {
                scroll->HandleScrollEnd();
            }
            scroll->currentVelocity_ = 0.0;
            if (scroll->isTouching_ || scroll->isDragUpdateStop_) {
                return;
            }
            scroll->moved_ = false;
#ifdef OHOS_PLATFORM
            LOGI("springController stop increase cpu frequency");
            ResSchedReport::GetInstance().ResSchedDataReport("slide_off");
            if (FrameReport::GetInstance().GetEnable()) {
                FrameReport::GetInstance().EndListFling();
            }
#endif
            if (scroll->scrollEnd_) {
                scroll->scrollEnd_();
            }
            // Send event to accessibility when scroll stop.
            auto context = scroll->GetContext().Upgrade();
            if (context) {
                AccessibilityEvent scrollEvent;
                scrollEvent.nodeId = scroll->nodeId_;
                scrollEvent.eventType = "scrollend";
                context->SendEventToAccessibility(scrollEvent);
            }
#if !defined(PREVIEW)
            LayoutInspector::SupportInspector();
#endif
        }
    });
}

void Scrollable::ProcessScrollMotionStop()
{
    if ((!scrollPause_ || !scrollOverCallback_) && moved_) {
        HandleScrollEnd();
    }

    // spring effect special process
    if (scrollPause_) {
        scrollPause_ = false;
        HandleOverScroll(currentVelocity_);
    } else {
        currentVelocity_ = 0.0;
        if (isDragUpdateStop_) {
            return;
        }
        moved_ = false;
#ifdef OHOS_PLATFORM
        LOGI("controller stop increase cpu frequency");
        ResSchedReport::GetInstance().ResSchedDataReport("slide_off");
        if (FrameReport::GetInstance().GetEnable()) {
            FrameReport::GetInstance().EndListFling();
        }
#endif
        if (scrollEnd_) {
            scrollEnd_();
        }
#if !defined(PREVIEW)
        LayoutInspector::SupportInspector();
#endif
    }
}

void Scrollable::ProcessSpringMotion(double position)
{
    LOGD("[scroll] currentPos_(%{public}lf), position(%{public}lf)", currentPos_, position);
    currentVelocity_ = scrollMotion_->GetCurrentVelocity();
    if (NearEqual(currentPos_, position)) {
        UpdateScrollPosition(0.0, SCROLL_FROM_ANIMATION_SPRING);
    } else {
        moved_ = UpdateScrollPosition(position - currentPos_, SCROLL_FROM_ANIMATION_SPRING);
        if (!moved_) {
            springController_->Stop();
        } else if (!touchUp_) {
            if (scrollTouchUpCallback_) {
                scrollTouchUpCallback_();
            }
            touchUp_ = true;
        }
    }
    currentPos_ = position;
}

void Scrollable::ProcessScrollMotion(double position)
{
    currentVelocity_ = motion_->GetCurrentVelocity();
    LOGD("[scrolling] position(%{public}lf), currentVelocity_(%{public}lf)", position, currentVelocity_);
    if ((NearEqual(currentPos_, position))) {
        UpdateScrollPosition(0.0, SCROLL_FROM_ANIMATION);
    } else {
        // UpdateScrollPosition return false, means reach to scroll limit.
        auto mainDelta = position - currentPos_;
        HandleScroll(mainDelta, SCROLL_FROM_ANIMATION, NestedState::GESTURE);
        if (!moved_) {
            controller_->Stop();
        } else if (!touchUp_) {
            if (scrollTouchUpCallback_) {
                scrollTouchUpCallback_();
            }
            touchUp_ = true;
        }
    }
    currentPos_ = position;

    // spring effect special process
    if (canOverScroll_ || (!overScrollOffsetCallback_ && (outBoundaryCallback_ && outBoundaryCallback_()))) {
        scrollPause_ = true;
        controller_->Stop();
    }
}

bool Scrollable::UpdateScrollPosition(const double offset, int32_t source) const
{
    bool ret = true;
    if (callback_) {
        ret = callback_(offset, source);
    }
    return ret;
}

void Scrollable::ProcessScrollOverCallback(double velocity)
{
    if (outBoundaryCallback_ && !outBoundaryCallback_() && !canOverScroll_) {
        return;
    }
    // In the case of chain animation enabled, you need to switch the control point first,
    // and then correct the offset value in notification process
    if (notifyScrollOverCallback_) {
        notifyScrollOverCallback_(velocity);
    }
    // then use corrected offset to make scroll motion.
    if (scrollOverCallback_) {
        scrollOverCallback_(velocity);
    }
}

bool Scrollable::HandleOverScroll(double velocity)
{
    auto parent = parent_.Upgrade();
    if (!parent || !nestedOpt_.NeedParent()) {
        if (edgeEffect_ != EdgeEffect::NONE) {
            ProcessScrollOverCallback(velocity);
            return true;
        }
        return false;
    }
    // parent handle over scroll first
    if ((velocity < 0 && (nestedOpt_.forward == NestedScrollMode::SELF_FIRST ||
        nestedOpt_.forward == NestedScrollMode::PARALLEL)) ||
        (velocity > 0 && (nestedOpt_.backward == NestedScrollMode::SELF_FIRST ||
        nestedOpt_.forward == NestedScrollMode::PARALLEL))) {
        if (parent->HandleOverScroll(velocity)) {
            return true;
        }
        if (edgeEffect_ != EdgeEffect::NONE) {
            ProcessScrollOverCallback(velocity);
            return true;
        }
    }

    // self handle over scroll first
    if (edgeEffect_ != EdgeEffect::NONE) {
        ProcessScrollOverCallback(velocity);
        return true;
    }
    return parent->HandleOverScroll(velocity);
}

void Scrollable::SetSlipFactor(double SlipFactor)
{
    slipFactor_ = std::clamp(SlipFactor, -ADJUSTABLE_VELOCITY, ADJUSTABLE_VELOCITY);
}

const RefPtr<SpringProperty>& Scrollable::GetDefaultOverSpringProperty()
{
    return DEFAULT_OVER_SPRING_PROPERTY;
}

} // namespace OHOS::Ace
