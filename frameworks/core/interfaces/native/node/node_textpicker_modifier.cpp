/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "core/interfaces/native/node/node_textpicker_modifier.h"

#include "bridge/common/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/text_style.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components_ng/pattern/tabs/tabs_model.h"
#include "core/components_ng/pattern/text_picker/textpicker_model_ng.h"
#include "core/pipeline/base/element_register.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t SIZE_OF_THREE = 3;
constexpr int32_t POS_0 = 0;
constexpr int32_t POS_1 = 1;
constexpr int32_t POS_2 = 2;
const char DEFAULT_DELIMITER = '|';
const int32_t ERROR_INT_CODE = -1;
std::string g_strValue;

void SetTextPickerBackgroundColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    TextPickerModelNG::SetBackgroundColor(frameNode, Color(color));
}

void ResetTextPickerBackgroundColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<DialogTheme>();
    CHECK_NULL_VOID(theme);
    TextPickerModelNG::SetBackgroundColor(frameNode, theme->GetBackgroundColor());
}

void SetTextPickerCanLoop(ArkUINodeHandle node, int canLoop)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    TextPickerModelNG::SetCanLoop(frameNode, canLoop);
}

void ResetTextPickerCanLoop(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    TextPickerModelNG::SetCanLoop(frameNode, true);
}

ArkUI_Int32 GetTextPickerSelectedIndex(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_INT_CODE);
    return TextPickerModelNG::getTextPickerSelectedIndex(frameNode);
}

void SetTextPickerSelectedIndex(ArkUINodeHandle node, ArkUI_Uint32* values, ArkUI_Int32 size)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    if (TextPickerModelNG::IsSingle(frameNode)) {
        NodeModifier::SetSelectedIndexSingle(frameNode, values, size);
    } else {
        NodeModifier::SetSelectedIndexMulti(frameNode, values, size);
    }
}

void ResetTextPickerSelectedIndex(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    if (TextPickerModelNG::IsSingle(frameNode)) {
        NodeModifier::SetSelectedIndexSingle(frameNode, 0, 0);
    } else {
        NodeModifier::SetSelectedIndexMulti(frameNode, 0, 0);
    }
}

void SetTextPickerTextStyle(ArkUINodeHandle node, ArkUI_Uint32 color, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    NodeModifier::GetPickerTextStyle(color, fontInfo, styleVal, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetNormalTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void ResetTextPickerTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetNormalTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void SetTextPickerSelectedTextStyle(
    ArkUINodeHandle node, ArkUI_Uint32 color, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    NodeModifier::GetPickerTextStyle(color, fontInfo, styleVal, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetSelectedTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void ResetTextPickerSelectedTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetSelectedTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void SetTextPickerDisappearTextStyle(
    ArkUINodeHandle node, ArkUI_Uint32 color, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    NodeModifier::GetPickerTextStyle(color, fontInfo, styleVal, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetDisappearTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void ResetTextPickerDisappearTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PickerTextStyle pickerTextStyle;
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    TextPickerModelNG::SetDisappearTextStyle(frameNode, pickerTheme, pickerTextStyle);
}

void SetTextPickerDefaultPickerItemHeight(ArkUINodeHandle node, float dVal, ArkUI_Int32 dUnit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    TextPickerModelNG::SetDefaultPickerItemHeight(frameNode, Dimension(dVal, static_cast<DimensionUnit>(dUnit)));
}

void ResetTextPickerDefaultPickerItemHeight(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto height = Dimension(0.0);
    TextPickerModelNG::SetDefaultPickerItemHeight(frameNode, height);
}

ArkUI_CharPtr GetTextPickerSelectedTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, "");
    PickerTextStyle pickerTextStyle = TextPickerModelNG::getSelectedTextStyle(frameNode);
    std::vector<std::string> fontFamilies = pickerTextStyle.fontFamily.value_or(std::vector<std::string>());
    std::string families;
    //set index start
    int index = 0;
    for (auto& family : fontFamilies) {
        families += family;
        if (index != fontFamilies.size() - 1) {
            families += ",";
        }
        index++;
    }
    g_strValue = pickerTextStyle.textColor->ColorToString() + ",";
    g_strValue = g_strValue + pickerTextStyle.fontSize->ToString() + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontWeight.value_or(FontWeight::W100))) + ",";
    g_strValue = g_strValue + families + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontStyle.value_or(OHOS::Ace::FontStyle::NORMAL)));
    return g_strValue.c_str();
}

