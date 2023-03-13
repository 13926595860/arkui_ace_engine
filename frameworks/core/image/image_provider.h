/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H

#include <string>

#include "flutter/fml/memory/ref_counted.h"
#include "third_party/skia/include/codec/SkCodec.h"

#include "base/memory/ace_type.h"
#include "base/resource/internal_resource.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/render/canvas_image.h"
#include "core/image/image_loader.h"
#include "core/image/image_source_info.h"
#include "core/pipeline/pipeline_base.h"

class SkSVGDOM;
namespace OHOS::Ace {

class ImageObject;

class SvgDom;
using ImageObjSuccessCallback = std::function<void(ImageSourceInfo, const RefPtr<ImageObject>)>;
using UploadSuccessCallback = std::function<void(ImageSourceInfo, const RefPtr<NG::CanvasImage>&)>;
using SvgDomSuccessCallback = std::function<void(ImageSourceInfo, const RefPtr<SvgDom>&)>;
using FailedCallback = std::function<void(ImageSourceInfo, const std::string&)>;
using CancelableTask = CancelableCallback<void()>;
using OnPostBackgroundTask = std::function<void(CancelableTask)>;

struct LoadCallback {
    LoadCallback(
        const ImageObjSuccessCallback& success, const UploadSuccessCallback& upload, const FailedCallback& failed)
        : successCallback(success), uploadCallback(upload), failedCallback(failed)
    {}
    ~LoadCallback() = default;

    ImageObjSuccessCallback successCallback;
    UploadSuccessCallback uploadCallback;
    FailedCallback failedCallback;
};

class FlutterRenderImage;
class ImageProvider {
public:
    static void TryLoadImageInfo(const RefPtr<PipelineBase>& context, const std::string& src,
        std::function<void(bool, int32_t, int32_t)>&& loadCallback);

#ifndef NG_BUILD
    static void GetSVGImageDOMAsyncFromSrc(const std::string& src, std::function<void(const sk_sp<SkSVGDOM>&)> callback,
        std::function<void()> failedCallback, const WeakPtr<PipelineBase> context, uint64_t svgThemeColor = 0,
        OnPostBackgroundTask onBackgroundTaskPostCallback = nullptr);

    static void GetSVGImageDOMAsyncFromData(const sk_sp<SkData>& skData,
        std::function<void(const sk_sp<SkSVGDOM>&)> callback, std::function<void()> failedCallback,
        const WeakPtr<PipelineBase> context, uint64_t svgThemeColor = 0,
        OnPostBackgroundTask onBackgroundTaskPostCallback = nullptr);
#endif

    // upload image data to gpu context for painting asynchronously.
    static void UploadImageToGPUForRender(const WeakPtr<PipelineBase> context, const sk_sp<SkImage>& image,
        const sk_sp<SkData>& data, const std::function<void(sk_sp<SkImage>, sk_sp<SkData>)>&& callback,
        const std::string src);

    // get out source image data asynchronously.
    static void FetchImageObject(const ImageSourceInfo& imageInfo, const ImageObjSuccessCallback& successCallback,
        const UploadSuccessCallback& uploadSuccessCallback, const FailedCallback& failedCallback,
        const WeakPtr<PipelineBase>& context, bool syncMode, bool useSkiaSvg, bool needAutoResize,
        const OnPostBackgroundTask& onBackgroundTaskPostCallback = nullptr);

    static sk_sp<SkImage> ResizeSkImage(
        const sk_sp<SkImage>& rawImage, const std::string& src, Size imageSize, bool forceResize = false);

    static sk_sp<SkImage> ApplySizeToSkImage(
        const sk_sp<SkImage>& rawImage, int32_t dstWidth, int32_t dstHeight, const std::string& srcKey = std::string());

    static bool IsWideGamut(const sk_sp<SkColorSpace>& colorSpace);

    static bool NeedExchangeWidthAndHeight(SkEncodedOrigin origin);

    // This is a synchronization interface for getting out source image.
    static sk_sp<SkImage> GetSkImage(
        const std::string& src, const WeakPtr<PipelineBase> context, Size targetSize = Size());

    static RefPtr<ImageObject> GeneratorAceImageObject(
        const ImageSourceInfo& imageInfo, const RefPtr<PipelineBase> context, bool useSkiaSvg);

    static sk_sp<SkData> LoadImageRawData(const ImageSourceInfo& imageInfo, const RefPtr<PipelineBase> context);

    static sk_sp<SkData> LoadImageRawDataFromFileCache(
        const RefPtr<PipelineBase> context, const std::string key, const std::string suffix = "");

    static RefPtr<ImageObject> QueryImageObjectFromCache(
        const ImageSourceInfo& imageInfo, const RefPtr<PipelineBase>& pipelineContext);
    static SkColorType PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap);
    static SkAlphaType AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap);
    static SkImageInfo MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap);
    static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap);

    static bool TrySetLoadingImage(const ImageSourceInfo& imageInfo, const ImageObjSuccessCallback& successCallback,
        const UploadSuccessCallback& uploadCallback, const FailedCallback& failedCallback);

    static void ProccessLoadingResult(const RefPtr<TaskExecutor>& taskExecutor, const ImageSourceInfo& imageInfo,
        bool canStartUploadImageObj, const RefPtr<ImageObject>& imageObj, const RefPtr<PipelineBase>& context,
        const std::string& errorMsg = "");

    static bool TryUploadingImage(
        const std::string& key, const UploadSuccessCallback& successCallback, const FailedCallback& failedCallback);

    static void ProccessUploadResult(const RefPtr<TaskExecutor>& taskExecutor, const ImageSourceInfo& imageInfo,
        const Size& imageSize, const RefPtr<NG::CanvasImage>& canvasImage, const std::string& errorMsg = "");

private:
    static std::mutex loadingImageMutex_;
    static std::unordered_map<std::string, std::vector<LoadCallback>> loadingImage_;

    static std::mutex uploadMutex_;
    static std::unordered_map<std::string, std::vector<LoadCallback>> uploadingImage_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H
