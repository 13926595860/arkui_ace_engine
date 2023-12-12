/// <reference path="./import.ts" />
/// <reference path="./ArkComponent.ts" />
const FontWeightMap = {
  0: 'lighter',
  1: 'normal',
  2: 'regular',
  3: 'medium',
  4: 'bold',
  5: 'bolder',
  100: '100',
  200: '200',
  300: '300',
  400: '400',
  500: '500',
  600: '600',
  700: '700',
  800: '800',
  900: '900',
}

class ArkButtonComponent extends ArkComponent implements ButtonAttribute {
  constructor(nativePtr: KNode) {
    super(nativePtr);
  }
  onGestureJudgeBegin(callback: (gestureInfo: GestureInfo, event: BaseGestureEvent) => GestureJudgeResult): this {
    throw new Error('Method not implemented.');
  }
  backgroundColor(value: ResourceColor): this {
    modifierWithKey(this._modifiersWithKeys, ButtonBackgroundColorModifier.identity, ButtonBackgroundColorModifier, value);
    return this;
  }
  type (value: ButtonType): this {
    if (isNumber(value)) {
      modifier(this._modifiers, ButtonTypeModifier, value);
    }
    else {
      modifier(this._modifiers, ButtonTypeModifier, undefined);
    }
    return this;
  }
  stateEffect(value: boolean): this {
    if (isBoolean(value)) {
      modifier(this._modifiers, ButtonStateEffectModifier, value);
    } else {
      modifier(this._modifiers, ButtonStateEffectModifier, undefined);
    }
    return this;
  }
  fontColor(value: ResourceColor): this {
    modifierWithKey(this._modifiersWithKeys, ButtonFontColorModifier.identity, ButtonFontColorModifier, value);
    return this;
  }
  fontSize(value: Length): this {
    modifierWithKey(this._modifiersWithKeys, ButtonFontSizeModifier.identity, ButtonFontSizeModifier, value);
    return this;
  }
  fontWeight(value: string | number | FontWeight): this {
    if (typeof value === "string" || typeof value === "number") {
      modifier(this._modifiers, ButtonFontWeightModifier, value);
    } else {
      modifier(this._modifiers, ButtonFontWeightModifier, undefined);
    }
    return this;
  }
  fontStyle(value: FontStyle): this {
    if (typeof value === 'number' && value >= 0 && value < 2) {
      modifier(this._modifiers, ButtonFontStyleModifier, value);
    } else {
      modifier(this._modifiers, ButtonFontStyleModifier, undefined);
    }
    return this;
  }
  fontFamily(value: string | Resource): this {
    modifierWithKey(this._modifiersWithKeys, ButtonFontFamilyModifier.identity, ButtonFontFamilyModifier, value);
    return this;
  }
  labelStyle(value: LabelStyle): this {
    modifierWithKey(this._modifiersWithKeys, ButtonLabelStyleModifier.identity,ButtonLabelStyleModifier, value);
    return this;
  }
}
class ButtonBackgroundColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol("buttonBackgroundColor");
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetBackgroundColor(node);
    } else {
      GetUINativeModule().button.setBackgroundColor(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    if (isResource(this.stageValue) && isResource(this.value)) {
      return !isResourceEqual(this.stageValue, this.value);
    } else {
      return true;
    }
  }
}
class ButtonStateEffectModifier extends Modifier<boolean> {
  constructor(value: boolean) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonStateEffect');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetStateEffect(node);
    }
    else {
      GetUINativeModule().button.setStateEffect(node, this.value);
    }
  }
}
class ButtonFontStyleModifier extends Modifier<number> {
  constructor(value: number) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonFontStyle');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetFontStyle(node);
    }
    else {
      GetUINativeModule().button.setFontStyle(node, this.value);
    }
  }
}
class ButtonFontFamilyModifier extends ModifierWithKey<string | Resource> {
  constructor(value: string | Resource) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonFontFamily');
  applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        GetUINativeModule().button.resetFontFamily(node);
      }
      else {
        GetUINativeModule().button.setFontFamily(node, this.value);
      }
  }
  checkObjectDiff(): boolean {
    if (isResource(this.stageValue) && isResource(this.value)) {
      return !isResourceEqual(this.stageValue, this.value);
    }
    else {
      return true;
    }
  }
}
class ButtonLabelStyleModifier extends ModifierWithKey<LabelStyle> {
  constructor(value: LabelStyle) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonLabelStyle');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
        GetUINativeModule().button.resetLabelStyle(node);
    }
    else {
      let textOverflow = this.value.overflow; // number -> Ace::TextOverflow
      let maxLines = this.value.maxLines; // number -> uint32_t
      let minFontSize = this.value.minFontSize; // number/string -> Dimension
      let maxFontSize = this.value.maxFontSize; // number/string -> Dimension
      let heightAdaptivePolicy = this.value.heightAdaptivePolicy; // number -> Ace::TextHeightAdaptivePolicy
      let fontSize = undefined; // number/string ->Dimension
      let fontWeight = undefined; // string -> Ace::FontWeight
      let fontStyle = undefined; // number -> Ace::FontStyle
      let fontFamily = undefined; // string ->std::vector<std::string>
      if (isObject(this.value.font))
      {
        fontSize = this.value.font.size;
        fontWeight = 'normal';
        fontStyle = this.value.font.style;
        fontFamily = this.value.font.family;
        if (typeof this.value.font.weight === "string") {
          fontWeight = this.value.font.weight;
        } else {
          if (this.value.font.weight in FontWeightMap) {
            fontWeight = FontWeightMap[this.value.font.weight];
          }
      }
      GetUINativeModule().button.setLabelStyle(node, textOverflow, maxLines, minFontSize, maxFontSize,
        heightAdaptivePolicy, fontSize, fontWeight, fontStyle, fontFamily);
      }
    }
  }
  checkObjectDiff(): boolean {
    if (isResource(this.stageValue) && isResource(this.value)) {
      return !isResourceEqual(this.stageValue, this.value);
    }else if(!isResource(this.stageValue) && !isResource(this.value)){
      return !(this.value.overflow === this.stageValue.overflow &&
        this.value.maxLines === this.stageValue.maxLines &&
        this.value.minFontSize === this.stageValue.minFontSize &&
        this.value.maxFontSize === this.stageValue.maxFontSize  &&
        this.value.heightAdaptivePolicy === this.stageValue.heightAdaptivePolicy &&
        this.value.font === this.stageValue.font)
    }
    else {
      return true;
    }
  }
}
class ButtonTypeModifier extends Modifier<number> {
  constructor(value: number) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonType');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetType(node);
    }
    else {
      GetUINativeModule().button.setType(node, this.value);
    }
  }
}
class ButtonFontColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonFontColor');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetFontColor(node);
    }
    else {
      GetUINativeModule().button.setFontColor(node, this.value);
    }
  }
  
  checkObjectDiff(): boolean {
    if (isResource(this.stageValue) && isResource(this.value)) {
      return !isResourceEqual(this.stageValue, this.value);
    } 
    else {
      return true;
    }
  }
}
class ButtonFontSizeModifier extends ModifierWithKey<Length> {
  constructor(value: Length) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonFontSize');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetFontSize(node);
    }
    else {
      GetUINativeModule().button.setFontSize(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    if (isResource(this.stageValue) && isResource(this.value)) {
      return !isResourceEqual(this.stageValue, this.value);
    }
    else {
      return true;
    }
  }
}
class ButtonFontWeightModifier extends Modifier<string | number | FontWeight> {
  constructor(value: string | number | FontWeight) {
    super(value);
  }
  static identity: Symbol = Symbol('buttonFontWeight');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().button.resetFontWeight(node);
    }
    else {
      let fontWeightStr = 'normal';
      if (typeof this.value === "string") {
        fontWeightStr = this.value;
      } else {
        if (this.value in FontWeightMap) {
          fontWeightStr = FontWeightMap[this.value];
        }
      }
      GetUINativeModule().button.setFontWeight(node, fontWeightStr);
    }
  }
}
// @ts-ignore
globalThis.Button.attributeModifier = function (modifier) {
  const elmtId = ViewStackProcessor.GetElmtIdToAccountFor();
  let nativeNode = GetUINativeModule().getFrameNodeById(elmtId);
  let component = this.createOrGetNode(elmtId, ()=> {
    return new ArkButtonComponent(nativeNode);
  });
  modifier.applyNormalAttribute(component);
  component.applyModifierPatch();
}