ArkUI_CharPtr GetTextPickerTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, "");
    PickerTextStyle pickerTextStyle = TextPickerModelNG::getNormalTextStyle(frameNode);
    std::vector<std::string> fontFamilies = pickerTextStyle.fontFamily.value_or(std::vector<std::string>());
    std::string families;
    //set index start
    int index = 0;
    for (auto& family : fontFamilies) {
        families += family;
        if (index != fontFamilies.size() - 1) {
            families += ",";
        }
        index++;
    }
    g_strValue = pickerTextStyle.textColor->ColorToString() + ",";
    g_strValue = g_strValue + pickerTextStyle.fontSize->ToString() + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontWeight.value_or(FontWeight::W100))) + ",";
    g_strValue = g_strValue + families + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontStyle.value_or(OHOS::Ace::FontStyle::NORMAL)));
    return g_strValue.c_str();
}

ArkUI_CharPtr GetTextPickerDisappearTextStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, "");
    PickerTextStyle pickerTextStyle = TextPickerModelNG::getDisappearTextStyle(frameNode);
    std::vector<std::string> fontFamilies = pickerTextStyle.fontFamily.value_or(std::vector<std::string>());
    std::string families;
    //set index start
    int index = 0;
    for (auto& family : fontFamilies) {
        families += family;
        if (index != fontFamilies.size() - 1) {
            families += ",";
        }
        index++;
    }
    g_strValue = pickerTextStyle.textColor->ColorToString() + ",";
    g_strValue = g_strValue + pickerTextStyle.fontSize->ToString() + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontWeight.value_or(FontWeight::W100))) + ",";
    g_strValue = g_strValue + families + ",";
    g_strValue =
        g_strValue + std::to_string(static_cast<int>(pickerTextStyle.fontStyle.value_or(OHOS::Ace::FontStyle::NORMAL)));
    return g_strValue.c_str();
}

} // namespace

namespace NodeModifier {
const ArkUITextPickerModifier* GetTextPickerModifier()
{
    static const ArkUITextPickerModifier modifier = { SetTextPickerBackgroundColor, SetTextPickerCanLoop,
        GetTextPickerSelectedIndex, SetTextPickerSelectedIndex, GetTextPickerTextStyle, SetTextPickerTextStyle,
        GetTextPickerSelectedTextStyle, SetTextPickerSelectedTextStyle, GetTextPickerDisappearTextStyle,
        SetTextPickerDisappearTextStyle, SetTextPickerDefaultPickerItemHeight, ResetTextPickerCanLoop,
        ResetTextPickerSelectedIndex, ResetTextPickerTextStyle, ResetTextPickerSelectedTextStyle,
        ResetTextPickerDisappearTextStyle, ResetTextPickerDefaultPickerItemHeight, ResetTextPickerBackgroundColor };

    return &modifier;
}

void GetPickerTextStyle(uint32_t color, ArkUI_CharPtr fontInfo, int32_t styleVal, NG::PickerTextStyle& textStyle)
{
    textStyle.textColor = Color(color);

    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, DEFAULT_DELIMITER, res);

    if (res.size() != SIZE_OF_THREE) {
        return;
    }

    if (res[POS_0] != "-1") {
        textStyle.fontSize = StringUtils::StringToCalcDimension(res[POS_0], false, DimensionUnit::FP);
    } else {
        textStyle.fontSize = Dimension(-1);
    }

    if (res[POS_1] != "-1") {
        textStyle.fontWeight = StringUtils::StringToFontWeight(res[POS_1], FontWeight::NORMAL);
    }

    if (res[POS_2] != "-1") {
        textStyle.fontFamily = Framework::ConvertStrToFontFamilies(res[POS_2]);
    }
    textStyle.fontStyle = static_cast<Ace::FontStyle>(styleVal);
}

