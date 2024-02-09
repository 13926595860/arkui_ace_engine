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
#include "core/interfaces/native/node/node_api.h"

#include "core/interfaces/native/node/node_common_modifier.h"
#include "core/interfaces/native/node/node_date_picker_modifier.h"
#include "core/interfaces/native/node/node_image_span_modifier.h"
#include "core/interfaces/native/node/node_image_modifier.h"
#include "core/interfaces/native/node/node_list_modifier.h"
#include "core/interfaces/native/node/node_list_item_group_modifier.h"
#include "core/interfaces/native/node/node_loading_progress_modifier.h"
#include "core/interfaces/native/node/node_scroll_modifier.h"
#include "core/interfaces/native/node/node_span_modifier.h"
#include "core/interfaces/native/node/node_stack_modifier.h"
#include "core/interfaces/native/node/node_text_input_modifier.h"
#include "core/interfaces/native/node/node_text_area_modifier.h"
#include "core/interfaces/native/node/node_textpicker_modifier.h"
#include "core/interfaces/native/node/node_xcomponent_modifier.h"
#include "core/interfaces/native/node/node_text_modifier.h"
#include "core/interfaces/native/node/node_timepicker_modifier.h"
#include "core/interfaces/native/node/node_toggle_modifier.h"
#include "core/interfaces/native/node/node_swiper_modifier.h"

#include "core/interfaces/native/node/alphabet_indexer_modifier.h"
#include "core/interfaces/native/node/checkboxgroup_modifier.h"
#include "core/interfaces/native/node/calendar_picker_modifier.h"
#include "core/interfaces/native/node/checkbox_modifier.h"
#include "core/interfaces/native/node/column_modifier.h"
#include "core/interfaces/native/node/blank_modifier.h"
#include "core/interfaces/native/node/column_split_modifier.h"
#include "core/interfaces/native/node/grid_col_modifier.h"
#include "core/interfaces/native/node/grid_item_modifier.h"
#include "core/interfaces/native/node/button_modifier.h"

namespace OHOS::Ace::NG {
namespace {
const ArkUINodeModifiers impl = {
    ARKUI_NODE_MODIFIERS_API_VERSION,
    NodeModifier::GetCommonModifier,
    NodeModifier::GetCheckboxGroupModifier,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetTextModifier,
    NodeModifier::GetButtonModifier,
    NodeModifier::GetToggleModifier,
    NodeModifier::GetImageSpanModifier,
    NodeModifier::GetBlankModifier,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetCheckboxModifier,
    NodeModifier::GetTimepickerModifier,
    NodeModifier::GetTextPickerModifier,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetStackModifier,
    nullptr,
    nullptr,
    NodeModifier::GetGridColModifier,
    nullptr,
    nullptr,
    NodeModifier::GetTextAreaModifier,
    nullptr,
    NodeModifier::GetColumnModifier,
    nullptr,
    NodeModifier::GetImageModifier,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetColumnSplitModifier,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetCalendarPickerModifier,
    NodeModifier::GetTextInputModifier,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetDatePickerModifier,
    nullptr,
    NodeModifier::GetAlphabetIndexerModifier,
    nullptr,
    nullptr,
    NodeModifier::GetScrollModifier,
    NodeModifier::GetGridItemModifier,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    NodeModifier::GetSwiperModifier,
    nullptr,
    NodeModifier::GetListModifier,
    NodeModifier::GetListItemGroupModifier,
    nullptr,
    NodeModifier::GetLoadingProgressModifier,
    nullptr,
    nullptr,
    nullptr,
#ifdef PLUGIN_COMPONENT_SUPPORTED
    nullptr,
#else
    nullptr,
#endif
#ifdef XCOMPONENT_SUPPORTED
    NodeModifier::GetXComponentModifier,
#else
    nullptr,
#endif
    NodeModifier::GetUIStateModifier,
#ifdef FORM_SUPPORTED
    nullptr,
#else
    nullptr,
#endif
    nullptr, // FlexModifier
    nullptr, // ScrollBarModifier
    nullptr, // ScrollerModifier
    nullptr, // TabContentModifier
    nullptr, // TabsControllerModifier
    nullptr, // GestureModifier
    nullptr, // BadgeModifier
    nullptr, // WebModifier
    nullptr, // RefreshModifier
    nullptr, // MenuItemGroupModifier
    nullptr, // SearchControllerModifier
    nullptr, // SideBarModifier
    nullptr, // PatternLockControllerModifier
    nullptr, // TextTimerControllerModifier
    nullptr, // TextClockControllerModifier
    nullptr, // RichEditorControllerModifier
    nullptr, // TextAreaControllerModifier
};

} // namespace

} // namespace OHOS::Ace::NG

extern "C" {

const ArkUINodeModifiers* GetArkUINodeModifiers()
{
    return &OHOS::Ace::NG::impl;
}
}
