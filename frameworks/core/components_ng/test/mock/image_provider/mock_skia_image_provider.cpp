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
#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/image_provider/image_provider.h"
#include "core/components_ng/render/adapter/skia_image.h"
namespace OHOS::Ace::NG {
void ImageProvider::MakeCanvasImage(const WeakPtr<ImageObject>& objWp, const WeakPtr<ImageLoadingContext>& ctxWp,
    const SizeF& targetSize, bool forceResize, bool sync)
{
    auto ctx = ctxWp.Upgrade();
    ctx->SuccessCallback(MakeRefPtr<SkiaImage>(nullptr));
}

RefPtr<CanvasImage> ImageProvider::QueryCanvasImageFromCache(const ImageSourceInfo& src, const SizeF& targetSize)
{
    return nullptr;
}
} // namespace OHOS::Ace::NG