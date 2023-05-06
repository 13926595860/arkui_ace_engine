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

#include "core/components_ng/pattern/window_scene/scene/container/window_scene.h"

#include "session/container/include/scene_session_stage.h"

namespace OHOS::Ace::NG {

extern "C" ACE_EXPORT void* OHOS_ACE_CreateWindowScene(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    LOGI("Ace lib loaded, CreateWindowScene.");
    auto windowScene = std::make_shared<WindowScene>(context, iSession);
    return new std::shared_ptr<UIWindow>(windowScene);
}

WindowScene::WindowScene(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
    : WindowPattern(context)
{
    sessionStage_ = new Rosen::SceneSessionStage(iSession);
}

} // namespace OHOS::Ace::NG
