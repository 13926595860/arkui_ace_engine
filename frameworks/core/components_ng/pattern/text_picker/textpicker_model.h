/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_PICKER_TEXT_PICKER_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_PICKER_TEXT_PICKER_MODEL_H

#include <functional>
#include "base/geometry/dimension.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"

namespace OHOS::Ace {
using TextChangeEvent = std::function<void(const std::string&, double)>;
using TextCascadeChangeEvent = std::function<void(const std::vector<std::string>&, std::vector<double>)>;
class TextPickerModel {
public:
    static TextPickerModel* GetInstance();
    virtual ~TextPickerModel() = default;

    virtual void Create(RefPtr<PickerTheme> pickerTheme, uint32_t columnKind) = 0;
    virtual void SetSelected(uint32_t value) = 0;
    virtual void SetRange(const std::vector<NG::RangeContent>& value) = 0;
    virtual void SetValue(const std::string& value) = 0;
    virtual void SetOnChange(TextChangeEvent&& onChange) = 0;
    virtual void SetDefaultPickerItemHeight(const Dimension& value) = 0;
    virtual void SetDefaultAttributes(const RefPtr<PickerTheme>& pickerTheme) = 0;
    virtual void SetDisappearTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) = 0;
    virtual void SetNormalTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) = 0;
    virtual void SetSelectedTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) = 0;
    virtual void MultiInit(const RefPtr<PickerTheme> pickerTheme) = 0;
    virtual void SetColumns(const std::vector<NG::TextCascadePickerOptions>& options) = 0;
    virtual void SetIsCascade(bool isCascade) = 0;
    virtual void SetOnCascadeChange(TextCascadeChangeEvent&& onChange) = 0;
    virtual void SetValues(const std::vector<std::string>& values) = 0;
    virtual void SetSelecteds(const std::vector<uint32_t>& values) = 0;
    virtual bool IsSingle() = 0;
    virtual bool GetSingleRange(std::vector<NG::RangeContent>& rangeValue) = 0;
    virtual bool IsCascade() = 0;
    virtual uint32_t GetMaxCount() = 0;
    virtual void SetMaxCount(uint32_t maxCount) = 0;
    virtual bool GetMultiOptions(std::vector<NG::TextCascadePickerOptions>& options) = 0;
private:
    static std::unique_ptr<TextPickerModel> textPickerInstance_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_PICKER_TEXT_PICKER_MODEL_H
