/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SWIPER_SWIPER_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SWIPER_SWIPER_PATTERN_H

#include <optional>

#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/constants.h"
#include "core/components/swiper/swiper_controller.h"
#include "core/components/swiper/swiper_indicator_theme.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/swiper/swiper_accessibility_property.h"
#include "core/components_ng/pattern/swiper/swiper_event_hub.h"
#include "core/components_ng/pattern/swiper/swiper_layout_algorithm.h"
#include "core/components_ng/pattern/swiper/swiper_layout_property.h"
#include "core/components_ng/pattern/swiper/swiper_model.h"
#include "core/components_ng/pattern/swiper/swiper_paint_method.h"
#include "core/components_ng/pattern/swiper/swiper_paint_property.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::NG {
class SwiperPattern : public Pattern {
    DECLARE_ACE_TYPE(SwiperPattern, Pattern);

public:
    SwiperPattern();
    ~SwiperPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    bool UsResRegion() override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<SwiperLayoutProperty>();
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<SwiperPaintProperty>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<SwiperAccessibilityProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        auto layoutAlgorithm = MakeRefPtr<SwiperLayoutAlgorithm>();
        layoutAlgorithm->SetIsLoop(IsLoop());
        layoutAlgorithm->SetCurrentOffsetTimes(
            !IsLoop() && GetEdgeEffect() == EdgeEffect::FADE
                ? std::clamp(currentOffsetTimes_, 0.0f, static_cast<float>(TotalCount() - GetDisplayCount()))
                : currentOffsetTimes_);
        layoutAlgorithm->SetTotalCount(TotalCount());
        layoutAlgorithm->SetDisplayCount(GetDisplayCount());
        layoutAlgorithm->SetHoverRatio(hoverRatio_);
        layoutAlgorithm->SetMaxChildSize(maxChildSize_);
        layoutAlgorithm->SetItemRange(itemRange_);
        layoutAlgorithm->SetOnlyNeedMeasurePages(onlyNeedMeasurePages_);
        return layoutAlgorithm;
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto layoutProperty = GetLayoutProperty<SwiperLayoutProperty>();
        CHECK_NULL_RETURN(layoutProperty, nullptr);
        const auto& paddingProperty = layoutProperty->GetPaddingProperty();
        bool needClipPadding = paddingProperty != nullptr;
        bool needPaintFade = !IsLoop() && GetEdgeEffect() == EdgeEffect::FADE && overstepBoundaryOffsetTimes_ != 0.0f;
        auto paintMethod = MakeRefPtr<SwiperPaintMethod>(
            GetDirection(), overstepBoundaryOffsetTimes_ * -1.0f * maxChildSize_.MainSize(GetDirection()));
        paintMethod->SetNeedPaintFade(needPaintFade);
        paintMethod->SetNeedClipPadding(needClipPadding);
        return paintMethod;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<SwiperEventHub>();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        Pattern::ToJsonValue(json);
        json->Put("currentOffsetTimes", currentOffsetTimes_);

        if (indicatorIsBoolean_) {
            return;
        }

