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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H

#include <optional>

#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components_ng/pattern/text_picker/textpicker_accessibility_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_layout_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_paint_method.h"
#include "core/components_ng/pattern/text_picker/toss_animation_controller.h"

namespace OHOS::Ace::NG {
using EventCallback = std::function<void(bool)>;

namespace {
const Dimension TEXT_FOCUS_PAINT_WIDTH = 2.0_vp;
}
class TextPickerPattern : public LinearLayoutPattern {
    DECLARE_ACE_TYPE(TextPickerPattern, LinearLayoutPattern);

public:
    TextPickerPattern() : LinearLayoutPattern(false) {};

    ~TextPickerPattern() override = default;

    bool IsAtomicNode() const override
    {
        return true;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<TextPickerEventHub>();
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<TextPickerLayoutProperty>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<TextPickerAccessibilityProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto textPickerPaintMethod = MakeRefPtr<TextPickerPaintMethod>();
        textPickerPaintMethod->SetDefaultPickerItemHeight(CalculateHeight());
        textPickerPaintMethod->SetEnabled(enabled_);
        textPickerPaintMethod->SetBackgroundColor(backgroundColor_);
        return textPickerPaintMethod;
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<LinearLayoutAlgorithm>();
    }

    void SetEventCallback(EventCallback&& value);

    void FireChangeEvent(bool refresh);

    void OnColumnsBuilding();

    void FlushOptions();

    void SetDefaultPickerItemHeight();

    RefPtr<FrameNode> GetColumnNode();

    uint32_t GetShowOptionCount() const;

    void SetSelected(uint32_t value)
    {
        selectedIndex_ = value;
    }

    uint32_t GetSelected() const
    {
        return selectedIndex_;
    }

    void SetRange(const std::vector<NG::RangeContent>& value)
    {
        if (value.empty()) {
            LOGE("input value of range is empty.");
            return;
        }
        range_.clear();
        for (size_t i = 0; i < value.size(); i++) {
            RangeContent content;
            content.icon_ = value[i].icon_;
            content.text_ = value[i].text_;
            range_.emplace_back(content);
        }
    }

    std::vector<NG::RangeContent> GetRange() const
    {
        return range_;
    }

    void SetColumnsKind(uint32_t columnKind)
    {
        columnsKind_ = columnKind;
    }

    void ClearOption()
    {
        options_.clear();
    }

    void AppendOption(const NG::RangeContent& value)
    {
        options_.emplace_back(value);
    }

    uint32_t GetOptionCount() const
    {
        return options_.size();
    }

    std::string GetSelectedObject(bool isColumnChange, int32_t status = 0) const;

    std::string GetOption(uint32_t index) const
    {
        if (index >= GetOptionCount()) {
            LOGE("index out of range.");
            return "";
        }
        return options_[index].text_;
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    FocusPattern GetFocusPattern() const override
    {
        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_RETURN(pipeline, FocusPattern());
        auto pickerTheme = pipeline->GetTheme<PickerTheme>();
        CHECK_NULL_RETURN(pickerTheme, FocusPattern());
        auto focusColor = pickerTheme->GetFocusColor();

        FocusPaintParam focusPaintParams;
        focusPaintParams.SetPaintColor(focusColor);
        focusPaintParams.SetPaintWidth(TEXT_FOCUS_PAINT_WIDTH);

        return { FocusType::NODE, true, FocusStyleType::CUSTOM_REGION, focusPaintParams };
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override;
private:
    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    bool OnKeyEvent(const KeyEvent& event);
    bool HandleDirectionKey(KeyCode code);
    double CalculateHeight();

    void InitDisabled();
    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void PaintFocusState();
    void SetButtonIdeaSize();
    std::string GetRangeStr() const;

    bool enabled_ = true;
    double defaultPickerItemHeight_;
    uint32_t selectedIndex_ = 0;
    std::vector<NG::RangeContent> range_;
    std::vector<NG::RangeContent> options_;
    uint32_t columnsKind_;
    Color backgroundColor_ = Color::WHITE;

    ACE_DISALLOW_COPY_AND_MOVE(TextPickerPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H
