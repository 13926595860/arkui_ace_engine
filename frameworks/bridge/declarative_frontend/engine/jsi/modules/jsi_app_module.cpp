/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/engine/jsi/modules/jsi_app_module.h"

#include "bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#include "bridge/js_frontend/engine/common/js_constants.h"
#include "core/common/container.h"
#include "core/image/image_file_cache.h"

namespace OHOS::Ace::Framework {

shared_ptr<JsValue> AppGetInfo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        return runtime->NewNull();
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        return runtime->NewNull();
    }

    shared_ptr<JsValue> appInfo = runtime->NewObject();
    auto appId = delegate->GetAppID();
    auto appName = delegate->GetAppName();
    auto versionName = delegate->GetVersionName();
    auto versionCode = delegate->GetVersionCode();
    appInfo->SetProperty(runtime, "appID", runtime->NewString(appId));
    appInfo->SetProperty(runtime, "appName", runtime->NewString(appName));
    appInfo->SetProperty(runtime, "versionName", runtime->NewString(versionName));
    appInfo->SetProperty(runtime, "versionCode", runtime->NewNumber(versionCode));
    return appInfo;
}

shared_ptr<JsValue> AppTerminate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        return runtime->NewNull();
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        return runtime->NewNull();
    }
    auto pipelineContext = delegate->GetPipelineContext();
    if (!pipelineContext) {
        return runtime->NewNull();
    }
    auto uiTaskExecutor = delegate->GetUiTask();
    WeakPtr<PipelineBase> pipelineContextWeak(pipelineContext);
    uiTaskExecutor.PostTask(
        [pipelineContextWeak]() mutable {
            auto pipelineContext = pipelineContextWeak.Upgrade();
            if (pipelineContext) {
                pipelineContext->Finish();
            }
        },
        "ArkUIAppTerminate");
    return runtime->NewNull();
}

shared_ptr<JsValue> AppSetImageCacheCount(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto container = Container::Current();
    if (!container) {
        return runtime->NewNull();
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        return runtime->NewNull();
    }
    if (argc != 1 || !argv[0]->IsNumber(runtime)) {
        return runtime->NewNull();
    }
    int32_t size = argv[0]->ToInt32(runtime);
    if (size < 0) {
        return runtime->NewNull();
    }
    auto taskExecutor = pipelineContext->GetTaskExecutor();
    if (!taskExecutor) {
        return runtime->NewNull();
    }
    WeakPtr<PipelineBase> pipelineContextWeak(pipelineContext);
    taskExecutor->PostTask(
        [pipelineContextWeak, size]() mutable {
            auto pipelineContext = pipelineContextWeak.Upgrade();
            if (pipelineContext) {
                auto imageCache = pipelineContext->GetImageCache();
                if (imageCache) {
                    imageCache->SetCapacity(size);
                }
            }
        },
        TaskExecutor::TaskType::UI, "ArkUISetImageCacheCount");
    return runtime->NewNull();
}

shared_ptr<JsValue> AppSetImageRawDataCacheSize(
    const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto container = Container::Current();
    if (!container) {
        return runtime->NewNull();
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        return runtime->NewNull();
    }
    if (argc != 1 || !argv[0]->IsNumber(runtime)) {
        return runtime->NewNull();
    }
    int32_t size = argv[0]->ToInt32(runtime);
    if (size < 0) {
        return runtime->NewNull();
    }
    auto taskExecutor = pipelineContext->GetTaskExecutor();
    if (!taskExecutor) {
        return runtime->NewNull();
    }
    WeakPtr<PipelineBase> pipelineContextWeak(pipelineContext);
    taskExecutor->PostTask(
        [pipelineContextWeak, size]() mutable {
            auto pipelineContext = pipelineContextWeak.Upgrade();
            if (pipelineContext) {
                auto imageCache = pipelineContext->GetImageCache();
                if (imageCache) {
                    imageCache->SetDataCacheLimit(size);
                }
            }
        },
        TaskExecutor::TaskType::UI, "ArkUISetImageDataCacheSize");
    return runtime->NewNull();
}

shared_ptr<JsValue> AppSetImageFileCacheSize(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1 || !argv[0]->IsNumber(runtime)) {
        return runtime->NewNull();
    }
    int32_t size = argv[0]->ToInt32(runtime);
    if (size < 0) {
        return runtime->NewNull();
    }
    ImageFileCache::GetInstance().SetCacheFileLimit(size);
    return runtime->NewNull();
}

shared_ptr<JsValue> AppRequestFullWindow(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return runtime->NewNull();
}

shared_ptr<JsValue> AppScreenOnVisible(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return runtime->NewNull();
}

void InitAppModule(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj)
{
    moduleObj->SetProperty(runtime, APP_GET_INFO, runtime->NewFunction(AppGetInfo));
    moduleObj->SetProperty(runtime, APP_TERMINATE, runtime->NewFunction(AppTerminate));
    moduleObj->SetProperty(runtime, APP_SET_IMAGE_CACHE_COUNT, runtime->NewFunction(AppSetImageCacheCount));
    moduleObj->SetProperty(
        runtime, APP_SET_IMAGE_RAWDATA_CACHE_SIZE, runtime->NewFunction(AppSetImageRawDataCacheSize));
    moduleObj->SetProperty(runtime, APP_SET_IMAGE_FILE_CACHE_SIZE, runtime->NewFunction(AppSetImageFileCacheSize));

    moduleObj->SetProperty(runtime, APP_REQUEST_FULL_WINDOW, runtime->NewFunction(AppRequestFullWindow));
    moduleObj->SetProperty(runtime, APP_SCREEN_ON_VISIBLE, runtime->NewFunction(AppScreenOnVisible));
}

} // namespace OHOS::Ace::Framework