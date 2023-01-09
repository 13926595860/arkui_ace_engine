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

#include "frameworks/bridge/declarative_frontend/jsview/js_sceneview.h"

#include "base/geometry/quaternion.h"
#include "base/geometry/vec3.h"
#include "bridge/declarative_frontend/jsview/models/model_view_impl.h"
#include "core/components_ng/pattern/model/model_view_ng.h"
#include "core/components/scene_viewer/scene_viewer_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "foundation/graphic/graphic_3d/3d_widget_adapter/include/custom/custom_render_descriptor.h"
#include "foundation/graphic/graphic_3d/3d_widget_adapter/include/data_type/geometry/cone.h"
#include "foundation/graphic/graphic_3d/3d_widget_adapter/include/data_type/geometry/cube.h"
#include "foundation/graphic/graphic_3d/3d_widget_adapter/include/data_type/geometry/sphere.h"

namespace OHOS::Ace {

std::unique_ptr<ModelView> ModelView::instance_ = nullptr;

ModelView* ModelView::GetInstance()
{
    if (!instance_) {
#ifdef NG_BUILD
        instance_.reset(new NG::ModelViewNG());
#else
        if (Container::IsCurrentUseNewPipeline()) {
            LOGD("ModelView::GetInstance() NG Pipeline");
            instance_.reset(new NG::ModelViewNG());
        } else {
            LOGD("ModelView::GetInstance() NOT NG Pipeline");
            instance_.reset(new Framework::ModelViewImpl());
        }
#endif
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {

void JSSceneView::JsOnClick(const JSCallbackInfo& info)
{
    LOGD("JSSceneView JsOnClick");
    if (info[0]->IsFunction()) {
        RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
        auto onClickId = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc)]() {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                LOGD("About to call onclick method on js");
                func->Execute();
            });
        auto top = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto component = AceType::DynamicCast<OHOS::Ace::SceneViewerComponent>(top);

        if (!component) {
            LOGE("Cannot assign click handler. Component is not SceneViewComponent");
            return;
        }
        component->SetClickedEventId(onClickId);
    }
}

void JSSceneView::JsSetHandleCameraMove(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsBoolean()) {
        LOGE("arg is not Boolean.");
        return;
    }

    bool value = info[0]->ToBoolean();
    ModelView::GetInstance()->SetHandleCameraMove(value);
}

void JSSceneView::Create(const JSCallbackInfo& info)
{
    LOGD("JSSceneView::Create()");
    if (info.Length() < 1) {
        LOGE("JSSceneView: info is invalid.");
        return;
    }

    std::string srcPath;
    auto parseOk = ParseJsMedia(info[0], srcPath);
    if (!parseOk) {
        LOGW("JSSceneView::Create() arg parsing failed.");
        return;
    }

    LOGD("srcPath after ParseJsMedia(): %s", srcPath.c_str());
    ModelView::GetInstance()->Create(srcPath);
}

