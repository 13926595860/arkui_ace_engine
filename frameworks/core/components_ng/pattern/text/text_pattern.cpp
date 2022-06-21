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

#include "core/components_ng/pattern/text/text_pattern.h"

#include "base/utils/utils.h"
#include "core/components_ng/pattern/text/text_layout_algorithm.h"
#include "core/components_ng/render/canvas.h"
#include "core/pipeline/base/render_context.h"

namespace OHOS::Ace::NG {
void TextPattern::PaintContent(RenderContext* renderContext, const OffsetF& offset)
{
    CHECK_NULL_VOID(textLayoutAlgorithm_);
    const auto& paragraph = textLayoutAlgorithm_->GetTxtParagraph();
    CHECK_NULL_VOID(paragraph);

    const auto& canvas = renderContext->GetCanvas();
    renderContext->SetDrawContentAtLast(true);
    paragraph->Paint(canvas, offset.GetX(), offset.GetY());
}

bool TextPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout)
{
    if (skipMeasure) {
        return false;
    }
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);
    auto textLayoutAlgorithm = DynamicCast<TextLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(textLayoutAlgorithm, false);
    textLayoutAlgorithm_ = textLayoutAlgorithm;
    return true;
}
} // namespace OHOS::Ace::NG
