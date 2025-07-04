/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// WARNING! THIS FILE IS AUTO-GENERATED, DO NOT MAKE CHANGES, THEY WILL BE LOST ON NEXT GENERATION!

import { SymbolEffect, SymbolEffectInternal } from "./ArkSymbolEffectMaterialized"
import { TypeChecker, ArkUIGeneratedNativeModule } from "#components"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class PulseSymbolEffectInternal {
    public static fromPtr(ptr: KPointer): PulseSymbolEffect {
        const obj : PulseSymbolEffect = new PulseSymbolEffect()
        obj.peer = new Finalizable(ptr, PulseSymbolEffect.getFinalizer())
        return obj
    }
}
export class PulseSymbolEffect extends SymbolEffect implements MaterializedBase {
    static ctor_pulsesymboleffect(): KPointer {
        const retval  = ArkUIGeneratedNativeModule._PulseSymbolEffect_ctor()
        return retval
    }
    constructor() {
        super()
        const ctorPtr : KPointer = PulseSymbolEffect.ctor_pulsesymboleffect()
        this.peer = new Finalizable(ctorPtr, PulseSymbolEffect.getFinalizer())
    }
    static getFinalizer(): KPointer {
        return ArkUIGeneratedNativeModule._PulseSymbolEffect_getFinalizer()
    }
}
