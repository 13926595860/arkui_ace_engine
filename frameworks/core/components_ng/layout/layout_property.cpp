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

#include "core/components_ng/layout/layout_property.h"

#include "core/components_ng/property/layout_constraint.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {
void LayoutProperty::Reset()
{
    layoutConstraint_.reset();
    calcLayoutConstraint_.reset();
    padding_.reset();
    magicItemProperty_.reset();
    positionProperty_.reset();
    measureType_.reset();
    CleanDirty();
}

RefPtr<LayoutProperty> LayoutProperty::Clone() const
{
    auto layoutProperty = MakeRefPtr<LayoutProperty>();
    layoutProperty->UpdateLayoutProperty(this);
    return layoutProperty;
}

void LayoutProperty::UpdateLayoutProperty(const LayoutProperty* layoutProperty)
{
    layoutConstraint_ = layoutProperty->layoutConstraint_;
    if (layoutProperty->calcLayoutConstraint_) {
        calcLayoutConstraint_ = std::make_unique<MeasureProperty>(*layoutProperty->calcLayoutConstraint_);
    }
    if (layoutProperty->padding_) {
        padding_ = std::make_unique<PaddingProperty>(*layoutProperty->padding_);
    }
    if (layoutProperty->magicItemProperty_) {
        magicItemProperty_ = std::make_unique<MagicItemProperty>(*layoutProperty->magicItemProperty_);
    }
    if (layoutProperty->positionProperty_) {
        positionProperty_ = std::make_unique<PositionProperty>(*layoutProperty->positionProperty_);
    }
    measureType_ = layoutProperty->measureType_;
    propertyChangeFlag_ = layoutProperty->propertyChangeFlag_;
}

void LayoutProperty::UpdateCalcLayoutProperty(const MeasureProperty& constraint)
{
    if (!calcLayoutConstraint_) {
        calcLayoutConstraint_ = std::make_unique<MeasureProperty>(constraint);
        propertyChangeFlag_ = propertyChangeFlag_ | PROPERTY_UPDATE_MEASURE;
        return;
    }
    if (*calcLayoutConstraint_ == constraint) {
        return;
    }
    calcLayoutConstraint_->selfIdealSize = constraint.selfIdealSize;
    calcLayoutConstraint_->maxSize = constraint.maxSize;
    calcLayoutConstraint_->minSize = constraint.minSize;
    propertyChangeFlag_ = propertyChangeFlag_ | PROPERTY_UPDATE_MEASURE;
}

void LayoutProperty::UpdateLayoutConstraint(const LayoutConstraintF& parentConstraint, bool updateFlag)
{
    layoutConstraint_ = parentConstraint;
    if (calcLayoutConstraint_) {
        auto parentSize = parentConstraint.parentIdealSize.value_or(SizeF(-1, -1));
        if (calcLayoutConstraint_->maxSize.has_value()) {
            layoutConstraint_->UpdateMaxSizeWithCheck(
                ConvertToSize(calcLayoutConstraint_->maxSize.value(), parentConstraint.scaleProperty, parentSize));
        }
        if (calcLayoutConstraint_->minSize.has_value()) {
            layoutConstraint_->UpdateMinSizeWithCheck(
                ConvertToSize(calcLayoutConstraint_->minSize.value(), parentConstraint.scaleProperty, parentSize));
        }
        if (calcLayoutConstraint_->selfIdealSize.has_value()) {
            layoutConstraint_->UpdateSelfIdealSizeWithCheck(ConvertToSize(
                calcLayoutConstraint_->selfIdealSize.value(), parentConstraint.scaleProperty, parentSize));
        }
    }
    layoutConstraint_->scaleProperty = parentConstraint.scaleProperty;
    CheckSelfIdealSize();
}

void LayoutProperty::CheckSelfIdealSize()
{
    if (measureType_ == MeasureType::MATCH_PARENT && layoutConstraint_->parentIdealSize.has_value()) {
        layoutConstraint_->UpdateSelfIdealSizeWithCheck(layoutConstraint_->parentIdealSize.value());
    }
}

void LayoutProperty::UpdateContentConstraint(const std::optional<LayoutConstraintF>& replace, bool updateFlag)
{
    if (replace) {
        if (replace == contentConstraint_) {
            return;
        }
        contentConstraint_ = replace;
        if (updateFlag) {
            propertyChangeFlag_ = propertyChangeFlag_ | PROPERTY_UPDATE_MEASURE;
        }
        return;
    }
    auto contentConstraint = layoutConstraint_.value_or(LayoutConstraintF());
    if (padding_) {
        auto paddingF = ConvertToPaddingPropertyF(
            *padding_, contentConstraint.scaleProperty, contentConstraint.selfIdealSize.value_or(SizeF(0, 0)));
        contentConstraint.MinusPadding(paddingF.left, paddingF.right, paddingF.top, paddingF.bottom);
    }
    if (contentConstraint_ == contentConstraint) {
        return;
    }
    contentConstraint_ = contentConstraint;
    if (updateFlag) {
        propertyChangeFlag_ = propertyChangeFlag_ | PROPERTY_UPDATE_MEASURE;
    }
}

PaddingPropertyF LayoutProperty::CreatePaddingPropertyF()
{
    auto layoutConstraint = layoutConstraint_.value_or(LayoutConstraintF());
    auto sizeF = SizeF(0, 0);
    if (layoutConstraint_.has_value()) {
        sizeF = layoutConstraint_->selfIdealSize.value_or(SizeF(0, 0));
    }
    return ConvertToPaddingPropertyF(padding_, layoutConstraint.scaleProperty, sizeF);
}
} // namespace OHOS::Ace::NG
