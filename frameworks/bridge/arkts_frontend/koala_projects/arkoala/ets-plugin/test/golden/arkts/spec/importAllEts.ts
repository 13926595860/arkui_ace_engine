import { ArkColumn, ArkCommonMethodComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, stateOf } from "@koalaui/arkts-arkui";
import { MutableState } from "@koalaui/runtime";
import { LocalStorage } from "@koalaui/arkui-common";
import { observableProxy } from "@koalaui/common";
import * as AllComponent from './test/pages/NamespaceComponent';
import TsModule from './test/pages/TsModule';
/** @memo:stable */
class ArkImportTestComponent extends ArkStructBase<ArkImportTestComponent, ImportTestOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: ImportTestOptions): void {
        this.__backing_myState1 = stateOf<any>(initializers?.myState1 ?? (new TsModule(1).method()), this);
        this.__backing_myState2 = stateOf<number>(initializers?.myState2 ?? (0), this);
        this.__backing_myState3 = stateOf<boolean>(initializers?.myState3 ?? (false), this);
        this.__backing_myState4 = stateOf<string>(initializers?.myState4 ?? ('ImportTest'), this);
    }
    private __backing_myState1?: MutableState<any>;
    private get myState1(): any {
        return this.__backing_myState1!.value;
    }
    private set myState1(value: any) {
        this.__backing_myState1!.value = observableProxy(value);
    }
    private __backing_myState2?: MutableState<number>;
    private get myState2(): number {
        return this.__backing_myState2!.value;
    }
    private set myState2(value: number) {
        this.__backing_myState2!.value = observableProxy(value);
    }
    private __backing_myState3?: MutableState<boolean>;
    private get myState3(): boolean {
        return this.__backing_myState3!.value;
    }
    private set myState3(value: boolean) {
        this.__backing_myState3!.value = observableProxy(value);
    }
    private __backing_myState4?: MutableState<string>;
    private get myState4(): string {
        return this.__backing_myState4!.value;
    }
    private set myState4(value: string) {
        this.__backing_myState4!.value = observableProxy(value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: ImportTestOptions) {
        ArkColumn(__builder, () => {
            AllComponent.NamespaceComponent1({
                __backing_NamespaceComponent1Link1: this.__backing_myState1,
                __backing_NamespaceComponent1Link2: this.__backing_myState2,
                __backing_NamespaceComponent1Link3: this.__backing_myState3,
                __backing_NamespaceComponent1Link4: this.__backing_myState4,
                myVar: 100,
                myVar2: 80
            });
            AllComponent.NamespaceComponent1({
                __backing_NamespaceComponent1Link1: this.__backing_myState1,
                __backing_NamespaceComponent1Link2: this.__backing_myState2,
                __backing_NamespaceComponent1Link3: this.__backing_myState3,
                __backing_NamespaceComponent1Link4: this.__backing_myState4,
                myVar: 100,
                myVar2: 80
            })
                .width(100);
            AllComponent.default({
                __backing_NamespaceComponent3Link1: this.__backing_myState1,
                __backing_NamespaceComponent3Link2: this.__backing_myState2,
                __backing_NamespaceComponent3Link3: this.__backing_myState3,
                __backing_NamespaceComponent3Link4: this.__backing_myState4,
                myVar: 100,
                myVar2: 80
            });
            AllComponent.default({
                __backing_NamespaceComponent3Link1: this.__backing_myState1,
                __backing_NamespaceComponent3Link2: this.__backing_myState2,
                __backing_NamespaceComponent3Link3: this.__backing_myState3,
                __backing_NamespaceComponent3Link4: this.__backing_myState4,
                myVar: 100,
                myVar2: 80
            })
                .height(200);
        });
    }
}
export {};
/** @memo */
export function ImportTest(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: ImportTestOptions): void {
    const updatedInitializers: ImportTestOptions = {
        __backing_myState1: initializers?.__backing_myState1,
        __backing_myState2: initializers?.__backing_myState2,
        __backing_myState3: initializers?.__backing_myState3,
        __backing_myState4: initializers?.__backing_myState4
    };
    ArkImportTestComponent._instantiate(style, () => new ArkImportTestComponent, content, updatedInitializers);
}
export interface ImportTestOptions {
    __backing_myState1?: MutableState<any>;
    myState1?: any;
    __backing_myState2?: MutableState<number>;
    myState2?: number;
    __backing_myState3?: MutableState<boolean>;
    myState3?: boolean;
    __backing_myState4?: MutableState<string>;
    myState4?: string;
}
