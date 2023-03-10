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

#include "core/pipeline_ng/ui_task_scheduler.h"

#include "base/memory/referenced.h"
#include "base/thread/background_task_executor.h"
#include "base/thread/cancelable_callback.h"
#include "base/utils/time_util.h"
#include "base/utils/utils.h"
#include "core/common/thread_checker.h"
#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {

UITaskScheduler::~UITaskScheduler() = default;

void UITaskScheduler::AddDirtyLayoutNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    CHECK_NULL_VOID(dirty);
    dirtyLayoutNodes_[dirty->GetPageId()].emplace(dirty);
}

void UITaskScheduler::AddDirtyRenderNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    CHECK_NULL_VOID(dirty);
    auto result = dirtyRenderNodes_[dirty->GetPageId()].emplace(dirty);
    if (!result.second) {
        LOGW("fail to emplace %{public}s render node", dirty->GetTag().c_str());
    }
}

static inline bool Cmp(const RefPtr<FrameNode>& nodeA, const RefPtr<FrameNode>& nodeB)
{
    if (!nodeA || !nodeB) {
        return false;
    }
    return nodeA->GetLayoutPriority() > nodeB->GetLayoutPriority();
}

void UITaskScheduler::FlushLayoutTask(bool forceUseMainThread)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    auto dirtyLayoutNodes = std::move(dirtyLayoutNodes_);
    std::vector<RefPtr<FrameNode>> orderedNodes;
    bool needReorder = false;
    for (auto&& pageNodes : dirtyLayoutNodes) {
        for (auto&& node : pageNodes.second) {
            if (!node || node->IsInDestroying()) {
                continue;
            }
            orderedNodes.emplace_back(node);
            needReorder = node->GetLayoutPriority() != 0 ? true : needReorder;
        }
    }
    if (needReorder) {
        std::sort(orderedNodes.begin(), orderedNodes.end(), Cmp);
    }

    // Priority task creation
    uint64_t time = 0;
    for (auto& node : orderedNodes) {
        if (!node) {
            continue;
        }
        time = GetSysTimestamp();
        auto task = node->CreateLayoutTask(forceUseMainThread);
        if (task) {
            if (forceUseMainThread || (task->GetTaskThreadType() == MAIN_TASK)) {
                (*task)();
                time = GetSysTimestamp() - time;
                if (frameInfo_ != nullptr) {
                    frameInfo_->AddTaskInfo(node->GetTag(), node->GetId(), time, FrameInfo::TaskType::LAYOUT);
                }
            } else {
                LOGW("need to use multithread feature");
            }
        }
    }
}

void UITaskScheduler::FlushRenderTask(bool forceUseMainThread)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    auto dirtyRenderNodes = std::move(dirtyRenderNodes_);
    // Priority task creation
    uint64_t time = 0;
    for (auto&& pageNodes : dirtyRenderNodes) {
        for (auto&& node : pageNodes.second) {
            if (!node) {
                continue;
            }
            if (node->IsInDestroying()) {
                continue;
            }
            time = GetSysTimestamp();
            auto task = node->CreateRenderTask(forceUseMainThread);
            if (task) {
                if (forceUseMainThread || (task->GetTaskThreadType() == MAIN_TASK)) {
                    (*task)();
                    time = GetSysTimestamp() - time;
                    if (frameInfo_ != nullptr) {
                        frameInfo_->AddTaskInfo(node->GetTag(), node->GetId(), time, FrameInfo::TaskType::RENDER);
                    }
                } else {
                    LOGW("need to use multithread feature");
                }
            }
        }
    }
}

bool UITaskScheduler::NeedAdditionalLayout()
{
    // if dirtynodes still exist after layout done as new dirty nodes are added during layout,
    // we need to initiate the additional layout, under normal build layout workflow the additional
    // layout will not be excuted.
    if (dirtyLayoutNodes_.empty()) {
        return false;
    }
    for (auto&& pageNodes : dirtyLayoutNodes_) {
        for (auto&& node : pageNodes.second) {
            if (!node || !node->GetLayoutProperty()) {
                continue;
            }
            const auto& geometryTransition = node->GetLayoutProperty()->GetGeometryTransition();
            if (!geometryTransition || !geometryTransition->IsNodeInAndActive(node)) {
                continue;
            }
            node->GetLayoutProperty()->CleanDirty();
            node->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
            LOGD("GeometryTransition needs additional layout, node%{public}d is marked dirty", node->GetId());
            auto parent = node->GetParent();
            while (parent) {
                auto frameNode = AceType::DynamicCast<FrameNode>(parent);
                if (frameNode) {
                    frameNode->GetLayoutProperty()->CleanDirty();
                    frameNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
                    LOGD("GeometryTransition needs additional layout, parent node%{public}d is marked dirty",
                        frameNode->GetId());
                    break;
                }
                parent = parent->GetParent();
            }
        }
    }
    return true;
}

void UITaskScheduler::FlushTask()
{
    CHECK_RUN_ON(UI);
    ACE_SCOPED_TRACE("UITaskScheduler::FlushTask");
    FlushLayoutTask();
    // after first layout done if dirtynodes still exist due to new dirty nodes are added during fist layout,
    // we need to initiate the second layout, otherwise the second will not be excuted.
    if (NeedAdditionalLayout()) {
        FlushLayoutTask();
    }
    ElementRegister::GetInstance()->ClearPendingRemoveNodes();
    FlushRenderTask();
}

void UITaskScheduler::AddPredictTask(PredictTask&& task)
{
    predictTask_.push_back(std::move(task));
}

void UITaskScheduler::FlushPredictTask(int64_t deadline)
{
    decltype(predictTask_) tasks(std::move(predictTask_));
    for (const auto& task : tasks) {
        if (task) {
            task(deadline);
        }
    }
}

void UITaskScheduler::CleanUp()
{
    dirtyLayoutNodes_.clear();
    dirtyRenderNodes_.clear();
}

bool UITaskScheduler::isEmpty()
{
    if (dirtyLayoutNodes_.empty() && dirtyRenderNodes_.empty()) {
        return true;
    }
    return false;
}

} // namespace OHOS::Ace::NG