        auto indicatorType = GetIndicatorType();
        if (indicatorType == SwiperIndicatorType::DOT) {
            json->Put("indicator", GetDotIndicatorStyle().c_str());
        } else {
            json->Put("indicator", GetDigitIndicatorStyle().c_str());
        }
    }

    void FromJson(const std::unique_ptr<JsonValue>& json) override;

    std::string GetDotIndicatorStyle() const
    {
        auto swiperParameters = GetSwiperParameters();
        CHECK_NULL_RETURN(swiperParameters, "");
        auto jsonValue = JsonUtil::Create(true);
        jsonValue->Put("left", swiperParameters_->dimLeft.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("top", swiperParameters_->dimTop.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("right", swiperParameters_->dimRight.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("bottom", swiperParameters_->dimBottom.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("itemWidth", swiperParameters_->itemWidth.value_or(6.0_vp).ToString().c_str());
        jsonValue->Put("itemHeight", swiperParameters_->itemHeight.value_or(6.0_vp).ToString().c_str());
        jsonValue->Put("selectedItemWidth", swiperParameters_->selectedItemWidth.value_or(6.0_vp).ToString().c_str());
        jsonValue->Put("selectedItemHeight", swiperParameters_->selectedItemHeight.value_or(6.0_vp).ToString().c_str());
        jsonValue->Put("selectedColor",
            swiperParameters_->selectedColorVal.value_or(Color::FromString("#ff007dff")).ColorToString().c_str());
        jsonValue->Put(
            "color", swiperParameters_->colorVal.value_or(Color::FromString("#19182431")).ColorToString().c_str());
        jsonValue->Put("mask", swiperParameters_->maskValue ? "true" : "false");
        return jsonValue->ToString();
    }

    std::string GetDigitIndicatorStyle() const
    {
        auto swiperParameters = GetSwiperDigitalParameters();
        CHECK_NULL_RETURN(swiperParameters, "");
        auto jsonValue = JsonUtil::Create(true);
        auto pipelineContext = PipelineBase::GetCurrentContext();
        CHECK_NULL_RETURN(pipelineContext, "");
        auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
        CHECK_NULL_RETURN(swiperIndicatorTheme, "");
        jsonValue->Put("left", swiperDigitalParameters_->dimLeft.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("top", swiperDigitalParameters_->dimTop.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("right", swiperDigitalParameters_->dimRight.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("bottom", swiperDigitalParameters_->dimBottom.value_or(0.0_vp).ToString().c_str());
        jsonValue->Put("fontSize", swiperDigitalParameters_->fontSize.value_or(
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize()).ToString().c_str());
        jsonValue->Put("fontColor", swiperDigitalParameters_->fontColor.value_or(
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor()).ColorToString().c_str());
        jsonValue->Put("fontWeight", V2::ConvertWrapFontWeightToStirng(
            swiperDigitalParameters_->fontWeight.value_or(FontWeight::NORMAL)).c_str());
        jsonValue->Put("selectedFontSize", swiperDigitalParameters_->selectedFontSize.value_or(
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize()).ToString().c_str());
        jsonValue->Put("selectedFontColor", swiperDigitalParameters_->selectedFontColor.value_or(
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor()).ColorToString().c_str());
        jsonValue->Put("selectedFontWeight", V2::ConvertWrapFontWeightToStirng(
            swiperDigitalParameters_->selectedFontWeight.value_or(FontWeight::NORMAL)).c_str());
        return jsonValue->ToString();
    }

    int32_t GetCurrentShownIndex() const
    {
        return currentIndex_;
    }

    RefPtr<SwiperController> GetSwiperController() const
    {
        return swiperController_;
    }

    void SetSwiperController(const RefPtr<SwiperController>& swiperController)
    {
        swiperController_ = swiperController;
    }

    int32_t GetCurrentIndex() const
    {
        return currentIndex_;
    }

    float GetTurnPageRate() const
    {
        float turnPageRate = static_cast<float>(currentIndex_);
        if (moveDirection_ == MoveDirection::ADVANCE) {
            turnPageRate = std::fmod(currentOffsetTimes_, 1) * -1;
        } else if (moveDirection_ == MoveDirection::ADVANCE) {
            turnPageRate = 1.0f - std::fmod(currentOffsetTimes_, 1);
        }
        return turnPageRate;
    }

    void SetIndicatorDoingAnimation(bool indicatorDoingAnimation)
    {
        indicatorDoingAnimation_ = indicatorDoingAnimation;
    }

    int32_t TotalCount() const;

    Axis GetDirection() const;

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    ScopeFocusAlgorithm GetScopeFocusAlgorithm() override
    {
        return ScopeFocusAlgorithm(direction_ != Axis::HORIZONTAL, true, ScopeType::OTHERS,
            [wp = WeakClaim(this)](
                FocusStep step, const WeakPtr<FocusHub>& currFocusNode, WeakPtr<FocusHub>& nextFocusNode) {
                auto swiper = wp.Upgrade();
                if (swiper) {
                    nextFocusNode = swiper->GetNextFocusNode(step, currFocusNode);
                }
            });
    }

    void UpdateChangeEvent(ChangeEvent&& event)
    {
        if (!changeEvent_) {
            changeEvent_ = std::make_shared<ChangeEvent>(event);
            auto eventHub = GetEventHub<SwiperEventHub>();
            CHECK_NULL_VOID(eventHub);
            eventHub->AddOnChangeEvent(changeEvent_);
        } else {
            (*changeEvent_).swap(event);
        }
    }

    void UpdateOnChangeEvent(ChangeEvent&& event)
    {
        if (!onIndexChangeEvent_) {
            onIndexChangeEvent_ = std::make_shared<ChangeEvent>(event);
            auto eventHub = GetEventHub<SwiperEventHub>();
            CHECK_NULL_VOID(eventHub);
            eventHub->AddOnChangeEvent(onIndexChangeEvent_);
        } else {
            (*onIndexChangeEvent_).swap(event);
        }
    }

    void SetSwiperParameters(const SwiperParameters& swiperParameters)
    {
        swiperParameters_ = std::make_shared<SwiperParameters>(swiperParameters);
    }

    void SetSwiperDigitalParameters(const SwiperDigitalParameters& swiperDigitalParameters)
    {
        swiperDigitalParameters_ = std::make_shared<SwiperDigitalParameters>(swiperDigitalParameters);
    }

    void ShowNext();
    void ShowPrevious();
    void SwipeTo(int32_t index);

    void OnVisibleChange(bool isVisible) override;

    int32_t GetStartIndex() const
    {
        return startIndex_;
    }

    int32_t GetEndIndex() const
    {
        return endIndex_;
    }

    bool HasIndicatorNode() const
    {
        return indicatorId_.has_value();
    }

    bool HasLeftButtonNode() const
    {
        return leftButtonId_.has_value();
    }

    bool HasRightButtonNode() const
    {
        return rightButtonId_.has_value();
    }

    int32_t GetIndicatorId()
    {
        if (!indicatorId_.has_value()) {
            indicatorId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return indicatorId_.value();
    }

    int32_t GetLeftButtonId()
    {
        if (!leftButtonId_.has_value()) {
            leftButtonId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return leftButtonId_.value();
    }

    int32_t GetRightButtonId()
    {
        if (!rightButtonId_.has_value()) {
            rightButtonId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return rightButtonId_.value();
    }

    void RemoveIndicatorNode()
    {
        CHECK_NULL_VOID_NOLOG(HasIndicatorNode());
        auto swiperNode = GetHost();
        CHECK_NULL_VOID(swiperNode);
        swiperNode->RemoveChildAtIndex(swiperNode->GetChildIndexById(GetIndicatorId()));
        indicatorId_ = std::nullopt;
    }

    void RemoveLeftButtonNode()
    {
        CHECK_NULL_VOID_NOLOG(HasLeftButtonNode());
        auto swiperNode = GetHost();
        CHECK_NULL_VOID(swiperNode);
        swiperNode->RemoveChildAtIndex(swiperNode->GetChildIndexById(GetLeftButtonId()));
        leftButtonId_ = std::nullopt;
    }

    void RemoveRightButtonNode()
    {
        CHECK_NULL_VOID_NOLOG(HasRightButtonNode());
        auto swiperNode = GetHost();
        CHECK_NULL_VOID(swiperNode);
        swiperNode->RemoveChildAtIndex(swiperNode->GetChildIndexById(GetRightButtonId()));
        rightButtonId_ = std::nullopt;
    }

    SwiperIndicatorType GetIndicatorType() const;

    bool IsIndicatorCustomSize() const
    {
        return IsCustomSize_;
    }

    void SetIsIndicatorCustomSize(bool IsCustomSize)
    {
        IsCustomSize_ = IsCustomSize;
    }

    void SetIndicatorIsBoolean(bool isBoolean)
    {
        indicatorIsBoolean_ = isBoolean;
    }

    const std::shared_ptr<AnimationUtils::Animation>& GetAnimation() const
    {
        return animation_;
    }

    std::shared_ptr<SwiperParameters> GetSwiperParameters() const;
    std::shared_ptr<SwiperDigitalParameters> GetSwiperDigitalParameters() const;

    void ArrowHover(bool hoverFlag);
    void IndicatorHover(bool hoverFlag);
    bool IsLoop() const;
    bool IsEnabled() const;
    void OnWindowShow() override;
    void OnWindowHide() override;

private:
    void AttachNodeAnimatableProperty();
    void UpdateCurrentOffsetTimes(float value);
    void HandleAnimationEnds();
    void CalculateItemRange(int32_t index);
    void PlayTranslateAnimation(int32_t duration);
    void ForcedStopTranslateAnimation();
    void GoAutoPlay();
    void ForcedFinishAutoPlay();
    void HandleFinishAutoPlayAnimationEnds();

    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    // Init pan recognizer to move items when drag update, play translate animation when drag end.
    void InitPanEvent();

    // Init touch event, stop animation when touch down.
    void InitTouchEvent();

    // Init on key event
    void InitOnKeyEvent();
    bool OnKeyEvent(const KeyEvent& event);
    void FlushFocus(const RefPtr<FrameNode>& curShowFrame);
    WeakPtr<FocusHub> GetNextFocusNode(FocusStep step, const WeakPtr<FocusHub>& currentFocusNode);

    // Init controller of swiper, controller support showNext, showPrevious and finishAnimation interface.
    void InitSwiperController();

    // Init indicator
    void InitIndicator();
    void InitArrow();

    void HandleDragStart();
    void HandleDragUpdate(const GestureEvent& info);
    void HandleDragEnd(double dragVelocity);

    void HandleTouchEvent(const TouchEventInfo& info);
    void HandleTouchDown();
    void HandleTouchUp();

    // Implement of swiper controller
    void SwipeToWithoutAnimation(int32_t index);
    void FinishAnimation();

    void StartAutoPlay();
    void FireChangeEvent() const;
    void FireAnimationStartEvent() const;
    void FireAnimationEndEvent() const;

    const std::optional<LayoutConstraintF>& GetLayoutConstraint() const;
    RefPtr<GestureEventHub> GetGestureHub() const;
    float GetItemSpace() const;
    int32_t CurrentIndex() const;
    int32_t GetDisplayCount() const;
    int32_t GetCachedCount() const;
    int32_t GetDuration() const;
    int32_t GetInterval() const;
    RefPtr<Curve> GetCurve() const;
    EdgeEffect GetEdgeEffect() const;
    bool IsAutoPlay() const;
    bool IsDisableSwipe() const;
    bool IsShowIndicator() const;
    void OnIndexChange() const;
    bool IsOutOfHotRegion(const PointF& dragPoint) const;
    bool IsOutOfIndicatorZone(const PointF& dragPoint);
    void SaveDotIndicatorProperty(const RefPtr<FrameNode>& indicatorNode);
    void SaveDigitIndicatorProperty(const RefPtr<FrameNode>& indicatorNode);
    void RegisterVisibleAreaChange();
    bool NeedAutoPlay() const;
    bool IsShowArrow() const;
    void SaveArrowProperty(const RefPtr<FrameNode>& arrowNode);
    RefPtr<FocusHub> GetFocusHubChild(std::string childFrameName);
    WeakPtr<FocusHub> PreviousFocus(const RefPtr<FocusHub>& curFocusNode);
    WeakPtr<FocusHub> NextFocus(const RefPtr<FocusHub>& curFocusNode);
    void SetAccessibilityAction();

    RefPtr<PanEvent> panEvent_;
    RefPtr<TouchEventImpl> touchEvent_;

    RefPtr<SwiperController> swiperController_;

    bool isLastIndicatorFocused_ = false;
    int32_t startIndex_ = 0;
    int32_t endIndex_ = 0;

    PanDirection panDirection_;
    float distance_ = 0.0f;

    bool indicatorDoingAnimation_ = false;
    bool isInit_ = true;
    bool hasVisibleChangeRegistered_ = false;
    bool isVisible_ = true;
    bool IsCustomSize_ = false;
    bool indicatorIsBoolean_ = true;

    Axis direction_ = Axis::HORIZONTAL;

    ChangeEventPtr changeEvent_;
    ChangeEventPtr onIndexChangeEvent_;

    mutable std::shared_ptr<SwiperParameters> swiperParameters_;
    mutable std::shared_ptr<SwiperDigitalParameters> swiperDigitalParameters_;

    WeakPtr<FrameNode> lastWeakShowNode_;

    CancelableCallback<void()> translateTask_;
    // Arrow default hover ratio
    float hoverRatio_ = 1.0f;
    std::optional<int32_t> indicatorId_;
    std::optional<int32_t> leftButtonId_;
    std::optional<int32_t> rightButtonId_;
    std::optional<SwiperIndicatorType> lastSwiperIndicatorType_;

    enum MoveDirection { ADVANCE, STATIC, RETREAT } moveDirection_ = MoveDirection::STATIC;

    std::shared_ptr<AnimationUtils::Animation> animation_;

    int32_t oldIndex_ = 0;
    int32_t currentIndex_ = 0;
    int32_t targetIndex_ = 0;
    std::set<int32_t> itemRange_;
    SizeF maxChildSize_ = { 0, 0 };

    bool needCalculateItemRange_ = true;
    float currentOffsetTimes_ = 0;
    float overstepBoundaryOffsetTimes_ = 0;
    bool needUpdateCurrentOffsetTimes_ = true;
    bool translateAnimationIsRunning_ = false;
    bool returnToOriginalAnimation_ = true;
    bool playingAutoPlay_ = false;
    bool onlyNeedMeasurePages_ = false;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SWIPER_SWIPER_PATTERN_H
