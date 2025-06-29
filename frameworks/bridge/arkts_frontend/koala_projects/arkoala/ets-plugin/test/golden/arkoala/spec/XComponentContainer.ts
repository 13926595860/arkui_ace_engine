import { ArkColumn, ArkCommonMethodComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, ArkXComponent, LocalStorage, XComponentType } from "@koalaui/arkoala-arkui";
import { registerArkuiEntry } from "@koalaui/arkoala-arkui/ohos.router";
/** @memo:stable */
class ArkHomeComponentComponent extends ArkStructBase<ArkHomeComponentComponent, HomeComponentOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: HomeComponentOptions): void {
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: HomeComponentOptions) {
        ArkColumn(__builder, () => {
            ArkXComponent(undefined, undefined, { id: '1', type: 'component' });
            ArkXComponent(undefined, undefined, { id: '2', type: 1 });
            ArkXComponent(undefined, undefined, { id: '3', type: XComponentType.COMPONENT });
        });
    }
}
export {};
/** @memo */
export function HomeComponent(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: HomeComponentOptions): void {
    const updatedInitializers: HomeComponentOptions = {};
    ArkHomeComponentComponent._instantiate(style, () => new ArkHomeComponentComponent, content, updatedInitializers);
}
export interface HomeComponentOptions {
}
registerArkuiEntry(HomeComponent, "XComponentContainer");
export const __Entry = HomeComponent;