void JSSceneView::JSCamera(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() <= 0 || !info[0]->IsObject()) {
        LOGE("JSSceneView: arg is invalid.");
        return;
    }

    LOGD("info[0]->ToString().c_str(): %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    // Using single/same animation option for both Position and Rotation currently.
    AnimationOption animOption = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();

    // Default Camera zNear, zFar, fovDegrees.
    auto zNear = 0.5f;
    auto zFar = 50.0f;
    auto fovD = 60.0f;
    if (argsPtrItem->Contains("zNear")) {
        zNear = argsPtrItem->GetDouble("zNear", zNear);
        LOGD("zNear: %f", zNear);
    }

    if (argsPtrItem->Contains("zFar")) {
        zFar = argsPtrItem->GetDouble("zFar", zFar);
        LOGD("zFar: %f", zFar);
    }

    if (argsPtrItem->Contains("fovD")) {
        fovD = argsPtrItem->GetDouble("fovD", fovD);
        LOGD("fovD: %f", fovD);
    }

    ModelView::GetInstance()->SetCameraFrustum(zNear, zFar, fovD);

    // positionInAngles
    if (argsPtrItem->Contains("positionInAngles")) {
        auto positionArgs = argsPtrItem->GetObject("positionInAngles");
        auto x = positionArgs->GetDouble("x", 0.0);
        auto y = positionArgs->GetDouble("y", 0.0);
        auto z = positionArgs->GetDouble("z", 0.0);
        auto distance = positionArgs->GetDouble("distance", 4.0);
        LOGD("positionInAngles: x: %f, y: %f, z: %f, distance: %f,", x, y, z, distance);
        bool isAngularPosition = true;
        // Angles and distance are animatable.
        ModelView::GetInstance()->SetCameraPosition(
            AnimatableFloat(x, animOption), AnimatableFloat(y, animOption),
            AnimatableFloat(z, animOption), AnimatableFloat(distance, animOption), isAngularPosition);
    }

    // position (Not positionInAngles)
    if (!argsPtrItem->Contains("positionInAngles") && argsPtrItem->Contains("position")) {
        auto positionArgs = argsPtrItem->GetObject("position");
        auto x = positionArgs->GetDouble("x", 0.0);
        auto y = positionArgs->GetDouble("y", 0.0);
        auto z = positionArgs->GetDouble("z", 0.0);
        LOGD("position: x: %f, y: %f, z: %f", x, y, z);
        bool isAngularPosition = false;
        // Angles and distance are animatable.
        ModelView::GetInstance()->SetCameraPosition(
            AnimatableFloat(x, animOption), AnimatableFloat(y, animOption),
            AnimatableFloat(z, animOption), AnimatableFloat(0.0, animOption), isAngularPosition);
    }

    // rotation: { x : number, y : number, z : number, w : number}
    if (argsPtrItem->Contains("rotation")) {
        auto rotationArgs = argsPtrItem->GetObject("rotation");
        auto x = rotationArgs->GetDouble("x", 0.0);
        auto y = rotationArgs->GetDouble("y", 0.0);
        auto z = rotationArgs->GetDouble("z", 0.0);
        auto w = rotationArgs->GetDouble("w", 1.0);
        LOGD("rotation: x: %f, y: %f, z: %f, w: %f", x, y, z, w);
        Quaternion rotation(x, y, z, w);
        ModelView::GetInstance()->SetCameraRotation(rotation);
    }

    // lookAt / target : {x : number, y : number, z : number }
    if (argsPtrItem->Contains("lookAt")) {
        auto lookatArgs = argsPtrItem->GetObject("lookAt");
        auto x = lookatArgs->GetDouble("x", 0.0);
        auto y = lookatArgs->GetDouble("y", 0.0);
        auto z = lookatArgs->GetDouble("z", 0.0);
        LOGD("lookAt: x: %f, y: %f, z: %f", x, y, z);
        Vec3 lookVec(x, y, z);
        ModelView::GetInstance()->SetCameraLookAt(lookVec);
    }

    // up: {x : number, y : number, z : number } (Default: 0,1,0)
    if (argsPtrItem->Contains("up")) {
        auto upArgs = argsPtrItem->GetObject("up");
        auto x = upArgs->GetDouble("x", 0.0);
        auto y = upArgs->GetDouble("y", 1.0);
        auto z = upArgs->GetDouble("z", 0.0);
        LOGD("upArgs: x: %f, y: %f, z: %f", x, y, z);
        Vec3 upVec(x, y, z);
        ModelView::GetInstance()->SetCameraUp(upVec);
    }
}

void JSSceneView::JSSetTransparent(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsBoolean()) {
        LOGE("arg is not Boolean.");
        return;
    }

    bool value = info[0]->ToBoolean();
    LOGD("JSSceneView::JSSetTransparentBackground(%s)", value ? "true" : "false");
    ModelView::GetInstance()->SetTransparent(value);
}

