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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_FILTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_FILTER_H

#include "frameworks/core/components/svg/parse/svg_node.h"
#include "frameworks/core/components/svg/svg_filter_component.h"

namespace OHOS::Ace {

class SvgFilter : public SvgNode {
    DECLARE_ACE_TYPE(SvgFilter, SvgNode);

public:
    SvgFilter();
    ~SvgFilter() override = default;
    static RefPtr<SvgNode> Create();

    void SetAttr(const std::string& name, const std::string& value) override;
    RefPtr<RenderNode> CreateRender(
        const LayoutParam& layoutParam, const RefPtr<SvgBaseDeclaration>& parent, bool useBox = true) override;
    bool ProcessIteratively(const LayoutParam& layoutParam, std::stack<SvgCreateRenderInfo>& createRenderTaskSt,
        SvgCreateRenderInfo& svgCreateRenderInfo) override;
    bool BeforeChildrenProcessed(SvgCreateRenderInfo& svgCreateRenderInfo) override;
    void AppendChild(const RefPtr<SvgNode>& child) override;
    void Update(RefPtr<RenderNode>& node) override;
    bool IsCreateRenderRecursive() const override
    {
        return true;
    }

    RefPtr<Component> GetComponent() const override
    {
        return component_;
    }

private:
    RefPtr<SvgFilterComponent> component_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_FILTER_H