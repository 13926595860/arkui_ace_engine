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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_REFRESH_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_REFRESH_H

#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {

class JSRefresh : public JSContainerBase {
public:
    static bool ParseCustomBuilder(const JSCallbackInfo& info);
    static bool ParseRefreshingContent(const JSRef<JSObject>& paramObject);
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);
    static void OnStateChange(const JSCallbackInfo& args);
    static void OnRefreshing(const JSCallbackInfo& args);
    static void OnOffsetChange(const JSCallbackInfo& args);
    static void SetPullDownRatio(const JSCallbackInfo& info);
    static void SetMaxPullDownDistance(const JSCallbackInfo& info);
    static void ParsFrictionData(const JsiRef<JsiValue>& value);

private:
    static void JsRefreshOffset(const JSCallbackInfo& info);
    static void JsRefreshOffset(const JSRef<JSVal>& jsValue);
    static void SetPullToRefresh(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_REFRESH_H
