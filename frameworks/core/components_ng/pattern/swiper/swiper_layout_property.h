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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SWIPER_SWIPER_LAYOUT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SWIPER_SWIPER_LAYOUT_PROPERTY_H

#include "base/geometry/axis.h"
#include "base/geometry/dimension.h"
#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/declaration/swiper/swiper_declaration.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/property/property.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT SwiperLayoutProperty : public LayoutProperty {
    DECLARE_ACE_TYPE(SwiperLayoutProperty, LayoutProperty);

public:
    SwiperLayoutProperty() = default;

    ~SwiperLayoutProperty() override = default;

    RefPtr<LayoutProperty> Clone() const override
    {
        auto value = MakeRefPtr<SwiperLayoutProperty>();
        value->LayoutProperty::UpdateLayoutProperty(DynamicCast<LayoutProperty>(this));
        value->propDirection_ = CloneDirection();
        value->propIndex_ = CloneIndex();
        value->propItemSpace_ = CloneItemSpace();
        value->propCachedCount_ = CloneCachedCount();
        value->propDisplayMode_ = CloneDisplayMode();
        value->propDisplayCount_ = CloneDisplayCount();
        value->propShowIndicator_ = CloneShowIndicator();
        value->propIndicatorType_ = CloneIndicatorType();
        value->propLeft_ = CloneLeft();
        value->propTop_ = CloneTop();
        value->propRight_ = CloneRight();
        value->propBottom_ = CloneBottom();
        value->propPrevMargin_ = ClonePrevMargin();
        value->propNextMargin_ = CloneNextMargin();
        value->propDisplayArrow_ = CloneDisplayArrow();
        value->propHoverShow_ = CloneHoverShow();
        value->propIsShowBoard_ = CloneIsShowBoard();
        value->propIsSiderMiddle_ = CloneIsSiderMiddle();
        value->propBoardSize_ = CloneBoardSize();
        value->propBoardColor_ = CloneBoardColor();
        value->propArrowSize_ = CloneArrowSize();
        value->propArrowColor_ = CloneArrowColor();
        return value;
    }

    void Reset() override
    {
        LayoutProperty::Reset();
        ResetDirection();
        ResetIndex();
        ResetItemSpace();
        ResetCachedCount();
        ResetDisplayMode();
        ResetDisplayCount();
        ResetShowIndicator();
        ResetIndicatorType();
        ResetLeft();
        ResetTop();
        ResetRight();
        ResetBottom();
        ResetPrevMargin();
        ResetNextMargin();
        ResetDisplayArrow();
        ResetHoverShow();
        ResetIsShowBoard();
        ResetIsSiderMiddle();
        ResetBoardSize();
        ResetBoardColor();
        ResetArrowSize();
        ResetArrowColor();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        LayoutProperty::ToJsonValue(json);
        json->Put("index", std::to_string(propIndex_.value_or(0)).c_str());
        json->Put("vertical", propDirection_.value_or(Axis::HORIZONTAL) == Axis::VERTICAL ? "true" : "false");
        json->Put("indicator", propShowIndicator_.value_or(true) ? "true" : "false");
        json->Put("itemSpace", propItemSpace_.value_or(Dimension(0, DimensionUnit::VP)).ToString().c_str());
        json->Put("cachedCount", propCachedCount_.value_or(1));
        json->Put("displayMode", propDisplayMode_.value_or(SwiperDisplayMode::STRETCH) == SwiperDisplayMode::AUTO_LINEAR
                                     ? "SwiperDisplayMode.AutoLinear"
                                     : "SwiperDisplayMode.Stretch");
        json->Put("displayCount", propDisplayCount_.value_or(1));
        json->Put("prevMargin", propPrevMargin_.value_or(Dimension(0, DimensionUnit::VP)).ToString().c_str());
        json->Put("nextMargin", propNextMargin_.value_or(Dimension(0, DimensionUnit::VP)).ToString().c_str());
        json->Put("displayArrow", propDisplayArrow_.value_or(false) ? "true" : "false");
        json->Put("hoverShow", propHoverShow_.value_or(false) ? "true" : "false");
        json->Put("isShowBoard", propIsShowBoard_.value_or(false) ? "true" : "false");
        json->Put("isSiderMiddle", propIsSiderMiddle_.value_or(false) ? "true" : "false");
        json->Put("boardSize", propBoardSize_.value_or(Dimension(0, DimensionUnit::VP)).ToString().c_str());
        json->Put("arrowSize", propArrowSize_.value_or(Dimension(0, DimensionUnit::VP)).ToString().c_str());
        json->Put("boardColor", propBoardColor_.value_or(Color::TRANSPARENT).ColorToString().c_str());
        json->Put("arrowColor", propArrowColor_.value_or(Color::TRANSPARENT).ColorToString().c_str());
    }

    void UpdateIndexWithoutMeasure(int32_t index)
    {
        if (propIndex_ != index) {
            propIndex_ = index;
        }
    }

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Direction, Axis, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Index, int32_t, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ItemSpace, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(CachedCount, int32_t, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(DisplayMode, SwiperDisplayMode, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(DisplayCount, int32_t, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ShowIndicator, bool, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(IndicatorType, SwiperIndicatorType, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Left, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Top, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Right, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Bottom, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(PrevMargin, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(NextMargin, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(DisplayArrow, bool, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(HoverShow, bool, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(IsShowBoard, bool, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(IsSiderMiddle, bool, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(BoardSize, Dimension, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(BoardColor, Color, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ArrowSize, Dimension, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ArrowColor, Color, PROPERTY_UPDATE_NORMAL);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SWIPER_SWIPER_LAYOUT_PROPERTY_H
