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

#include "core/components/text_overlay/rosen_render_text_overlay.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImage.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPath.h"
#endif

#include "core/components/common/painter/rosen_decoration_painter.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/text_field/rosen_render_text_field.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension CLIP_ORIGIN_X = 4.5754_vp;
constexpr Dimension CLIP_ORIGIN_Y = 6.3432_vp;
#ifndef USE_ROSEN_DRAWING
constexpr Dimension CLIP_LENGTH = 4.0_vp;
#endif
constexpr Dimension MAGNIFIER_WIDTH = 48.0_vp;
constexpr Dimension MAGNIFIER_RADIUS = 24.0_vp;
constexpr Dimension MAGNIFIER_OFFSET = 7.0_vp;
constexpr Dimension FIRST_DRAG_OFFSET = 19.0_vp;
constexpr Dimension HANDLE_LINE_WIDTH = 1.5_vp;
constexpr Dimension MORE_BUTTON_SIZE = 40.0_vp;
constexpr double MAGNIFIER_GAIN = 1.25;
constexpr double FIXED_OFFSET = 18.0;

} // namespace

class RenderWeb;
void RosenRenderTextOverlay::Paint(RenderContext& context, const Offset& offset)
{
    if (!showOption_.showStartHandle && !showOption_.showEndHandle && !isUsedByWeb_) {
        return;
    }

    PaintHandles(context);

    AdjustForAnimation();

    if ((!isSingleHandle_ || showOption_.showMenu) && !showMagnifier_) {
        RenderNode::Paint(context, offset);
    }

    PaintMore(context);
}

void RosenRenderTextOverlay::PaintHandles(RenderContext& context) const
{
    auto renderContext = AceType::DynamicCast<RosenRenderContext>(&context);
    if (!renderContext) {
        return;
    }
    auto canvas = renderContext->GetCanvas();
    if (!canvas) {
        return;
    }
    Offset centerOffset = Offset(0.0, NormalizeToPx(handleRadius_));

    // Paint one handle.
    if (isSingleHandle_ && clipRect_.IsInRegion(Point(startHandleOffset_.GetX(), startHandleOffset_.GetY()))) {
        PaintHandle(canvas, startHandleOffset_ + centerOffset);
        return;
    }

    // Paint start handle of textOverlay.
    if (showOption_.showStartHandle &&
        clipRect_.IsInRegion(Point(startHandleOffset_.GetX(), startHandleOffset_.GetY()))) {
        PaintHandle(canvas,
            startHandleOffset_ + Offset(0.0, -startHandleHeight_.value_or(lineHeight_) - NormalizeToPx(handleRadius_)),
            true);
    }

    // Paint end handle of textOverlay.
    if (showOption_.showEndHandle && clipRect_.IsInRegion(Point(endHandleOffset_.GetX(), endHandleOffset_.GetY()))) {
        PaintHandle(canvas, endHandleOffset_ + centerOffset);
    }
}