void JSSceneView::JSSetBackground(const JSCallbackInfo& info)
{
    LOGD("JSSceneView::JSSetBackground()");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    std::string srcPath;
    auto parseOk = ParseJsMedia(info[0], srcPath);
    if (!parseOk) {
        LOGE("JSSceneView::JSSetBackground() arg parsing failed.");
        return;
    }

    LOGD("srcPath after ParseJsMedia(): %s", srcPath.c_str());
    ModelView::GetInstance()->SetBackground(srcPath);
}

void JSSceneView::JSLight(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() <= 0 || !info[0]->IsObject()) {
        LOGE("JSSceneView: arg is invalid.");
        return;
    }

    LOGD("JSLight() info[0]: %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    AnimationOption animOption = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    auto type = argsPtrItem->GetInt("type", 1);
    auto intensity = argsPtrItem->GetDouble("intensity", 10.0);
    auto shadow = argsPtrItem->GetBool("shadow", false);
    LOGD("type: %d, intensity: %f, shadow: %d", type, intensity, shadow);

    Vec3 color = Vec3(1.0, 1.0, 1.0, animOption);
    if (argsPtrItem->Contains("color")) {
        LOGD("JSLight color");
        auto colorArgs = argsPtrItem->GetObject("color");
        if (!colorArgs || colorArgs->IsNull()) {
            LOGE("Js Parse object failed. light color is null. %s", colorArgs->ToString().c_str());
        } else {
            auto r = colorArgs->GetDouble("x", 1.0);
            auto g = colorArgs->GetDouble("y", 1.0);
            auto b = colorArgs->GetDouble("z", 1.0);
            color = Vec3(r, g, b, animOption);
            LOGD("color r: %f, g: %f, b: %f", color.GetX(), color.GetY(), color.GetZ());
        }
    }

    OHOS::Render3D::Position position;
    // positionInAngles
    if (argsPtrItem->Contains("positionInAngles")) {
        auto positionArgs = argsPtrItem->GetObject("positionInAngles");
        auto x = positionArgs->GetDouble("x", 0.0);
        auto y = positionArgs->GetDouble("y", 0.0);
        auto z = positionArgs->GetDouble("z", 0.0);
        auto distance = positionArgs->GetDouble("distance", 4.0);
        bool isAngularPosition = true;
        LOGD("positionInAngles: x: %f, y: %f, z: %f, distance: %f,", x, y, z, distance);
        position.Set(AnimatableFloat(x, animOption), AnimatableFloat(y, animOption),
            AnimatableFloat(z, animOption), AnimatableFloat(distance, animOption), isAngularPosition);
    }

    // position (Not positionInAngles)
    if (!argsPtrItem->Contains("positionInAngles") && argsPtrItem->Contains("position")) {
        auto positionArgs = argsPtrItem->GetObject("position");
        auto x = positionArgs->GetDouble("x", 0.0);
        auto y = positionArgs->GetDouble("y", 0.0);
        auto z = positionArgs->GetDouble("z", 0.0);
        LOGD("position: x: %f, y: %f, z: %f", x, y, z);
        bool isAngularPosition = false;
        position.Set(AnimatableFloat(x, animOption), AnimatableFloat(y, animOption),
            AnimatableFloat(z, animOption), AnimatableFloat(0.0, animOption), isAngularPosition);
    }

    double maxInvalid = std::numeric_limits<double>::max();
    Quaternion rotation = Quaternion(maxInvalid, maxInvalid, maxInvalid, maxInvalid);
    // rotation: { x : number, y : number, z : number, w : number}
    if (argsPtrItem->Contains("rotation")) {
        auto rotationArgs = argsPtrItem->GetObject("rotation");
        auto x = rotationArgs->GetDouble("x", 0.0);
        auto y = rotationArgs->GetDouble("y", 0.0);
        auto z = rotationArgs->GetDouble("z", 0.0);
        auto w = rotationArgs->GetDouble("w", 1.0);
        LOGD("rotation: x: %f, y: %f, z: %f, w: %f", x, y, z, w);
        rotation = Quaternion(x, y, z, w);
    }

    ModelView::GetInstance()->AddLight(AceType::MakeRefPtr<OHOS::Render3D::SVLight>(
        type, color, AnimatableFloat(intensity, animOption), shadow, position, rotation));
}

