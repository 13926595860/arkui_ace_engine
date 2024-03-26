/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/text_picker/textpicker_paint_method.h"

#include "core/components/common/properties/color.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/text_picker/textpicker_layout_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

namespace {
constexpr uint8_t ENABLED_ALPHA = 255;
constexpr uint8_t DISABLED_ALPHA = 102;
constexpr uint8_t DOUBLE = 2;
const Dimension PICKER_DIALOG_DIVIDER_MARGIN = 24.0_vp;
} // namespace

CanvasDrawFunction TextPickerPaintMethod::GetForegroundDrawFunction(PaintWrapper* paintWrapper)
{
    const auto& geometryNode = paintWrapper->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, nullptr);
    auto frameRect = geometryNode->GetFrameRect();

    auto renderContext = paintWrapper->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, nullptr);
    auto pickerNode = renderContext->GetHost();
    CHECK_NULL_RETURN(pickerNode, nullptr);
    auto layoutProperty = pickerNode->GetLayoutProperty<TextPickerLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, nullptr);

    return
        [weak = WeakClaim(this), layoutProperty, frameRect, enabled = enabled_, pattern = pattern_](RSCanvas& canvas) {
            auto picker = weak.Upgrade();
            CHECK_NULL_VOID(picker);
            auto textPickerPattern = DynamicCast<TextPickerPattern>(pattern.Upgrade());
            CHECK_NULL_VOID(textPickerPattern);

            PaddingPropertyF padding = layoutProperty->CreatePaddingAndBorder();
            RectF contentRect = { padding.left.value_or(0), padding.top.value_or(0),
                frameRect.Width() - padding.Width(), frameRect.Height() - padding.Height() };

            double dividerHeight = picker->defaultPickerItemHeight_;
            if (textPickerPattern->GetResizeFlag()) {
                dividerHeight = textPickerPattern->GetResizePickerItemHeight();
            }

            if (contentRect.Width() >= 0.0f && (contentRect.Height() >= dividerHeight)) {
                if (textPickerPattern->GetCustomDividerFlag()) {
                    auto divider = textPickerPattern->GetDivider();
                    picker->PaintCustomDividerLines(canvas, contentRect, frameRect, divider, dividerHeight);
                } else {
                    picker->PaintDefaultDividerLines(canvas, contentRect, dividerHeight);
                }
            }
            if (!enabled) {
                picker->PaintDisable(canvas, frameRect.Width(), frameRect.Height());
            }
        };
}

void TextPickerPaintMethod::PaintCustomDividerLines(RSCanvas& canvas, const RectF &contentRect, const RectF &frameRect,
    const ItemDivider &divider, double dividerHeight)
{
    DividerInfo info;
    if (NeedPaintDividerLines(contentRect, divider, dividerHeight, info)) {
        PaintDividerLines(canvas, contentRect, info);
    } else {
        PaintDisable(canvas, frameRect.Width(), frameRect.Height());
    }
}

void TextPickerPaintMethod::PaintDefaultDividerLines(RSCanvas& canvas, const RectF &contentRect,
    double dividerHeight)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<PickerTheme>();
    auto dividerColor = theme->GetDividerColor();
    auto dividerLineWidth = theme->GetDividerThickness().ConvertToPx();

    auto dividerLength = contentRect.Width();
    auto dividerMargin = contentRect.GetX();
    auto textPickerPattern = DynamicCast<TextPickerPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textPickerPattern);
    if (textPickerPattern->GetIsShowInDialog()) {
        dividerLength -= PICKER_DIALOG_DIVIDER_MARGIN.ConvertToPx() * DOUBLE;
        dividerMargin += PICKER_DIALOG_DIVIDER_MARGIN.ConvertToPx();
    }
    
    DividerInfo info;
    info.dividerColor = dividerColor;
    info.dividerWidth = dividerLineWidth;
    info.dividerLength = dividerLength;
    info.dividerMargin = dividerMargin;
    info.dividerHeight = dividerHeight;
    PaintDividerLines(canvas, contentRect, info);
}

bool TextPickerPaintMethod::NeedPaintDividerLines(const RectF &contentRect, const ItemDivider &divider,
    double dividerHeight, DividerInfo& info)
{
    if (divider.strokeWidth.ConvertToPx() > contentRect.Height()) {
        auto pipeline = PipelineContext::GetCurrentContext();
        if (!pipeline) {
            return false;
        }
        auto theme = pipeline->GetTheme<PickerTheme>();
        if (theme) {
            info.dividerWidth = theme->GetDividerThickness().ConvertToPx();
        } else {
            info.dividerWidth = 0.0f;
        }
    } else {
        info.dividerWidth = divider.strokeWidth.ConvertToPx();
    }
    
    if (info.dividerWidth <= 0) {
        return false;
    }

    info.dividerHeight = dividerHeight;
    info.startMargin = std::max(0.0, divider.startMargin.ConvertToPx());
    info.endMargin = std::max(0.0, divider.endMargin.ConvertToPx());
    info.dividerColor = divider.color;

    auto dividerLength = contentRect.Width();
    auto dividerMargin = contentRect.GetX();
    auto textPickerPattern = DynamicCast<TextPickerPattern>(pattern_.Upgrade());
    if (!textPickerPattern) {
        return false;
    }
    if (textPickerPattern->GetIsShowInDialog()) {
        dividerLength -= PICKER_DIALOG_DIVIDER_MARGIN.ConvertToPx() * DOUBLE;
        dividerMargin += PICKER_DIALOG_DIVIDER_MARGIN.ConvertToPx();
    }

    float checkMargin = dividerLength - info.startMargin - info.endMargin;
    if (NearZero(checkMargin)) {
        return false;
    }
    if (LessNotEqual(checkMargin, 0.0f)) {
        LOGE("StartMagin and endMargin are set to 0, because the parameters are wrong");
        info.startMargin = 0.0f;
        info.endMargin = 0.0f;
    }
    dividerMargin += info.startMargin;
    dividerLength = dividerLength - info.startMargin - info.endMargin;
    info.dividerMargin = dividerMargin;
    info.dividerLength = dividerLength;
    return true;
}

void TextPickerPaintMethod::PaintDividerLines(RSCanvas& canvas, RectF contentRect, const DividerInfo &info)
{
    DividerPainter dividerPainter(info.dividerWidth, info.dividerLength, false, info.dividerColor, LineCap::SQUARE);
    double upperLine = (contentRect.Height() - info.dividerHeight) / 2.0 + contentRect.GetY();
    double downLine = (contentRect.Height() + info.dividerHeight) / 2.0 + contentRect.GetY();

    OffsetF offset = OffsetF(info.dividerMargin, upperLine);
    dividerPainter.DrawLine(canvas, offset);
    OffsetF offsetY = OffsetF(info.dividerMargin, downLine);
    dividerPainter.DrawLine(canvas, offsetY);
}

void TextPickerPaintMethod::PaintDisable(RSCanvas& canvas, double X, double Y)
{
    double centerY = Y;
    double centerX = X;
    RSRect rRect(0, 0, centerX, centerY);
    RSPath path;
    path.AddRoundRect(rRect, 0, 0, RSPathDirection::CW_DIRECTION);
    RSPen pen;
    RSBrush brush;
    brush.SetColor(float(DISABLED_ALPHA) / ENABLED_ALPHA);
    pen.SetColor(float(DISABLED_ALPHA) / ENABLED_ALPHA);
    canvas.AttachBrush(brush);
    canvas.AttachPen(pen);
    canvas.DrawPath(path);
    canvas.DetachPen();
    canvas.DetachBrush();
}
} // namespace OHOS::Ace::NG