#ifndef USE_ROSEN_DRAWING
void RosenRenderTextOverlay::PaintHandle(SkCanvas* skCanvas, Offset centerOffset, bool isLeftHandle) const
{
    if (!skCanvas) {
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    skCanvas->save();
    skCanvas->translate(centerOffset.GetX(), centerOffset.GetY());
    // Paint outer circle.
    paint.setColor(handleColor_.GetValue());
    skCanvas->drawCircle(0.0, 0.0, NormalizeToPx(handleRadius_), paint);
    // Paint inner circle.
    paint.setColor(handleColorInner_.GetValue());
    skCanvas->drawCircle(0.0, 0.0, NormalizeToPx(handleRadiusInner_), paint);
    // Paint line of handle.
    paint.setAntiAlias(true);
    paint.setColor(handleColor_.GetValue());
    paint.setStrokeWidth(NormalizeToPx(HANDLE_LINE_WIDTH));
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    // 1.0 is avoid separation of handle circle and handle line.
    Offset startPoint(Offset(0.0, NormalizeToPx(-handleRadius_) + 1.0));
    // 1.0_dp is designed by UX, handle line is higher than height of select region.
    Offset endPoint(
        Offset(0.0, NormalizeToPx(-handleRadius_) - endHandleHeight_.value_or(lineHeight_) - NormalizeToPx(1.0_vp)));
    if (isLeftHandle) {
        startPoint.SetY(NormalizeToPx(handleRadius_) - 1.0);
        endPoint.SetY(NormalizeToPx(handleRadius_) + startHandleHeight_.value_or(lineHeight_) + NormalizeToPx(1.0_vp));
    }
    skCanvas->drawLine(startPoint.GetX(), startPoint.GetY(), endPoint.GetX(), endPoint.GetY(), paint);
    skCanvas->restore();
}
#else
void RosenRenderTextOverlay::PaintHandle(RSCanvas* canvas, Offset centerOffset, bool isLeftHandle) const
{
    if (!canvas) {
        return;
    }

    RSBrush brush;
    brush.SetAntiAlias(true);
    canvas->Save();
    canvas->Translate(centerOffset.GetX(), centerOffset.GetY());
    // Paint outer circle.
    brush.SetColor(handleColor_.GetValue());
    canvas->AttachBrush(brush);
    canvas->DrawCircle(RSPoint(0.0, 0.0), NormalizeToPx(handleRadius_));
    canvas->DetachBrush();
    // Paint inner circle.
    brush.SetColor(handleColorInner_.GetValue());
    canvas->AttachBrush(brush);
    canvas->DrawCircle(RSPoint(0.0, 0.0), NormalizeToPx(handleRadiusInner_));
    canvas->DetachBrush();
    // Paint line of handle.
    RSPen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(handleColor_.GetValue());
    pen.SetWidth(NormalizeToPx(HANDLE_LINE_WIDTH));
    pen.SetCapStyle(RSPen::CapStyle::ROUND_CAP);
    // 1.0 is avoid separation of handle circle and handle line.
    Offset startPoint(Offset(0.0, NormalizeToPx(-handleRadius_) + 1.0));
    // 1.0_dp is designed by UX, handle line is higher than height of select region.
    Offset endPoint(
        Offset(0.0, NormalizeToPx(-handleRadius_) - endHandleHeight_.value_or(lineHeight_) - NormalizeToPx(1.0_vp)));
    if (isLeftHandle) {
        startPoint.SetY(NormalizeToPx(handleRadius_) - 1.0);
        endPoint.SetY(NormalizeToPx(handleRadius_) + startHandleHeight_.value_or(lineHeight_) + NormalizeToPx(1.0_vp));
    }
    canvas->AttachPen(pen);
    canvas->DrawLine(RSPoint(startPoint.GetX(), startPoint.GetY()), RSPoint(endPoint.GetX(), endPoint.GetY()));
    canvas->DetachPen();
    canvas->Restore();
}
#endif

void RosenRenderTextOverlay::PaintMagnifier(RenderContext& context)
{
    auto textField = weakTextField_.Upgrade();
    auto rosenTextField = AceType::DynamicCast<RosenRenderTextField>(textField);
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    auto pipelineContext = context_.Upgrade();
    if (!canvas || !rosenTextField || !pipelineContext) {
        return;
    }
    auto bitmap = rosenTextField->GetBitmap();
    double viewScale = pipelineContext->GetViewScale();
    double globalX = 0.0;
    double globalY = 0.0;
    double x = 0.0;
    double y = 0.0;
    if (isTouchEndDrag_ || isSingleHandle_) {
        auto caretRect = rosenTextField->GetCaretRect();
        auto textFieldGlobalOffset = rosenTextField->GetGlobalOffset();
        globalX = textFieldGlobalOffset.GetX() + caretRect.GetOffset().GetX() + caretRect.Width() / 2 -
                  NormalizeToPx(MAGNIFIER_RADIUS);
        globalY = textFieldGlobalOffset.GetY() + caretRect.GetOffset().GetY() - NormalizeToPx(MAGNIFIER_WIDTH) -
                  NormalizeToPx(MAGNIFIER_OFFSET);
        if (!isSingleHandle_ && NeedAdjustEndHandle()) {
            globalY -= NormalizeToPx(FIRST_DRAG_OFFSET - MAGNIFIER_OFFSET);
        }
        x = caretRect.GetOffset().GetX() + caretRect.Width() / 2 - NormalizeToPx(MAGNIFIER_RADIUS);
        y = caretRect.GetOffset().GetY() + caretRect.Height() / 2 - NormalizeToPx(MAGNIFIER_RADIUS);
    } else if (isTouchStartDrag_) {
        auto caretRect = rosenTextField->GetStartRect();
        auto textFieldGlobalOffset = rosenTextField->GetGlobalOffset();
        globalX = textFieldGlobalOffset.GetX() + caretRect.GetOffset().GetX() + caretRect.Width() / 2 -
                  NormalizeToPx(MAGNIFIER_RADIUS);
        globalY = textFieldGlobalOffset.GetY() + caretRect.GetOffset().GetY() - NormalizeToPx(MAGNIFIER_WIDTH) -
                  NormalizeToPx(FIRST_DRAG_OFFSET);

        if (!isSingleHandle_ && NeedAdjustStartHandle()) {
            globalY += NormalizeToPx(FIRST_DRAG_OFFSET - MAGNIFIER_OFFSET);
        }
        x = caretRect.GetOffset().GetX() + caretRect.Width() / 2 - NormalizeToPx(MAGNIFIER_RADIUS);
        y = caretRect.GetOffset().GetY() + caretRect.Height() / 2 - NormalizeToPx(MAGNIFIER_RADIUS);
    } else {
        return;
    }

    globalX = LessOrEqual(globalX, 0.0) ? 0.0 : globalX;
    globalY = LessOrEqual(globalY, 0.0) ? 0.0 : globalY;

#ifndef USE_ROSEN_DRAWING
    SkRRect rrect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(globalX, globalY, NormalizeToPx(MAGNIFIER_WIDTH), NormalizeToPx(MAGNIFIER_WIDTH)),
        NormalizeToPx(MAGNIFIER_WIDTH), NormalizeToPx(MAGNIFIER_WIDTH));

    RosenDecorationPainter::PaintShadow(SkPath().addRRect(rrect), ShadowConfig::DefaultShadowM,
        static_cast<RosenRenderContext*>(&context)->GetRSNode());

    SkRRect ScaleRrect =
        SkRRect::MakeRectXY(SkRect::MakeXYWH(globalX * viewScale, globalY * viewScale,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale, NormalizeToPx(MAGNIFIER_WIDTH) * viewScale),
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale, NormalizeToPx(MAGNIFIER_WIDTH) * viewScale);

    canvas->save();
    canvas->scale(1.0 / viewScale, 1.0 / viewScale);
    SkPaint paint;
    canvas->clipRRect(ScaleRrect, true);

    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawRRect(ScaleRrect, paint);

    canvas->drawImageRect(bitmap.asImage(),
        SkRect::MakeXYWH(x * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET, y * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale, NormalizeToPx(MAGNIFIER_WIDTH) * viewScale),
        SkRect::MakeXYWH(globalX * viewScale, globalY * viewScale, NormalizeToPx(MAGNIFIER_WIDTH) * viewScale,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale),
        SkSamplingOptions(), nullptr, SkCanvas::kStrict_SrcRectConstraint);
    canvas->restore();
#else
    RSRoundRect rrect(
        RSRect(globalX, globalY, NormalizeToPx(MAGNIFIER_WIDTH) + globalX, NormalizeToPx(MAGNIFIER_WIDTH) + globalY),
        NormalizeToPx(MAGNIFIER_WIDTH), NormalizeToPx(MAGNIFIER_WIDTH));
    RSRecordingPath path;
    path.AddRoundRect(rrect);

    RosenDecorationPainter::PaintShadow(
        path, ShadowConfig::DefaultShadowM, static_cast<RosenRenderContext*>(&context)->GetRSNode());

    RSRoundRect ScaleRrect(RSRect(globalX * viewScale, globalY * viewScale,
                               NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + globalX * viewScale,
                               NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + globalY * viewScale),
        NormalizeToPx(MAGNIFIER_WIDTH) * viewScale, NormalizeToPx(MAGNIFIER_WIDTH) * viewScale);

    canvas->Save();
    canvas->Scale(1.0 / viewScale, 1.0 / viewScale);
    RSBrush brush;
    canvas->ClipRoundRect(ScaleRrect, RSClipOp::INTERSECT, true);

    brush.SetColor(RSColor::COLOR_WHITE);
    brush.SetAntiAlias(true);
    canvas->AttachBrush(brush);
    canvas->DrawRoundRect(ScaleRrect);
    canvas->DetachBrush();

    RSImage image;
    image.BuildFromBitmap(bitmap);
    canvas->DrawImageRect(image,
        RSRect(x * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET, y * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + (x * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET),
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + (y * viewScale * MAGNIFIER_GAIN + FIXED_OFFSET)),
        RSRect(globalX * viewScale, globalY * viewScale,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + globalX * viewScale,
            NormalizeToPx(MAGNIFIER_WIDTH) * viewScale + globalY * viewScale),
        RSSamplingOptions());
    canvas->Restore();
#endif
}

