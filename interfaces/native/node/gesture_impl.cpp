/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstdint>

#include "native_gesture.h"
#include "native_node.h"
#include "native_type.h"
#include "node_model.h"
#include "gesture_impl.h"

#include "core/gestures/gesture_event.h"
#include "frameworks/core/interfaces/arkoala/arkoala_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ArkUI_GestureRecognizer {
    int32_t type = -1;
    ArkUIGesture* gesture = nullptr;
    void* extraData = nullptr;
};
#ifdef __cplusplus
};

// the ArkUI_GestureEvent struct actually same as ArkUIAPIEventGestureAsyncEvent;
struct ArkUI_GestureEvent {
    ArkUIAPIEventGestureAsyncEvent eventData;
};

#endif


ArkUI_GestureEventActionType OH_ArkUI_GestureEvent_GetActionType(const ArkUI_GestureEvent* event)
{
    ArkUI_GestureEventActionType ret;
    switch (event->eventData.subKind) {
        case ON_ACTION:
        case ON_ACTION_START:
            ret = GESTURE_EVENT_ACTION_ACCEPT;
            break;
        case ON_ACTION_UPDATE:
            ret = GESTURE_EVENT_ACTION_UPDATE;
            break;
        case ON_ACTION_END:
            ret = GESTURE_EVENT_ACTION_END;
            break;
        case ON_ACTION_CANCEL:
            ret = GESTURE_EVENT_ACTION_CANCEL;
            break;
        default:
            ret = GESTURE_EVENT_ACTION_ACCEPT;
            break;
    }
    return ret;
}

float OH_ArkUI_PanGesture_GetVelocity(const ArkUI_GestureEvent* event)
{
    return event->eventData.velocity;
}

float OH_ArkUI_PanGesture_GetVelocityX(const ArkUI_GestureEvent* event)
{
    return event->eventData.velocityX;
}

float OH_ArkUI_PanGesture_GetVelocityY(const ArkUI_GestureEvent* event)
{
    return event->eventData.velocityY;
}

float OH_ArkUI_PanGesture_GetOffsetX(const ArkUI_GestureEvent* event)
{
    return event->eventData.x;
}

float OH_ArkUI_PanGesture_GetOffsetY(const ArkUI_GestureEvent* event)
{
    return event->eventData.y;
}

namespace OHOS::Ace::GestureModel {

struct GestureInnerData {
    void (*targetReceiver)(ArkUI_GestureEvent* event, void* extraParam);
    void* extraParam;
};

ArkUI_GestureRecognizer* CreatePanGesture(int32_t fingersNum, ArkUI_GestureDirectionMask mask, double distanceNum)
{
    auto* gesture = OHOS::Ace::NodeModel::GetFullImpl()->getNodeModifiers()->getGestureModifier()->createPanGesture(
        fingersNum, mask, distanceNum);
    return new ArkUI_GestureRecognizer{ PAN_GESTURE, gesture, nullptr };
}

void DisposeGesture(ArkUI_GestureRecognizer* recognizer)
{
    OHOS::Ace::NodeModel::GetFullImpl()->getNodeModifiers()->getGestureModifier()->dispose(recognizer->gesture);
    delete reinterpret_cast<GestureInnerData*>(recognizer->extraData);
    delete recognizer;
}

int32_t SetGestureEventTarget(ArkUI_GestureRecognizer* recognizer, ArkUI_GestureEventActionTypeMask mask,
    void* extraParam, void (*targetReceiver)(ArkUI_GestureEvent* event, void* extraParam))
{
    // 手势事件通过通用的异步事件接口进行处理。(HandleGestureEvent)
    // 把回调函数和上下文都封装到内部结构体中。
    if (recognizer->extraData) {
        delete reinterpret_cast<GestureInnerData*>(recognizer->extraData);
    }
    recognizer->extraData = new GestureInnerData { targetReceiver, extraParam };
    OHOS::Ace::NodeModel::GetFullImpl()->getNodeModifiers()->getGestureModifier()->registerGestureEvent(
        recognizer->gesture, mask, recognizer->extraData);
    return 0;
}

int32_t AddGestureToNode(ArkUI_NodeHandle node, ArkUI_GestureRecognizer* recognizer, ArkUI_GesturePriority priorityNum,
    ArkUI_GestureMask mask)
{
    OHOS::Ace::NodeModel::GetFullImpl()->getNodeModifiers()->getGestureModifier()->addGestureToNode(
        node->uiNodeHandle, recognizer->gesture, priorityNum, mask);
    return 0;
}

void HandleGestureEvent(ArkUINodeEvent* event)
{
    auto* extraData = reinterpret_cast<GestureInnerData*>(event->extraParam);
    extraData->targetReceiver(reinterpret_cast<ArkUI_GestureEvent *>(&event->gestureAsyncEvent), extraData->extraParam);
    delete event;
}

}; // namespace OHOS::Ace::GestureModel