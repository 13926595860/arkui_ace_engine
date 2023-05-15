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

#include "core/components_ng/pattern/text/text_layout_property.h"

namespace OHOS::Ace::NG {
namespace {
static const std::array<std::string, 6> TEXT_BASE_LINE_TO_STRING = {
    "textBaseline.ALPHABETIC",
    "textBaseline.IDEOGRAPHIC",
    "textBaseline.TOP",
    "textBaseline.BOTTOM",
    "textBaseline.MIDDLE",
    "textBaseline.HANGING",
};

inline std::unique_ptr<JsonValue> CovertShadowToJson(const Shadow& shadow)
{
    auto jsonShadow = JsonUtil::Create(true);
    jsonShadow->Put("radius", std::to_string(shadow.GetBlurRadius()).c_str());
    jsonShadow->Put("color", shadow.GetColor().ColorToString().c_str());
    jsonShadow->Put("offsetX", std::to_string(shadow.GetOffset().GetX()).c_str());
    jsonShadow->Put("offsetY", std::to_string(shadow.GetOffset().GetY()).c_str());
    jsonShadow->Put("type", std::to_string(static_cast<int32_t>(shadow.GetShadowType())).c_str());
    return jsonShadow;
}
} // namespace

std::string TextLayoutProperty::GetCopyOptionString() const
{
    std::string copyOptionString = "CopyOptions.None";
    switch (GetCopyOptionValue(CopyOptions::None)) {
        case CopyOptions::InApp:
            copyOptionString = "CopyOptions.InApp";
            break;
        case CopyOptions::Local:
            copyOptionString = "CopyOptions.Local";
            break;
        case CopyOptions::Distributed:
            copyOptionString = "CopyOptions.Distributed";
            break;
        case CopyOptions::None:
        default:
            break;
    }
    return copyOptionString;
}

void TextLayoutProperty::ToJsonValue(std::unique_ptr<JsonValue>& json) const
{
    LayoutProperty::ToJsonValue(json);
    json->Put("content", GetContent().value_or("").c_str());
    json->Put("fontSize", GetFontSize().value_or(16.0_fp).ToString().c_str());
    json->Put("fontColor",
        GetForegroundColor().value_or(GetTextColor().value_or(Color::BLACK)).ColorToString().c_str());
    json->Put("fontStyle", GetItalicFontStyle().value_or(Ace::FontStyle::NORMAL) == Ace::FontStyle::NORMAL
                               ? "FontStyle.Normal"
                               : "FontStyle.Italic");
    json->Put("fontWeight", V2::ConvertWrapFontWeightToStirng(GetFontWeight().value_or(FontWeight::NORMAL)).c_str());
    std::vector<std::string> fontFamilyVector =
        GetFontFamily().value_or<std::vector<std::string>>({ "HarmonyOS Sans" });
    if (fontFamilyVector.empty()) {
        fontFamilyVector = std::vector<std::string>({ "HarmonyOS Sans" });
    }
    std::string fontFamily = fontFamilyVector.at(0);
    for (uint32_t i = 1; i < fontFamilyVector.size(); ++i) {
        fontFamily += ',' + fontFamilyVector.at(i);
    }
    json->Put("fontFamily", fontFamily.c_str());

    auto jsonDecoration = JsonUtil::Create(true);
    std::string type = V2::ConvertWrapTextDecorationToStirng(GetTextDecoration().value_or(TextDecoration::NONE));
    jsonDecoration->Put("type", type.c_str());
    jsonDecoration->Put("color", GetTextDecorationColor().value_or(Color::BLACK).ColorToString().c_str());
    json->Put("decoration", jsonDecoration->ToString().c_str());

    json->Put("textCase", V2::ConvertWrapTextCaseToStirng(GetTextCase().value_or(TextCase::NORMAL)).c_str());
    json->Put("minFontSize", GetAdaptMinFontSize().value_or(Dimension()).ToString().c_str());
    json->Put("maxFontSize", GetAdaptMaxFontSize().value_or(Dimension()).ToString().c_str());
    json->Put("letterSpacing", GetLetterSpacing().value_or(Dimension()).ToString().c_str());
    json->Put("lineHeight", GetLineHeight().value_or(0.0_vp).ToString().c_str());
    json->Put("textBaseline",
        TEXT_BASE_LINE_TO_STRING.at(static_cast<int32_t>(GetTextBaseline().value_or(TextBaseline::ALPHABETIC)))
            .c_str());
    json->Put(
        "baselineOffset", std::to_string(static_cast<int32_t>(GetBaselineOffset().value_or(0.0_vp).Value())).c_str());
    json->Put("textAlign", V2::ConvertWrapTextAlignToString(GetTextAlign().value_or(TextAlign::START)).c_str());
    json->Put(
        "textOverflow", V2::ConvertWrapTextOverflowToString(GetTextOverflow().value_or(TextOverflow::CLIP)).c_str());
    json->Put("maxLines", std::to_string(GetMaxLines().value_or(UINT32_MAX)).c_str());
    auto jsonShadow = CovertShadowToJson(GetTextShadow().value_or(Shadow()));
    json->Put("textShadow", jsonShadow);
    json->Put("heightAdaptivePolicy", V2::ConvertWrapTextHeightAdaptivePolicyToString(
        GetHeightAdaptivePolicy().value_or(TextHeightAdaptivePolicy::MAX_LINES_FIRST)).c_str());
    json->Put("copyOption", GetCopyOptionString().c_str());
}

} // namespace OHOS::Ace::NG