#ifndef USE_ROSEN_DRAWING
void RosenRenderTextOverlay::PaintClipLine(SkCanvas* skCanvas, SkPaint paint)
{
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return;
    }

    if (LessOrEqual(clipWidth_, 0.0)) {
        return;
    }

    paint.setStrokeWidth(NormalizeToPx(Dimension(clipWidth_, DimensionUnit::VP)));
    paint.setColor(clipColor_.GetValue());
    SkPath linePath;
    linePath.moveTo(SkDoubleToScalar(NormalizeToPx(CLIP_ORIGIN_X)), SkDoubleToScalar(NormalizeToPx(CLIP_ORIGIN_Y)));
    linePath.rLineTo(SkDoubleToScalar(NormalizeToPx(CLIP_LENGTH)), SkDoubleToScalar(NormalizeToPx(CLIP_LENGTH)));
    skCanvas->drawPath(linePath, paint);
}
#else
void RosenRenderTextOverlay::PaintClipLine(RSCanvas* canvas, RSPen pen)
{
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    if (LessOrEqual(clipWidth_, 0.0)) {
        return;
    }

    pen.SetWidth(NormalizeToPx(Dimension(clipWidth_, DimensionUnit::VP)));
    pen.SetColor(clipColor_.GetValue());
    RSRecordingPath linePath;
    linePath.MoveTo(static_cast<float>(NormalizeToPx(CLIP_ORIGIN_X)), static_cast<float>(NormalizeToPx(CLIP_ORIGIN_Y)));
    // Drawing is not supported
    canvas->AttachPen(pen);
    canvas->DrawPath(linePath);
    canvas->DetachPen();
}
#endif

