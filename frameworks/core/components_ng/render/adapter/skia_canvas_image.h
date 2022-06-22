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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_ADAPTER_SKIA_CANVAS_IMAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_ADAPTER_SKIA_CANVAS_IMAGE_H

#include "flutter/lib/ui/painting/image.h"

#include "core/components_ng/render/canvas_image.h"

namespace OHOS::Ace::NG {

class SkiaCanvasImage : public CanvasImage {
    DECLARE_ACE_TYPE(SkiaCanvasImage, CanvasImage)
public:
    SkiaCanvasImage(const fml::RefPtr<flutter::CanvasImage>& image) : image_(image) {}

    ~SkiaCanvasImage() override = default;

    sk_sp<SkImage> GetCanvasImage() const
    {
        if (image_) {
            return image_->image();
        }
        return nullptr;
    }

private:
    // TODO: add adapter for image.
    fml::RefPtr<flutter::CanvasImage> image_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_ADAPTER_SKIA_CANVAS_IMAGE_H