void SetSelectedIndexSingle(FrameNode* frameNode, uint32_t* selectedValues, const int32_t size)
{
    std::vector<NG::RangeContent> rangeResult;
    TextPickerModelNG::GetSingleRange(frameNode, rangeResult);
    if (selectedValues[0] >= rangeResult.size()) {
        TextPickerModelNG::SetSelected(frameNode, 0);
    } else {
        TextPickerModelNG::SetSelected(frameNode, selectedValues[0]);
    }
}

void SetSelectedIndexMultiInternal(FrameNode* frameNode, uint32_t count,
    std::vector<NG::TextCascadePickerOptions>& options, std::vector<uint32_t>& selectedValues)
{
    if (!TextPickerModelNG::IsCascade(frameNode)) {
        NodeModifier::SetSelectedInternal(count, options, selectedValues);
    } else {
        TextPickerModelNG::SetHasSelectAttr(frameNode, true);
        NodeModifier::ProcessCascadeSelected(options, 0, selectedValues);
        uint32_t maxCount = TextPickerModelNG::GetMaxCount(frameNode);
        if (selectedValues.size() < maxCount) {
            auto differ = maxCount - selectedValues.size();
            for (uint32_t i = 0; i < differ; i++) {
                selectedValues.emplace_back(0);
            }
        }
    }
}

void SetSelectedIndexSingleInternal(const std::vector<NG::TextCascadePickerOptions>& options, uint32_t count,
    uint32_t& selectedValue, std::vector<uint32_t>& selectedValues)
{
    if (options.size() > 0) {
        if (selectedValue >= options[0].rangeResult.size()) {
            selectedValue = 0;
        }
        selectedValues.emplace_back(selectedValue);
        for (uint32_t i = 1; i < count; i++) {
            selectedValues.emplace_back(0);
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            selectedValues.emplace_back(0);
        }
    }
}

void SetSelectedInternal(
    uint32_t count, std::vector<NG::TextCascadePickerOptions>& options, std::vector<uint32_t>& selectedValues)
{
    for (uint32_t i = 0; i < count; i++) {
        if (i > selectedValues.size() - 1) {
            selectedValues.emplace_back(0);
        } else {
            if (selectedValues[i] >= options[i].rangeResult.size()) {
                selectedValues[i] = 0;
            }
        }
    }
}

void SetSelectedIndexMulti(FrameNode* frameNode, uint32_t* inputs, const int32_t size)
{
    std::vector<NG::TextCascadePickerOptions> options;
    TextPickerModelNG::GetMultiOptions(frameNode, options);

    auto count = TextPickerModelNG::IsCascade(frameNode) ? TextPickerModelNG::GetMaxCount(frameNode) : options.size();
    std::vector<uint32_t> selectedValues;

    if (size >= 0) {
        selectedValues.assign(inputs, inputs + size);
        SetSelectedIndexMultiInternal(frameNode, count, options, selectedValues);
    } else {
        TextPickerModelNG::SetHasSelectAttr(frameNode, true);
        SetSelectedIndexSingleInternal(options, count, inputs[0], selectedValues);
    }
    TextPickerModelNG::SetSelecteds(frameNode, selectedValues);
}

void ProcessCascadeSelected(
    const std::vector<NG::TextCascadePickerOptions>& options, uint32_t index, std::vector<uint32_t>& selectedValues)
{
    std::vector<std::string> rangeResultValue;
    for (size_t i = 0; i < options.size(); i++) {
        rangeResultValue.emplace_back(options[i].rangeResult[0]);
    }

    if (index > selectedValues.size() - 1) {
        selectedValues.emplace_back(0);
    }
    if (selectedValues[index] >= rangeResultValue.size()) {
        selectedValues[index] = 0;
    }
    if (selectedValues[index] <= options.size() - 1 && options[selectedValues[index]].children.size() > 0) {
        ProcessCascadeSelected(options[selectedValues[index]].children, index + 1, selectedValues);
    }
}
} // namespace NodeModifier
} // namespace OHOS::Ace::NG