void RosenRenderTextOverlay::InitMoreButtonPaintColor()
{
    RefPtr<TextOverlayTheme> theme = GetTheme<TextOverlayTheme>();
    if (theme) {
        clipColor_ = theme->GetMenuBackgroundColor();
        arrowColor_ = theme->GetMenuButtonTextStyle().GetTextColor();
    }
}

Offset RosenRenderTextOverlay::CalculateMoreButtonCenter()
{
    Offset offset = moreButtonPosition_;
    if (textDirection_ == TextDirection::RTL) {
        offset += Offset(NormalizeToPx(ICON_PADDING), NormalizeToPx(ICON_PADDING));
    } else {
        offset += Offset(-NormalizeToPx(ICON_SIZE + ICON_PADDING), NormalizeToPx(ICON_PADDING));
    }
    RefPtr<TextOverlayTheme> theme = GetTheme<TextOverlayTheme>();
    if (theme) {
        auto padding = theme->GetMenuPadding();
        if (textDirection_ == TextDirection::RTL) {
            offset += Offset(NormalizeToPx(padding.Left()), NormalizeToPx(padding.Top()));
        } else {
            offset += Offset(-NormalizeToPx(padding.Right()), NormalizeToPx(padding.Top()));
        }
    }
    return offset;
}

bool RosenRenderTextOverlay::CheckNeedPaintMore()
{
    auto overlay = overlayComponent_.Upgrade();
    if (overlay && !overlay->HasMoreButton()) {
        return false;
    }

    if (isDragging_) {
        return false;
    }

    if (hasPoped_ || GetChildren().empty()) {
        RestoreMoreButtonStyle();
        return false;
    }

    if (HasToolBarOnly()) {
        RestoreMoreButtonStyle();
    }

    return true;
}