void JSSceneView::JSAddCube(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() <= 0 || !info[0]->IsObject()) {
        LOGE("JSSceneView: arg is invalid.");
        return;
    }

    LOGD("JSAddCube() info[0]: %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    auto name = argsPtrItem->GetString("name");
    auto width = argsPtrItem->GetDouble("width", 0.0);
    auto height = argsPtrItem->GetDouble("height", 0.0);
    auto depth = argsPtrItem->GetDouble("depth", 0.0);

    OHOS::Render3D::Vec3 position(0.0f, 0.0f, 0.0f);
    if (argsPtrItem->Contains("position")) {
        LOGD("Cube position");
        auto positionArgs = argsPtrItem->GetObject("position");
        position.SetX(positionArgs->GetDouble("x", 0.0));
        position.SetY(positionArgs->GetDouble("y", 0.0));
        position.SetZ(positionArgs->GetDouble("z", 0.0));
    }

    LOGD("JSAddCube(%s, %.2f, %.2f, %.2f)", name.c_str(), width, height, depth);
    ModelView::GetInstance()->AddGeometry(
        AceType::MakeRefPtr<OHOS::Render3D::SVCube>(name.c_str(), width, height, depth, position));
}

void JSSceneView::JSAddSphere(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() <= 0 || !info[0]->IsObject()) {
        LOGE("JSSceneView: arg is invalid.");
        return;
    }

    LOGD("JSAddSphere() info[0]: %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    auto name = argsPtrItem->GetString("name");
    auto radius = argsPtrItem->GetDouble("radius", 0.0);
    auto rings = argsPtrItem->GetInt("rings", 0);
    auto sectors = argsPtrItem->GetInt("sectors", 0);

    OHOS::Render3D::Vec3 position(0.0f, 0.0f, 0.0f);
    if (argsPtrItem->Contains("position")) {
        LOGD("Cube position");
        auto positionArgs = argsPtrItem->GetObject("position");
        position.SetX(positionArgs->GetDouble("x", 0.0));
        position.SetY(positionArgs->GetDouble("y", 0.0));
        position.SetZ(positionArgs->GetDouble("z", 0.0));
    }

    LOGD("JSAddSphere(%s, %.2f, %d, %d)", name.c_str(), radius, rings, sectors);
    ModelView::GetInstance()->AddGeometry(
        AceType::MakeRefPtr<OHOS::Render3D::SVSphere>(name.c_str(), radius, rings, sectors, position));
}

void JSSceneView::JSAddCone(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() <= 0 || !info[0]->IsObject()) {
        LOGE("JSSceneView: arg is invalid.");
        return;
    }

    LOGD("JSAddCone() info[0]: %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    auto name = argsPtrItem->GetString("name");
    auto radius = argsPtrItem->GetDouble("radius", 0.0);
    auto length = argsPtrItem->GetInt("length", 0);
    auto sectors = argsPtrItem->GetInt("sectors", 0);

    OHOS::Render3D::Vec3 position(0.0f, 0.0f, 0.0f);
    if (argsPtrItem->Contains("position")) {
        LOGD("Cube position");
        auto positionArgs = argsPtrItem->GetObject("position");
        position.SetX(positionArgs->GetDouble("x", 0.0));
        position.SetY(positionArgs->GetDouble("y", 0.0));
        position.SetZ(positionArgs->GetDouble("z", 0.0));
    }

    LOGD("JSAddCone(%s, %.2f, %d, %d)", name.c_str(), radius, length, sectors);
    ModelView::GetInstance()->AddGeometry(
        AceType::MakeRefPtr<OHOS::Render3D::SVCone>(name.c_str(), radius, length, sectors, position));
}

