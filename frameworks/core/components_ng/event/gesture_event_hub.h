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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_GESTURE_EVENT_HUB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_GESTURE_EVENT_HUB_H

#include <list>

#include "base/memory/referenced.h"
#include "core/components_ng/event/click_event.h"
#include "core/components_ng/event/pan_event.h"
#include "core/components_ng/event/scrollable_event.h"
#include "core/components_ng/event/touch_event.h"

namespace OHOS::Ace::NG {

class EventHub;

// The gesture event hub is mainly used to handle common gesture events.
class ACE_EXPORT GestureEventHub : public Referenced {
public:
    explicit GestureEventHub(const WeakPtr<EventHub>& eventHub);
    ~GestureEventHub() override = default;

    void AddGesture(const RefPtr<Gesture>& gesture)
    {
        if (!recreateGesture_) {
            gestures_.clear();
        }
        gestures_.emplace_back(gesture);
        recreateGesture_ = true;
    }

    void AddScrollableEvent(const RefPtr<ScrollableEvent>& scrollableEvent)
    {
        if (!scrollableActuator_) {
            scrollableActuator_ = MakeRefPtr<ScrollableActuator>(WeakClaim(this));
        }
        scrollableActuator_->AddScrollableEvent(scrollableEvent);
    }

    void RemoveScrollableEvent(const RefPtr<ScrollableEvent>& scrollableEvent)
    {
        if (!scrollableActuator_) {
            return;
        }
        scrollableActuator_->RemoveScrollableEvent(scrollableEvent);
    }

    // Set by user define, which will replace old one.
    void SetTouchEvent(TouchEventFunc&& touchEventFunc)
    {
        if (!touchEventActuator_) {
            touchEventActuator_ = MakeRefPtr<TouchEventActuator>();
        }
        touchEventActuator_->ReplaceTouchEvent(std::move(touchEventFunc));
    }

    void AddTouchEvent(const RefPtr<TouchEventImpl>& touchEvent)
    {
        if (!touchEventActuator_) {
            touchEventActuator_ = MakeRefPtr<TouchEventActuator>();
        }
        touchEventActuator_->AddTouchEvent(touchEvent);
    }

    void RemoveTouchEvent(const RefPtr<TouchEventImpl>& touchEvent)
    {
        if (!touchEventActuator_) {
            return;
        }
        touchEventActuator_->RemoveTouchEvent(touchEvent);
    }

    // Set by user define, which will replace old one.
    void SetClickEvent(GestureEventFunc&& clickEvent)
    {
        if (!clickEventActuator_) {
            clickEventActuator_ = MakeRefPtr<ClickEventActuator>(WeakClaim(this));
        }
        clickEventActuator_->ReplaceClickEvent(std::move(clickEvent));
    }

    void AddClickEvent(const RefPtr<ClickEvent>& clickEvent)
    {
        if (!clickEventActuator_) {
            clickEventActuator_ = MakeRefPtr<ClickEventActuator>(WeakClaim(this));
        }
        clickEventActuator_->AddClickEvent(clickEvent);
    }

    void RemoveClickEvent(const RefPtr<ClickEvent>& clickEvent)
    {
        if (!clickEventActuator_) {
            return;
        }
        clickEventActuator_->RemoveClickEvent(clickEvent);
    }

    // Set by user define, which will replace old one.
    void SetPanEvent(const RefPtr<PanEvent>& panEvent, PanDirection direction, int32_t fingers, float distance)
    {
        if (!panEventActuator_) {
            panEventActuator_ = MakeRefPtr<PanEventActuator>(WeakClaim(this), direction, fingers, distance);
        }
        panEventActuator_->ReplacePanEvent(panEvent);
    }

    void AddPanEvent(const RefPtr<PanEvent>& panEvent, PanDirection direction, int32_t fingers, float distance)
    {
        if (!panEventActuator_) {
            panEventActuator_ = MakeRefPtr<PanEventActuator>(WeakClaim(this), direction, fingers, distance);
        }
        panEventActuator_->AddPanEvent(panEvent);
    }

    void RemovePanEvent(const RefPtr<PanEvent>& panEvent)
    {
        if (!panEventActuator_) {
            return;
        }
        panEventActuator_->RemovePanEvent(panEvent);
    }

    // the return value means prevents event bubbling.
    bool ProcessTouchTestHit(const OffsetF& coordinateOffset, const TouchRestrict& touchRestrict,
        TouchTestResult& innerTargets, TouchTestResult& finalResult);

    RefPtr<FrameNode> GetFrameNode() const;

    void OnContextAttached() {}

private:
    void ProcessTouchTestHierarchy(const OffsetF& coordinateOffset, const TouchRestrict& touchRestrict,
        std::list<RefPtr<GestureRecognizer>>& innerRecognizers, TouchTestResult& finalResult);

    void UpdateGestureHierarchy();

    WeakPtr<EventHub> eventHub_;
    RefPtr<ScrollableActuator> scrollableActuator_;
    RefPtr<TouchEventActuator> touchEventActuator_;
    RefPtr<ClickEventActuator> clickEventActuator_;
    RefPtr<PanEventActuator> panEventActuator_;

    // Set by use gesture, priorityGesture and parallelGesture attribute function.
    std::list<RefPtr<Gesture>> gestures_;
    std::list<RefPtr<GestureRecognizer>> gestureHierarchy_;
    bool recreateGesture_ = true;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_GESTURE_EVENT_HUB_H