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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_ROOT_SCENE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_ROOT_SCENE_H

#include <event_handler.h>

#include "core/components_ng/pattern/window_scene/scene/container/window_pattern.h"

namespace OHOS::Ace::NG {

class RootScene : public WindowPattern {
    DECLARE_ACE_TYPE(RootScene, WindowPattern);

public:
    RootScene() = default;
    ~RootScene() override = default;

    void LoadContent(const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
        AbilityRuntime::Context* context = nullptr) override;

    void SetRootFrameNode(const RefPtr<NG::FrameNode>& root) override {}

    void Connect()  override {}
    void Foreground()  override {}
    void Background() override {}
    void Disconnect() override {}

private:
    void RegisterInputEventListener();

    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;

    ACE_DISALLOW_COPY_AND_MOVE(RootScene);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_ROOT_SCENE_H