void RosenRenderTextOverlay::PaintMore(RenderContext& context)
{
    if (!CheckNeedPaintMore()) {
        return;
    }

    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    auto pipelineContext = context_.Upgrade();
    double dipScale = 1.0;
    if (pipelineContext) {
        dipScale = pipelineContext->GetDipScale();
    }
    InitMoreButtonPaintColor();
    Offset moreOffset = CalculateMoreButtonCenter();
    Offset center = (DOT1_POSITION + DOT4_POSITION) * dipScale * HALF;
#ifndef USE_ROSEN_DRAWING
    canvas->save();
    canvas->translate(moreOffset.GetX() + center.GetX(), moreOffset.GetY() + center.GetY());
    if (textDirection_ == TextDirection::RTL) {
        // mirror effect
        canvas->scale(-1.0, 1.0);
    }
    canvas->rotate(rotateDegree_);
    canvas->translate(-center.GetX(), -center.GetY());

    Offset topLeft = (DOT1_POSITION + dot1StartOffset_) * dipScale;
    Offset topRight = (DOT2_POSITION + dot2StartOffset_) * dipScale;
    Offset bottomLeft = (DOT3_POSITION + dot3StartOffset_) * dipScale;
    Offset bottomRight = (DOT4_POSITION + dot4StartOffset_) * dipScale;

    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    skPaint.setColor(arrowColor_.GetValue());
    skPaint.setStyle(SkPaint::Style::kStroke_Style);
    skPaint.setStrokeWidth(NormalizeToPx(strokeWidth_));
    skPaint.setStrokeCap(SkPaint::Cap::kRound_Cap);

    SkPath path;
    path.moveTo(SkDoubleToScalar(topLeft.GetX()), SkDoubleToScalar(topLeft.GetY()));
    path.lineTo(SkDoubleToScalar(topLeft.GetX()), SkDoubleToScalar(topLeft.GetY()));
    path.moveTo(SkDoubleToScalar(topRight.GetX()), SkDoubleToScalar(topRight.GetY()));
    path.rLineTo(SkDoubleToScalar(dot2Offset_.GetX() * dipScale), SkDoubleToScalar(0.0));
    path.moveTo(SkDoubleToScalar(bottomLeft.GetX()), SkDoubleToScalar(bottomLeft.GetY()));
    path.rLineTo(SkDoubleToScalar(0.0), SkDoubleToScalar(dot3Offset_.GetY() * dipScale));
    path.moveTo(SkDoubleToScalar(bottomRight.GetX()), SkDoubleToScalar(bottomRight.GetY()));
    path.rLineTo(SkDoubleToScalar(dot4Offset_.GetX() * dipScale), SkDoubleToScalar(dot4Offset_.GetY() * dipScale));
    canvas->drawPath(path, skPaint);

    PaintClipLine(canvas, skPaint);
    canvas->restore();
#else
    canvas->Save();
    canvas->Translate(moreOffset.GetX() + center.GetX(), moreOffset.GetY() + center.GetY());
    if (textDirection_ == TextDirection::RTL) {
        // mirror effect
        canvas->Scale(-1.0, 1.0);
    }
    canvas->Rotate(rotateDegree_);
    canvas->Translate(-center.GetX(), -center.GetY());

    Offset topLeft = (DOT1_POSITION + dot1StartOffset_) * dipScale;
    Offset topRight = (DOT2_POSITION + dot2StartOffset_) * dipScale;
    Offset bottomLeft = (DOT3_POSITION + dot3StartOffset_) * dipScale;
    Offset bottomRight = (DOT4_POSITION + dot4StartOffset_) * dipScale;

    RSPen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(arrowColor_.GetValue());
    pen.SetWidth(NormalizeToPx(strokeWidth_));
    pen.SetCapStyle(RSPen::CapStyle::ROUND_CAP);

    RSRecordingPath path;
    path.MoveTo(static_cast<float>(topLeft.GetX()), static_cast<float>(topLeft.GetY()));
    path.LineTo(static_cast<float>(topLeft.GetX()), static_cast<float>(topLeft.GetY()));
    path.MoveTo(static_cast<float>(topRight.GetX()), static_cast<float>(topRight.GetY()));
    // Drawing is not supported
    path.MoveTo(static_cast<float>(bottomLeft.GetX()), static_cast<float>(bottomLeft.GetY()));
    // Drawing is not supported
    path.MoveTo(static_cast<float>(bottomRight.GetX()), static_cast<float>(bottomRight.GetY()));
    // Drawing is not supported
    canvas->AttachPen(pen);
    canvas->DrawPath(path);
    canvas->DetachPen();

    PaintClipLine(canvas, pen);
    canvas->Restore();
#endif
}

