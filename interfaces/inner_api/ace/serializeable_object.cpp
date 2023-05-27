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

#include "interfaces/inner_api/ace/serializeable_object.h"

#include "utils.h"

namespace OHOS::Ace {
namespace {

#if defined(WINDOWS_PLATFORM)
constexpr char ACE_LIB_NAME[] = "libace.dll";
#elif defined(MAC_PLATFORM)
constexpr char ACE_LIB_NAME[] = "libace.dylib";
#elif defined(LINUX_PLATFORM)
constexpr char ACE_LIB_NAME[] = "libace.so";
#else
constexpr char ACE_LIB_NAME[] = "libace.z.so";
#endif

using CreateFunction = SerializeableObject* (*)();
constexpr char NODE_OBJECT_CREATE_FUNC[] = "OHOS_ACE_CreateNodeObject";

SerializeableObject* CreateNodeObjectInner()
{
    LIBHANDLE handle = LOADLIB(ACE_LIB_NAME);
    if (handle == nullptr) {
        return nullptr;
    }

    auto entry = reinterpret_cast<CreateFunction>(LOADSYM(handle, NODE_OBJECT_CREATE_FUNC));
    if (entry == nullptr) {
        FREELIB(handle);
        return nullptr;
    }

    auto nodeObject = entry();
    FREELIB(handle);
    return nodeObject;
}
} // namespace

std::unique_ptr<SerializeableObject> SerializeableObject::CreateNodeObject()
{
    std::unique_ptr<SerializeableObject> obj;
    obj.reset(CreateNodeObjectInner());
    return obj;
}
} // namespace OHOS::Ace
