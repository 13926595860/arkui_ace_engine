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

import { StyledString, StyledStringInternal } from "./ArkStyledStringMaterialized"
import { ImageAttachment, ImageAttachmentInternal } from "./ArkImageAttachmentMaterialized"
import { CustomSpan, CustomSpanInternal } from "./ArkCustomSpanMaterialized"
import { StyleOptions, StyledStringKey, SpanStyle } from "./ArkStyledStringInterfaces"
import { TypeChecker, ArkUIGeneratedNativeModule } from "#components"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class MutableStyledStringInternal {
    public static fromPtr(ptr: KPointer): MutableStyledString {
        const obj : MutableStyledString = new MutableStyledString()
        obj.peer = new Finalizable(ptr, MutableStyledString.getFinalizer())
        return obj
    }
}
export class MutableStyledString extends StyledString implements MaterializedBase {
    static ctor_mutablestyledstring(value: string | ImageAttachment | CustomSpan, styles?: Array<StyleOptions>): KPointer {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        if (RuntimeType.STRING == value_type) {
            thisSerializer.writeInt8(0 as int32)
            const value_0  = value as string
            thisSerializer.writeString(value_0)
        }
        else if (TypeChecker.isImageAttachment(value, false, false, false, false, false)) {
            thisSerializer.writeInt8(1 as int32)
            const value_1  = value as ImageAttachment
            thisSerializer.writeImageAttachment(value_1)
        }
        else if (TypeChecker.isCustomSpan(value)) {
            thisSerializer.writeInt8(2 as int32)
            const value_2  = value as CustomSpan
            thisSerializer.writeCustomSpan(value_2)
        }
        let styles_type : int32 = RuntimeType.UNDEFINED
        styles_type = runtimeType(styles)
        thisSerializer.writeInt8(styles_type as int32)
        if ((RuntimeType.UNDEFINED) != (styles_type)) {
            const styles_value  = styles!
            thisSerializer.writeInt32(styles_value.length as int32)
            for (let i = 0; i < styles_value.length; i++) {
                const styles_value_element : StyleOptions = styles_value[i]
                thisSerializer.writeStyleOptions(styles_value_element)
            }
        }
        const retval  = ArkUIGeneratedNativeModule._MutableStyledString_ctor(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
    constructor(value?: string | ImageAttachment | CustomSpan, styles?: Array<StyleOptions>) {
        super()
        if (((value) !== (undefined)) || ((styles) !== (undefined)))
        {
            const ctorPtr : KPointer = MutableStyledString.ctor_mutablestyledstring((value)!, (styles)!)
            this.peer = new Finalizable(ctorPtr, MutableStyledString.getFinalizer())
        }
    }
    static getFinalizer(): KPointer {
        return ArkUIGeneratedNativeModule._MutableStyledString_getFinalizer()
    }
    public replaceString(start: number, length: number, other: string): void {
        const start_casted = start as (number)
        const length_casted = length as (number)
        const other_casted = other as (string)
        this.replaceString_serialize(start_casted, length_casted, other_casted)
        return
    }
    public insertString(start: number, other: string): void {
        const start_casted = start as (number)
        const other_casted = other as (string)
        this.insertString_serialize(start_casted, other_casted)
        return
    }
    public removeString(start: number, length: number): void {
        const start_casted = start as (number)
        const length_casted = length as (number)
        this.removeString_serialize(start_casted, length_casted)
        return
    }
    public replaceStyle(spanStyle: SpanStyle): void {
        const spanStyle_casted = spanStyle as (SpanStyle)
        this.replaceStyle_serialize(spanStyle_casted)
        return
    }
    public setStyle(spanStyle: SpanStyle): void {
        const spanStyle_casted = spanStyle as (SpanStyle)
        this.setStyle_serialize(spanStyle_casted)
        return
    }
    public removeStyle(start: number, length: number, styledKey: StyledStringKey): void {
        const start_casted = start as (number)
        const length_casted = length as (number)
        const styledKey_casted = styledKey as (StyledStringKey)
        this.removeStyle_serialize(start_casted, length_casted, styledKey_casted)
        return
    }
    public removeStyles(start: number, length: number): void {
        const start_casted = start as (number)
        const length_casted = length as (number)
        this.removeStyles_serialize(start_casted, length_casted)
        return
    }
    public clearStyles(): void {
        this.clearStyles_serialize()
        return
    }
    public replaceStyledString(start: number, length: number, other: StyledString): void {
        const start_casted = start as (number)
        const length_casted = length as (number)
        const other_casted = other as (StyledString)
        this.replaceStyledString_serialize(start_casted, length_casted, other_casted)
        return
    }
    public insertStyledString(start: number, other: StyledString): void {
        const start_casted = start as (number)
        const other_casted = other as (StyledString)
        this.insertStyledString_serialize(start_casted, other_casted)
        return
    }
    public appendStyledString(other: StyledString): void {
        const other_casted = other as (StyledString)
        this.appendStyledString_serialize(other_casted)
        return
    }
    private replaceString_serialize(start: number, length: number, other: string): void {
        ArkUIGeneratedNativeModule._MutableStyledString_replaceString(this.peer!.ptr, start, length, other)
    }
    private insertString_serialize(start: number, other: string): void {
        ArkUIGeneratedNativeModule._MutableStyledString_insertString(this.peer!.ptr, start, other)
    }
    private removeString_serialize(start: number, length: number): void {
        ArkUIGeneratedNativeModule._MutableStyledString_removeString(this.peer!.ptr, start, length)
    }
    private replaceStyle_serialize(spanStyle: SpanStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeSpanStyle(spanStyle)
        ArkUIGeneratedNativeModule._MutableStyledString_replaceStyle(this.peer!.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    private setStyle_serialize(spanStyle: SpanStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeSpanStyle(spanStyle)
        ArkUIGeneratedNativeModule._MutableStyledString_setStyle(this.peer!.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    private removeStyle_serialize(start: number, length: number, styledKey: StyledStringKey): void {
        ArkUIGeneratedNativeModule._MutableStyledString_removeStyle(this.peer!.ptr, start, length, styledKey.valueOf())
    }
    private removeStyles_serialize(start: number, length: number): void {
        ArkUIGeneratedNativeModule._MutableStyledString_removeStyles(this.peer!.ptr, start, length)
    }
    private clearStyles_serialize(): void {
        ArkUIGeneratedNativeModule._MutableStyledString_clearStyles(this.peer!.ptr)
    }
    private replaceStyledString_serialize(start: number, length: number, other: StyledString): void {
        ArkUIGeneratedNativeModule._MutableStyledString_replaceStyledString(this.peer!.ptr, start, length, toPeerPtr(other))
    }
    private insertStyledString_serialize(start: number, other: StyledString): void {
        ArkUIGeneratedNativeModule._MutableStyledString_insertStyledString(this.peer!.ptr, start, toPeerPtr(other))
    }
    private appendStyledString_serialize(other: StyledString): void {
        ArkUIGeneratedNativeModule._MutableStyledString_appendStyledString(this.peer!.ptr, toPeerPtr(other))
    }
}
