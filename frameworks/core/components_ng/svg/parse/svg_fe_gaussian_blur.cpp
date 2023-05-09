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

#include "frameworks/core/components_ng/svg/parse/svg_fe_gaussian_blur.h"

#ifndef NEW_SKIA
#include "include/effects/SkBlurImageFilter.h"
#else
#include "include/effects/SkImageFilters.h"
#endif

#include "base/utils/utils.h"
#include "frameworks/core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"

namespace OHOS::Ace::NG {

RefPtr<SvgNode> SvgFeGaussianBlur::Create()
{
    return AceType::MakeRefPtr<SvgFeGaussianBlur>();
}

SvgFeGaussianBlur::SvgFeGaussianBlur() : SvgFe()
{
    declaration_ = AceType::MakeRefPtr<SvgFeGaussianBlurDeclaration>();
    declaration_->Init();
    declaration_->InitializeStyle();
}

void SvgFeGaussianBlur::OnAsImageFilter(sk_sp<SkImageFilter>& imageFilter,
    const ColorInterpolationType& srcColor, ColorInterpolationType& currentColor) const
{
    auto declaration = AceType::DynamicCast<SvgFeGaussianBlurDeclaration>(declaration_);
    CHECK_NULL_VOID_NOLOG(declaration);
    imageFilter = MakeImageFilter(declaration->GetIn(), imageFilter);
#ifndef NEW_SKIA
    imageFilter = SkBlurImageFilter::Make(
        declaration->GetStdDeviation(), declaration->GetStdDeviation(), imageFilter, nullptr);
#else
    imageFilter = SkImageFilters::Blur(
        declaration->GetStdDeviation(), declaration->GetStdDeviation(), imageFilter, nullptr);
#endif
    ConverImageFilterColor(imageFilter, srcColor, currentColor);
}

} // namespace OHOS::Ace::NG