void RosenRenderTextOverlay::AdjustForAnimation()
{
    ResetRenderChild();
    InitRenderChild(AceType::Claim(this));
    if (!renderBox_ || !renderClip_) {
        return;
    }
    double moreButtonSize = NormalizeToPx(MORE_BUTTON_SIZE);

    // Adjust box and menu for painting before animation.
    if (!isAnimationStarted_ && GetChildren().front()) {
        if (hasMenu_) {
            if (renderMenu_) {
                renderMenu_->SetIsWattingForAnimationStart(true);
            }
        } else {
            renderBox_->SetPaintSize(Size(moreButtonSize, moreButtonSize));
            renderBox_->SetPosition(Offset(GetChildren().front()->GetLayoutSize().Width() - moreButtonSize, 0.0));
        }
    } else {
        if (renderMenu_) {
            renderMenu_->SetIsWattingForAnimationStart(false);
        }
    }

    // Adjust box for painting after animation.
    if (isAnimationStopped_ && hasMenu_ && GetChildren().front()) {
        renderBox_->SetPaintSize(Size(moreButtonSize, moreButtonSize));
        renderBox_->SetPosition(Offset(GetChildren().front()->GetLayoutSize().Width() - moreButtonSize, 0.0));
    }
}

bool RosenRenderTextOverlay::NeedAdjustEndHandle()
{
    auto textField = weakTextField_.Upgrade();
    auto rosenTextField = AceType::DynamicCast<RosenRenderTextField>(textField);
    if (!rosenTextField) {
        LOGE("RosenTextField is nullptr");
        return false;
    }
    return rosenTextField->GetEditingValue().selection.GetEnd() <= startIndex_;
}

bool RosenRenderTextOverlay::NeedAdjustStartHandle()
{
    auto textField = weakTextField_.Upgrade();
    auto rosenTextField = AceType::DynamicCast<RosenRenderTextField>(textField);
    if (!rosenTextField) {
        LOGE("RosenTextField is nullptr");
        return false;
    }
    return rosenTextField->GetEditingValue().selection.GetEnd() > endIndex_;
}

} // namespace OHOS::Ace