void JSSceneView::JSGLTFAnimation(const JSCallbackInfo& info)
{
    // Parse the info object.
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("JSSceneView JSGLTFAnimation: arg is invalid.");
        return;
    }

    LOGD("JSGLTFAnimation() info[0]: %s", info[0]->ToString().c_str());
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }

    auto name = argsPtrItem->GetString("name", "");
    auto state = argsPtrItem->GetInt("state", 0); // PLAY
    auto repeatCount = argsPtrItem->GetInt("repeatCount", -1); // -1 is Infinite
    auto speed = argsPtrItem->GetDouble("speed", 1.0);
    auto duration = argsPtrItem->GetDouble("duration", -1.0); // Invalid.
    auto reverse = argsPtrItem->GetBool("reverse", false);

    ModelView::GetInstance()->AddGLTFAnimation(AceType::MakeRefPtr<OHOS::Render3D::GLTFAnimation>(
        name, static_cast<OHOS::Render3D::AnimationState>(state), repeatCount, speed, duration, reverse));
}

void JSSceneView::JSAddCustomRender(const JSCallbackInfo& info)
{
    if (info.Length() < 3) {
        LOGE("addCustomRender() invocation error - Three arguments required");
        return;
    }

    if (info[0]->IsNull() || !info[0]->IsString()) {
        LOGE("addCustomRender() invocation error - URI must be a String");
        return;
    }

    if (info[1]->IsNull() || !info[1]->IsString()) {
        LOGE("addCustomRender() invocation error - native Type name must be a String");
        return;
    }

    if (info[2]->IsNull() || !info[2]->IsBoolean()) {
        LOGE("addCustomRender() invocation error - Needs frame callback flag. Must be a Boolean");
        return;
    }

    RefPtr<OHOS::Render3D::SVCustomRenderDescriptor> desc =
        AceType::MakeRefPtr<OHOS::Render3D::SVCustomRenderDescriptor>(
            info[0]->ToString(), info[1]->ToString(), info[2]->ToBoolean());

    LOGD("JSSceneView::JSaddCustomRender(%s, %s, %s)", desc->GetUri().c_str(),
        desc->GetNativeTypeName().c_str(), (desc->NeedsFrameCallback() ? "true" : "false"));
    ModelView::GetInstance()->AddCustomRender(desc);
}

void JSSceneView::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    ModelView::GetInstance()->SetWidth(value);
}

void JSSceneView::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    ModelView::GetInstance()->SetHeight(value);
}

void JSSceneView::JSBind(BindingTarget globalObj)
{
    LOGD("JSSceneView::JSBind()");
    JSClass<JSSceneView>::Declare("Model");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSceneView>::StaticMethod("create", &JSSceneView::Create, opt);
    JSClass<JSSceneView>::StaticMethod("onClick", &JSSceneView::JsOnClick);
    JSClass<JSSceneView>::StaticMethod("setHandleCameraMove", &JSSceneView::JsSetHandleCameraMove);
    JSClass<JSSceneView>::StaticMethod("camera", &JSSceneView::JSCamera);
    JSClass<JSSceneView>::StaticMethod("transparent", &JSSceneView::JSSetTransparent);
    JSClass<JSSceneView>::StaticMethod("background", &JSSceneView::JSSetBackground);
    JSClass<JSSceneView>::StaticMethod("light", &JSSceneView::JSLight);
    JSClass<JSSceneView>::StaticMethod("addCube", &JSSceneView::JSAddCube);
    JSClass<JSSceneView>::StaticMethod("addSphere", &JSSceneView::JSAddSphere);
    JSClass<JSSceneView>::StaticMethod("addCone", &JSSceneView::JSAddCone);
    JSClass<JSSceneView>::StaticMethod("glTFAnimation", &JSSceneView::JSGLTFAnimation);
    JSClass<JSSceneView>::StaticMethod("addCustomRender", &JSSceneView::JSAddCustomRender);
    JSClass<JSSceneView>::StaticMethod("width", &JSSceneView::JsWidth);
    JSClass<JSSceneView>::StaticMethod("height", &JSSceneView::JsHeight);
    JSClass<JSSceneView>::Inherit<JSViewAbstract>();
    JSClass<JSSceneView>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
