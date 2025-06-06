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

#include "adapter/preview/entrance/clipboard/clipboard_impl.h"

#include "adapter/preview/entrance/ace_preview_helper.h"
#include "frameworks/base/utils/utils.h"

namespace OHOS::Ace::Platform {

void ClipboardImpl::AddPixelMapRecord(const RefPtr<PasteDataMix>& pasteData, const RefPtr<PixelMap>& pixmap) {}
void ClipboardImpl::AddImageRecord(const RefPtr<PasteDataMix>& pasteData, const std::string& uri) {}
void ClipboardImpl::AddTextRecord(const RefPtr<PasteDataMix>& pasteData, const std::string& selectedStr) {}
void ClipboardImpl::AddSpanStringRecord(const RefPtr<PasteDataMix>& pasteData, std::vector<uint8_t>& data) {}
void ClipboardImpl::AddMultiTypeRecord(
    const RefPtr<PasteDataMix>& pasteData, const RefPtr<MultiTypeRecordMix>& multiTypeRecord) {};
void ClipboardImpl::SetData(const RefPtr<PasteDataMix>& pasteData, CopyOptions copyOption) {}
void ClipboardImpl::GetData(const std::function<void(const std::string&, bool isLastRecord)>& textCallback,
    const std::function<void(const RefPtr<PixelMap>&, bool isLastRecord)>& pixelMapCallback,
    const std::function<void(const std::string&, bool isLastRecord)>& urlCallback, bool syncMode)
{}
void ClipboardImpl::GetSpanStringData(
    const std::function<void(std::vector<std::vector<uint8_t>>&, const std::string&, bool&)>& callback, bool syncMode)
{}

RefPtr<PasteDataMix> ClipboardImpl::CreatePasteDataMix()
{
    return AceType::MakeRefPtr<PasteDataMix>();
}

void ClipboardImpl::SetData(const std::string& data, CopyOptions copyOption, bool isDragData)
{
    CHECK_NULL_VOID(taskExecutor_);
    taskExecutor_->PostTask(
        [data] {
            auto setClipboardData = AcePreviewHelper::GetInstance()->GetCallbackOfSetClipboardData();
            if (setClipboardData) {
                setClipboardData(data);
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIClipboardSetData");
}

void ClipboardImpl::GetData(const std::function<void(const std::string&)>& callback, bool syncMode)
{
    if (!taskExecutor_ || !callback) {
        return;
    }
    taskExecutor_->PostTask(
        [callback] {
            auto getClipboardData = AcePreviewHelper::GetInstance()->GetCallbackOfGetClipboardData();
            if (callback && getClipboardData) {
                callback(getClipboardData());
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIClipboardGetData");
}

void ClipboardImpl::HasData(const std::function<void(bool hasData)>& callback)
{
    if (!taskExecutor_ || !callback) {
        return;
    }
    taskExecutor_->PostTask(
        [callback] {
            auto getClipboardData = AcePreviewHelper::GetInstance()->GetCallbackOfGetClipboardData();
            if (callback && getClipboardData) {
                callback(!getClipboardData().empty());
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIClipboardHasData");
}

void ClipboardImpl::HasDataType(
    const std::function<void(bool hasData)>& callback, const std::vector<std::string>& mimeTypes)
{
    HasData(callback);
}

void ClipboardImpl::SetPixelMapData(const RefPtr<PixelMap>& pixmap, CopyOptions copyOption)
{
    if (!taskExecutor_ || !callbackSetClipboardPixmapData_) {
        return;
    }
    taskExecutor_->PostTask([callbackSetClipboardPixmapData = callbackSetClipboardPixmapData_,
                                pixmap] { callbackSetClipboardPixmapData(pixmap); },
        TaskExecutor::TaskType::UI, "ArkUIClipboardSetPixelMapData");
}

void ClipboardImpl::GetPixelMapData(const std::function<void(const RefPtr<PixelMap>&)>& callback, bool syncMode)
{
    if (!taskExecutor_ || !callbackGetClipboardPixmapData_ || !callback) {
        return;
    }
    taskExecutor_->PostTask([callbackGetClipboardPixmapData = callbackGetClipboardPixmapData_,
                                callback] { callback(callbackGetClipboardPixmapData()); },
        TaskExecutor::TaskType::UI, "ArkUIClipboardGetPixelMapData");
}

void ClipboardImpl::Clear() {}

void ClipboardImpl::RegisterCallbackSetClipboardPixmapData(CallbackSetClipboardPixmapData callback)
{
    callbackSetClipboardPixmapData_ = callback;
}

void ClipboardImpl::RegisterCallbackGetClipboardPixmapData(CallbackGetClipboardPixmapData callback)
{
    callbackGetClipboardPixmapData_ = callback;
}
} // namespace OHOS::Ace::Platform
