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

#include "core/components_ng/pattern/text/text_paint_method.h"

namespace OHOS::Ace::NG {
TextPaintMethod::TextPaintMethod(const WeakPtr<Pattern>& pattern, RefPtr<Paragraph> paragraph, float baselineOffset,
    RefPtr<TextContentModifier> textContentModifier, RefPtr<TextOverlayModifier> textOverlayModifier)
    : pattern_(pattern), paragraph_(std::move(paragraph)), baselineOffset_(baselineOffset),
      textContentModifier_(textContentModifier), textOverlayModifier_(textOverlayModifier)
{
    (void)baselineOffset_;
}

RefPtr<Modifier> TextPaintMethod::GetContentModifier(PaintWrapper* paintWrapper)
{
    return textContentModifier_;
}

void TextPaintMethod::UpdateContentModifier(PaintWrapper* paintWrapper) {}

RefPtr<Modifier> TextPaintMethod::GetOverlayModifier(PaintWrapper* paintWrapper)
{
    return textOverlayModifier_;
}

void TextPaintMethod::UpdateOverlayModifier(PaintWrapper* paintWrapper) {}
} // namespace OHOS::Ace::NG